/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "Dos9_Clone.h"
#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

/* Duplicate the current instance of Dos9 (much like fork() in unix systems) */
THREAD Dos9_CloneInstance(void(*func)(void*), void* arg)
{
    struct clone_data_t* data;
    THREAD th;

    if ((data = malloc(sizeof(struct clone_data_t))) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_CloneInstance()", -1);

    Dos9_DuplicateData(data);
    data->fn = func;
    data->arg = arg;

    if (Dos9_BeginThread(&th, Dos9_CloneTrampoline, 0, data))
        Dos9_ShowErrorMessage(DOS9_FAILED_FORK | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_CloneInstance()", -1);

    return th;
}

void* Dos9_CloneTrampoline(void* data)
{
    struct clone_data_t* cloned = data;
    void(*func)(void*);
    void* arg;

    bDelayedExpansion = cloned->bDelayedExpansion;
    bUseFloats = cloned->bUseFloats;
    bEchoOn = cloned->bEchoOn;
    iErrorLevel = cloned->iErrorLevel;
    bIsScript = cloned->bIsScript;
    bCmdlyCorrect = cloned->bCmdlyCorrect;
    bAbortCommand = cloned->bAbortCommand;
    lpclCommands = cloned->lpclCommands;
    lpvLocalVars = cloned->lpvLocalVars;
    lpvArguments = cloned->lpvArguments;
    dsDirStack = cloned->dsDirStack;
    lppsStreamStack = cloned->lppsStreamStack;
    colColor = cloned->colColor;
    lpeEnv = cloned->lpeEnv;
    strncpy(ifIn.lpFileName, cloned->ifIn.lpFileName, FILENAME_MAX);
    strncpy(lpCurrentDir, cloned->lpCurrentDir, FILENAME_MAX);
    ifIn.bEof = cloned->ifIn.bEof;
    ifIn.iPos = cloned->ifIn.iPos;
    *(ifIn.batch.name) = '\0';
    fInput = cloned->fInput;
    fOutput = cloned->fOutput;
    fError = cloned->fError;
    _fError = fError;
    _fOutput = fOutput;

    lpAltPromptString = NULL;

    func = cloned->fn;
    arg = cloned->arg;

    free(data);

    func(arg);

    Dos9_Exit();

    Dos9_EndThread((void*)iErrorLevel);

    return (void*)iErrorLevel;
}

/* Duplicate Dos9 internal structures */
int Dos9_DuplicateData(struct clone_data_t* data)
{
    int fd;

    data->bDelayedExpansion = bDelayedExpansion;
    data->bUseFloats = bUseFloats;
    data->bEchoOn = bEchoOn;
    data->iErrorLevel = iErrorLevel;
    data->bIsScript = bIsScript;
    data->bCmdlyCorrect = bCmdlyCorrect;
    data->bAbortCommand = bAbortCommand;

    if ((data->lpclCommands = Dos9_DuplicateCommandList(lpclCommands)) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                               __FILE__ "/Dos9_DuplicateData()", -1);

    data->lpvLocalVars = Dos9_DuplicateLocalVar(lpvLocalVars);
    data->lpvArguments = Dos9_DuplicateLocalVar(lpvArguments);

    Dos9_DirStackCopy(&data->dsDirStack);

    data->lppsStreamStack = NULL;
    data->colColor = colColor;
    data->lpeEnv = Dos9_EnvDup(lpeEnv);
    strncpy(data->lpCurrentDir, lpCurrentDir, FILENAME_MAX);
    strncpy(data->ifIn.lpFileName, ifIn.lpFileName, FILENAME_MAX);
    data->ifIn.bEof = ifIn.bEof;
    data->ifIn.iPos = ifIn.iPos;

#define DUPLICATE_STREAM(dest, s, mode) \
    if ((fd = dup(fileno(s))) == -1) \
        Dos9_ShowErrorMessage(DOS9_UNABLE_DUPLICATE_FD \
                               | DOS9_PRINT_C_ERROR, \
                               __FILE__ "/Dos9_DuplicateData()", -1); \
    \
    if ((dest = fdopen(fd, mode)) == NULL) \
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION \
                                | DOS9_PRINT_C_ERROR, \
                                __FILE__ "/Dos9_DuplicateData()", -1)

    DUPLICATE_STREAM(data->fInput, fInput, "r");
    DUPLICATE_STREAM(data->fOutput, fOutput, "w");
    DUPLICATE_STREAM(data->fError, fError, "w");

    Dos9_SetFdInheritance(fileno(data->fInput), 0);
    Dos9_SetFdInheritance(fileno(data->fOutput), 0);
    Dos9_SetFdInheritance(fileno(data->fError), 0);

    return 0;
}
