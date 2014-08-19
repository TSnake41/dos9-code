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

#include "Dos9_Type.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* TYPE file

   Type the file 'file'

*/

/* FIXME : Make TYPE understand joker lines. ie type *.txt
   should print all text files in the current directory
*/

int Dos9_CmdType(DOS9CONTEXT* pContext, char* lpLine)
{
	char lpFileName[FILENAME_MAX];
	FILE* pFile;

	while (lpLine=Dos9_GetNextParameter(pContext, lpLine+4, lpFileName, FILENAME_MAX)) {

		if (!strcmp(lpFileName, "/?")) {

			Dos9_ShowInternalHelp(pContext, DOS9_HELP_TYPE);
			return 0;

		} else if ((pFile=fopen(lpFileName, "r"))) {

			while (fgets(lpFileName, FILENAME_MAX, pFile))
				fprintf(pContext->pStack->out, "%s", lpFileName);

			fclose(pFile);

			return 0;

		}

	}

	Dos9_ShowErrorMessageX(pContext,
                            DOS9_FILE_ERROR,
                            lpFileName
                            );
	return -1;

}

