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

#include "Dos9_Cd.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* Change current directory

   CD [/d] [drive:][/directory]

   Changes current directory.

        [drive:][/directory] : The new directory path

        /d : Forces current disk change. Indeed, when you specify
        a distinct drive path without specifying '/d', the CD command
        only sets a diferent path for these drives, without actually
        changing the current path

 */

/* FIXME : Make it more compatible with cmd.exe
   In fact, cmd.exe supports different paths on differents drives
   through variables like %=x:% where x is the name of the drive.
   So that the /d switch is *really* usefull.
*/

int Dos9_CmdCd(DOS9CONTEXT* pContext, char* lpLine)
{
	char* lpNext;
	ESTR* lpEsDir=Dos9_EsInit();

    lpLine+=2;

	if (*lpLine!=' ' && *lpLine!='.'  && *lpLine!='\t' && *lpLine=='/'
        && *lpLine!=',' && *lpLine!=';') {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_BAD_COMMAND_LINE,
                                NULL
                                );
        goto error;

	}

	if ((lpNext=Dos9_GetNextParameterEs(pContext, lpLine, lpEsDir))) {

		if (!strcmp(Dos9_EsToChar(lpEsDir), "/?")) {

			Dos9_ShowInternalHelp(pContext, DOS9_HELP_CD);
			goto error;

		} else if (!stricmp(Dos9_EsToChar(lpEsDir), "/d")) {

			lpLine=lpNext;

		}

		lpLine=Dos9_SkipBlanks(lpLine);

		Dos9_GetEndOfLine(pContext, lpLine, lpEsDir);

		lpLine=Dos9_EsToChar(lpEsDir);

        /* remove trailing characters */

		lpNext=NULL;

		while (*lpLine) {

			switch(*lpLine) {
				case '\t':
				case ' ':

					if (!lpNext) lpNext=lpLine;
					break;

				default:
					lpNext=NULL;
			}

			lpLine++;

		}

		if (lpNext) *lpNext='\0';

		lpLine=Dos9_EsToChar(lpEsDir);

		DOS9_DBG("Changing directory to : \"%s\"\n", lpLine);

		/* don't change the OS actual current working directory.
           Indeed, various thread may be working on different
           paths, so intempestive use of path is not tolerable.
           In addition, the POSIX standards do not require
           chdir to be thread-safe or reentrant. Change it at
           the very execution time */

        snprintf(pContext->lpCurrentDir, FILENAME_MAX, "%s", lpLine);


	} else {

		fputs(pContext->lpCurrentDir, pContext->pStack->out);


	}

	Dos9_EsFree(lpEsDir);

	DOS9_DBG("Returning from \"cd\" on success\n");
	return 0;

error:

	Dos9_EsFree(lpEsDir);
	return -1;
}

