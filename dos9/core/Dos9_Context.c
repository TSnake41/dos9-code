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
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>
#include "Dos9_Core.h"

DOS9CONTEXT* Dos9_InitContext(COMMANDINFO* pInfo, int nb, char **env);
{
    DOS9CONTEXT* pContext;

    /* allocate the context */
    if (!(pContext=malloc(sizeof(DOS9CONTEXT))))
        goto error;

    /* Initialize the local var block and the STREAMSTACK member */
    if (!(pContext->pLocalVars = Dos9_GetLocalBlock())
        || !(pContext->pStack = Dos9_PushStreamStack(NULL))
        || !(pContext->pIn = malloc(sizeof(INPUT_FILE)))
        || !(pContext->pCommands = Dos9_MapCommandInfo(pInfo, nb))) {

error:
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_InitContext()",
                                -1);

        /* we already exited */

    }

    /* Initialize the INPUT_FILE member */
    pContext->pIn->bEof=0;
    pContext->pIn->iPos=0;
    *(pContext->pIn->lpFileName)='\0';

    /* Initialize the STREAMSTACK member */
    pContext->pStack->err = stderr; /* use stderr as error output*/
    pContext->pStack->in = stdin; /* use stdin as input */
    pContext->pStack->out = stdout; /* use stdout as output */
    pContext->pStack->freeindex = 0; /* free nothing when poping*/
    pContext->pStack->index = 0; /* redirect nothing more */
    pContext->pStack->lock = 1; /* prevent this element from being poped */

    /* initializes the current directory pointer */
    getcwd(pContext->lpCurrentDir, FILENAME_MAX);

    pContext->iErrorLevel = 0;
    pContext->iLastErrorLevel = 0;

    /* initializes the environment variable */
    pContext->pEnv = Dos9_InitEnv(env);

    return pContext;

}

DOS9CONTEXT* Dos9_DuplicateContext(DOS9CONTEXT* pContext)
{
    DOS9CONTEXT pNewContext;

    int i;

    LOCAL_VAR_BLOCK *pLocal;

    if (!(pNewContext = malloc(sizeof(DOS9CONTEXT))))
        goto error;

    if (!(pNewContext->pStack = Dos9_PushStreamStack(NULL))
        || !(pNewContext->pLocalVars = Dos9_GetLocalBlock())
        || !(pNewContext->pIn = malloc(sizeof(INPUT_FILE)))
        || !(pNewContext->pCommands
                = Dos9_DuplicateCommandList(pContext->pCommands))
        ) {

        /* do not care if we do not free allocated memory, failed
           allocation error can not be recovered after all. Freeing
           memory does not matter if we exit immediately */

error:
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_DuplicateContext()",
                                -1
                                );

    }

    /* in fact we do not really duplicate the stack, we only copy the top
       element, removing specification of FILE structures to be freed and
       setting the lock on.

       We do so in order to make sure that corruption nevers occurs, for
       example, if a file is closed, it can prevent other stacks that
       contains pointers to elements that are not valid anymore to work
       the right way.

       It would be far to slow to create a copy of *every* single file
       opened on the stack, thus, we do not make copies, and prefer
       reusing last level of stack, without free information.

       However, it forces you to *never* use simultaneously the duplicated
       context and the original context. It also forces you to use the
       duplicated context *before* the original context. Finally, if you
       want really independent contexts, make two duplicates of the
       original context, these two new context can be used simultaneously by
       various threads.

    */

    /* copy the content of the STREAMSTACK */
    pNewContext->pStack->in = pContext->pStack->in;
    pNewContext->pStack->out = pContext->pStack->out;
    pNewContext->pStack->err = pContext->pStack->err;

    /* Remove all file to be freed when poping the stack */
    memset(pNewContext->pStack->tofree, 0,
                STREAMSTACK_FREE_BUFSIZ*sizeof(FILE*));

    pNewContext->pStack->freeindex = 0;
    pNewContext->pStack->index = 0;

    /* lock the streamstack */
    pNewContext->pStack->lock = 1;

    /* copy the redirections */
    memcpy(pNewContext->pStack->redir, pContext->pStack->redir,
                STREAMSTACK_BUFSIZ*sizeof(struct _STREAMSTACK_REDIRECT));


    /* copy the content of the INPUT_FILE struct */
    memcpy(pNewContext->pIn, pContext->pIn, sizeof(INPUT_FILE));

    /* copy the content of the LOCAL_VAR_BLOCK */
    memcpy(pNewContext->pIn, pContext->pIn, sizeof(LOCAL_VAR_BLOCK));

    /* this time, we duplicate all the content of the LOCAL_VAR_BLOCK
       size. */
    for (i=0, pLocal=pNewContext->pLocalVars;i < LOCAL_VAR_BLOCK_SIZE;i++) {

        /* this is lawful because the C standard guarantees that operations
           will be evaluated in a sequential way (&& is a sequence point) */
        if ((pLocal[i] != NULL)
            && !(pLocal[i]=strdup(pLocal[i])) {

            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_DuplicateContext()",
                                    -1,
                                    );

        }

    }

    return pNewContext;

}

void Dos9_FreeContext(DOS9CONTEXT* pContext)
{
    /* free the local block */
    Dos9_FreeLocalBlock(pContext->pLocalVars);

    /* pop every element from the STREAMSTACK */
    while (pContext->pStack) {

        pContext->pStack->lock = 0; /* unlock poping lock */
        pContext->pStack = Dos9_PopStreamStack(pContext->pStack);

    }

    /* free the INPUT_FILE Structure */
    free(pContext->pIn);

    free(pContext);

}


