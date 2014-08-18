/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "Dos9_Stream.h"
#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

STREAMSTACK* Dos9_PushStreamStack(STREAMSTACK* lpssStreamStack)
{
    STREAMSTACK* ret;

    /* allocate a new block */
    if (!(ret=malloc(sizeof(STREAMSTACK)))) {

        return NULL;

    }

    if (lpssStreamStack != NULL) {

        /* copy the content of the preceding block */
        memcpy(ret, lpssStreamStack, sizeof(STREAMSTACK));

        /* initialize the content of the tofree and lock to 0 */
        memset(&(ret->tofree), NULL, STREAMSTACK_FREE_BUFSIZ*sizeof(FILE*));

        /* This could be done through the previous call, but we
           should not rely on the hope that struct will not change.
           It may indeed be a very hard to detect error */
        ret->freeindex = 0;
        ret->lock =0;

        ret->next = lppsStreamStack;

    } else {

        /* initialize the struc */
        memset(ret, 0, sizeof(STREAMSTACK));

    }

    return ret;

}

STREAMSTACK* Dos9_PopStreamStack(STREAMSTACK* lpssStreamStack)
{
    STREAMSTACK* ret;
    int i;

    /* do not pop the current element if the lock is activated */
    if (lpssStreamStack->lock)
        return lpssStreamStack;

    /* get the previous block */
    ret = lpssStreamStack->next;

    for (i=0;i < lpssStreamStack->freeindex; i++) {

        if (lpssStreamStack->tofree[i]) {

            fclose(lpssStreamStack->tofree[i]);

        }

    }

    free(lpssStreamStack);

    return ret;
}

int     Dos9_OpenOutput(STREAMSTACK* lpssStreamStack,
                            PARSED_STREAM_START* lpssStart)
{

    FILE* pFile;

    if (lpssStart->lpInputFile==NULL /* no file for stdin */
        || lpssStart->lpOutputFile==NULL /* no file for stdout */
        || lpssStart->lpErrorFile==NULL /* no file for stderr */
        || !(lpssStart->cOutputMode & PSTREAMSTART_REDIR_ERR)
            /* stderr is not redirected into stdout */
        )
            return 0;

    /* pushes the stack */
    if (!(lpssStreamStack=Dos9_PushStreamStack(lppsStreamStack))
        return -1;

    if (lpssStart->lpInputFile) {

        pFile = fopen(lpssStart->lpInputFile, "r");

        if (!pFile)
            return -1;

        /* setup free functions */
        lpssStreamStack->in = pFile;
        lpssStreamStack->tofree[ lpssStreamStack->index ++ ] = pFile;

    }

    if (lpssStart->lpOutputFile) {

        pFile = fopen(lpssStart->lpOutputFile,
                        (lpssStart->cOutputMode & PSTREAMSTART_TRUNC_OUT) ?
                        ("w") : ("a"));

        if (!pFile)
            return -1;

        /* setup free functions */
        lpssStreamStack->out = pFile;
        lpssStreamStack->tofree[ lpssStreamStack->index ++ ] = pFile;


    }

    if (lpssStart->lpErrorFile) {

        pFile = fopen(lpssStart->lpErrorFile,
                        (lpssStart->cOutputMode & PSTREAMSTART_TRUNC_ERR) ?
                        ("w") : ("a"));

        if (!pFile)
            return -1;

        /* setup free functions */
        lpssStreamStack->out = pFile;
        lpssStreamStack->tofree[ lpssStreamStack->index ++ ] = pFile;

    }

    if (lpssStart->cOutputMode & PSTREAMSTART_REDIR_ERR)
        lpssStreamStack->err = lpssStreamStack->out;

    if (lpssStart->cOutputMode & PSTREAMSTART_REDIR_OUT)

    return 0;

}

int     Dos9_OpenPipe(STREAMSTACK* lpssStreamStack)
{
    int fd[2];
    FILE* pFile;


    /* Open the pipe */
    if (pipe(fd) == -1)
        return -1;


    /* convert the descriptors to FILE */

    pFile = fdopen(fd[0], "r"); /* get the read end of the pipe */

    if (!pFile) {

        close(fd[1]);
        close(fd[2]);
        return -1;

    }

    if (!(lpssStreamStack=Dos9_PushStreamStack(lpssStreamStack))) {

        fclose(pFile);
        close(fd[1]);
        return -1;

    }

    lpssStreamStack->in = pFile;
    lpssStreamStack->tofree[lpssStreamStack->freeindex ++] = pFile;

    if (!(lpssStreamStack=Dos9_PushStreamStack(lpssStreamStack)) {

        close(fd[1]);
        return -1;

    }

    if (!(pFile=fdopen(fd[1], "w"))) {

        close(fd[1]);
        return -1;

    }

    lpssStreamStack->out = pFile;
    lpssStreamStack->tofree[lpssStreamStack->freeindex ++] = pFile;

    return 0;
}

int     Dos9_RedirectStreams(STREAMSTACK* lpssStack)
{
    dup2(fileno(lpssStreamStack->in), STDIN_FILENO);
    dup2(fileno(lpssStreamStack->out), STDOUT_FILENO);
    dup2(fileno(lpssStreamStack->err), STDERR_FILENO);
}
