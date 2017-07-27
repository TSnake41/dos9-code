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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Exit.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

int Dos9_CmdExit(char* lpLine)
{
    ESTR* param=Dos9_EsInit();
    char* ntoken;
    int ret=0;

	if ((ntoken=Dos9_GetNextParameterEs(lpLine+4, param))) {

		if (!stricmp(param->str, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_EXIT);
			Dos9_EsFree(param);
			return 0;

		} else if (!stricmp(param->str, "/b")) {

            /* /b allow resuming to upper executiong level, with or without
               specifying a return value in ERRORLEVEL */

            if ((ntoken = Dos9_GetNextParameterEs(ntoken, param))) {

                if (*ntoken) {

                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, ntoken, 0);
                    Dos9_EsFree(param);
                    return -1;

                }

                Dos9_SetEnv(lpeEnv, "ERRORLEVEL", param->str);

            }

            Dos9_EsFree(param);
            bAbortCommand = -1;
            return 0;

		} else {

			ret = atoi(ntoken);

		}
	}

    Dos9_Exit();

	if (bIgnoreExit)
        Dos9_EndThread(ret);
    else
        exit(ret);

	return 0;
}
