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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Commands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "Dos9_Ren.h"

#include "Dos9_Ask.h"

/* Rename a file

    REN origin dest

        origin : a pointer

*/

int Dos9_CmdRen(DOS9CONTEXT* pContext, char* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();
	char lpFileName[FILENAME_MAX],
         lpDir[FILENAME_MAX],
         lpFileDest[FILENAME_MAX];

	char* lpToken;

    lpLine +=3;

	if (*lpLine=='a' || *lpLine=='A')
        lpLine +=3;

	if ((lpLine=Dos9_GetNextParameterEs(pContext, lpLine, lpEstr))) {

        Dos9_AbsolutePath(lpFileName,
                            sizeof(lpFileName),
                            pContext->lpCurrentDir,
                            Dos9_EsToChar(lpEstr));

		strcpy(lpDir, lpFileName);

		if ((lpLine=Dos9_GetNextParameterEs(pContext, lpLine, lpEstr))) {

			/* removing old filename */
			lpLine=strrchr(lpDir, '\\');
			lpToken=strrchr(lpDir, '/'); /* retrieve the original path */

			if (lpToken>lpLine) {
				lpLine=lpToken;
			}

			if (lpLine) {
				lpLine++;
				*lpLine='\0';
			}

			Dos9_AbsolutePath(lpFileDest,
                                sizeof(lpFileDest),
                                lpDir,
                                Dos9_EsToChar(lpEstr)
                                );

			/* cat with new name */
			if (!printf("<DEBUG> renaming `%s` to `%s`\n", lpFileName, lpFileDest)) {

				Dos9_ShowErrorMessageX(pContext,
                                        DOS9_UNABLE_RENAME,
                                        lpFileName);

				Dos9_EsFree(lpEstr);
				return -1;

			}

			return 0;
		}
	}

	Dos9_ShowErrorMessageX(pContext,
                            DOS9_EXPECTED_MORE,
                            "REN / RENAME"
                            );
	Dos9_EsFree(lpEstr);

	return -1;
}
