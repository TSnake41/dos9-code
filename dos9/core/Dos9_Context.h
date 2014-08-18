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

#ifndef DOS9_CONTEXT_H
#define DOS9_CONTEXT_H

#include <libDos9.h>
#include "Dos9_Core.h"

#define DOS9_CONTEXT_DELAYED_EXPANSION  (0x01)
#define DOS9_CONTEXT_USE_FLOATS         (0x02)
#define DOS9_CONTEXT_CMDLYCORRECT       (0x04)
#define DOS9_CONTEXT_ABORT_COMMAND      (0x08)
#define DOS9_CONTEXT_ABORT_FILE         (0x10)
#define DOS9_CONTEXT_ECHO_ON            (0x20)
#define DOS9_CONTEXT_ABORT              (0x18)

/* This structure is designed in order to make Dos9 more
   flexible and to make it possible to have something
   *actually* thread safe.

   The context also stores a pointer the current directory,
   aiming to be able to have multiple threads running
   independently in different current working directories.
   This is included in an effort to make parallelization
   achievable. */

typedef struct DOS9CONTEXT {
    STREAMSTACK* pStack; /* redirection stack */
    LOCAL_VAR_BLOCK* pLocalVars; /* array for local variables */
    INPUT_FILE*  pIn; /* information on the loaded file */
    COMMANDLIST* pCommands; /* information about the command available*/
    ENVBUF* pEnv; /* information about environment variable */
    char lpCurrentDir[FILENAME_MAX];
    int iMode;
    int iErrorLevel;
    int iLastErrorLevel;
} DOS9CONTEXT;

DOS9CONTEXT* Dos9_InitContext(COMMANDINFO* pInfo, int nb, char** env);
DOS9CONTEXT* Dos9_DuplicateContext(DOS9CONTEXT* pContext);
void Dos9_FreeContext(DOS9CONTEXT* pContext);

//int Dos9_InitContext(DOS9_CONTEXT* lpdcContext);
//int Dos9_CloseContext(DOS9_CONTEXT* lpdcContext);

#endif // DOS9_CONTEXT_H
