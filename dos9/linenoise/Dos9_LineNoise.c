/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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

#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>
#include "../core/Dos9_Core.h"

#ifndef WIN32

#include "linenoise.h"
#include "utf8.h"

/* something similar to Dos9_EsGet() (ie. return 1 at eof) */
int Dos9_LineNoise(ESTR* estr, FILE* pFile)
{
    char* line;
    if (Dos9_LockMutex(&mLineNoise))
        Dos9_ShowErrorMessage(DOS9_LOCK_MUTEX_ERROR,
                              __FILE__ "/Dos9_LineNoise()" , -1);


    linenoiseSetEncodingFunctions(
        linenoiseUtf8PrevCharLen,
        linenoiseUtf8NextCharLen,
        linenoiseUtf8ReadCode);
    linenoiseSetMultiLine(1);

    line = linenoise(NULL);
    if (line)
        linenoiseHistoryAdd(line);


    if (Dos9_ReleaseMutex(&mLineNoise))
        Dos9_ShowErrorMessage(DOS9_RELEASE_MUTEX_ERROR,
                              __FILE__ "/Dos9_LineNoise()" , -1);

    if (line == NULL)
        return 1;


    Dos9_EsCpy(estr, line);

    return 0;
}

#endif /* !WIN32 */
