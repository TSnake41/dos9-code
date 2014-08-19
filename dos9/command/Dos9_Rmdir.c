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

#include "Dos9_Rmdir.h"

#include "Dos9_Ask.h"

/* RD [/S] [/Q] dir ...
   RMDIR [/S] [/Q] dir ...

   removes the 'dir' directory. It may also suppress severals directories specified in a row.

   /S : suppress subfolders and subfiles.
   /Q : Quiet for while browsing subfolders.

   */

int Dos9_CmdRmdir(DOS9CONTEXT* pContext, char* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();

	char  dirname[FILENAME_MAX];

	int	bQuiet=FALSE,
		bRecursive=FALSE,
		iChoice;

	if (!(lpLine=Dos9_GetNextParameterEs(pContext,
                                            lpLine, lpEstr))) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_EXPECTED_MORE,
                                "RD/RMDIR"
                                );
		goto error;

	}

	while ((lpLine=Dos9_GetNextParameterEs(pContext, lpLine, lpEstr))) {

		if (!strcmp(Dos9_EsToChar(lpEstr), "/?")) {

			Dos9_ShowInternalHelp(pContext, DOS9_HELP_RD);
			break;

		} else if (!stricmp(Dos9_EsToChar(lpEstr),"/S")) {

			/* FIXME : Empty the given directory and then try to suppress
			   the directory */

			bRecursive=TRUE;

		} else if (!stricmp(Dos9_EsToChar(lpEstr), "/Q")) {

			bQuiet=TRUE;

		} else {

			if (!bQuiet && bRecursive) {

				iChoice=Dos9_AskConfirmation(pContext,
                                                DOS9_ASK_YNA
                                                    | DOS9_ASK_INVALID_REASK
                                                    | DOS9_ASK_DEFAULT_Y,
												lpRmdirConfirm,
												Dos9_EsToChar(lpEstr));

				if (iChoice == DOS9_ASK_NO) {

					/* Do not remove the current directory, and look for
					   another directory in the command line */
					continue;

				} else if (iChoice == DOS9_ASK_ALL) {

					bQuiet=TRUE;

				}

			}

			Dos9_AbsolutePath(dirname,
                                sizeof(dirname),
                                pContext->lpCurrentDir,
                                Dos9_EsToChar(lpEstr),
                                );

			if (rmdir(dirname)) {

				Dos9_ShowErrorMessageX(pContext,
                                        DOS9_RMDIR_ERROR | DOS9_PRINT_C_ERROR,
										Dos9_EsToChar(lpEstr)
										);
				goto error;

			}

		}

	}

	Dos9_EsFree(lpEstr);
	return 0;

error:
	Dos9_EsFree(lpEstr);
	return -1;
}
