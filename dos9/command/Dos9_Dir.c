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

#include "Dos9_Dir.h"

typedef struct DIRCONTEXT {
    DOS9CONTEXT* pContext;
    int iDirNb;
    int iFileNb;
    char bSimple;
    short wAttr;
} DIRCONTEXT;

void Dos9_CmdDirShow(FILELIST* lpElement, DIRCONTEXT* pDir)
{
	char lpType[]="D RHSA ",
	              lpSize[16];

	struct tm* lTime;

	/* This structures get only one argument at a time,
	   thus ``lpElement->lpNext'' is inconsistent */

	if (Dos9_CheckFileAttributes(pDir->wAttr, lpElement)) {

		/* if the file has right attributes */

		if (!pDir->bSimple) {

			strcpy(lpType, "       ");
			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_DIR) {

				lpType[0]='D';
				pDir->iDirNb++;

			} else {

				pDir->iFileNb++;

			}

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_READONLY) lpType[2]='R';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_HIDDEN) lpType[3]='H';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_SYSTEM) lpType[4]='S';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_ARCHIVE) lpType[5]='A';

			/* !! Recyclage de la variable lpFilename pour afficher la taille du fichier */
			Dos9_FormatFileSize(lpSize, 16, Dos9_GetFileSize(lpElement));

			lTime=localtime(&Dos9_GetModifTime(lpElement));

            /* fixme : Truncate the name at the basis dir if not in simple mode */

			fprintf(pDir->pContext->pStack->out,
                    "%02d/%02d/%02d %02d:%02d\t%s\t%s\t%s\n",
                    lTime->tm_mday,
                    lTime->tm_mon+1,
                    1900+lTime->tm_year,
                    lTime->tm_hour,
                    lTime->tm_min,
                    lpSize, lpType,
                    lpElement->lpFileName);

		} else {

			fprintf(pDir->pContext->pStack->in, "%s\n", lpElement->lpFileName);

		}
	}
}

int Dos9_CmdDir(DOS9CONTEXT* pContext, char* lpLine)
{
	char *lpNext,
	     *lpToken,
	     lpFileName[FILENAME_MAX]= {0},
	     bSimple=0;

    short wAttr=0;

	int iFlag=DOS9_SEARCH_DEFAULT | DOS9_SEARCH_DIR_MODE;

	ESTR* lpParam=Dos9_EsInit();

	//if (lpNext=strchr(lpLine, ' ')) *lpNext='\0';
	lpNext=lpLine+3;

	wAttr=DOS9_CMD_ATTR_ALL;
	bSimple=FALSE;

	DIRCONTEXT dir_context;

    dir_context.iDirNb = 0;
    dir_context.iFileNb = 0;
	dir_context.pContext = pContext;

	while ((lpNext=Dos9_GetNextParameterEs(pContext, lpNext, lpParam))) {

		lpToken=Dos9_EsToChar(lpParam);

		if (!strcmp(lpToken, "/?")) {

			Dos9_ShowInternalHelp(pContext, DOS9_HELP_DIR);
			return 0;

		} else if (!stricmp("/b", lpToken)) {

			/* use the simple dir output */
			bSimple=TRUE;

			if (!wAttr)
                iFlag|=DOS9_SEARCH_NO_STAT;

			iFlag|=DOS9_SEARCH_NO_PSEUDO_DIR;

		} else if (!stricmp("/s", lpToken)) {

			/* set the command to recusive */
			iFlag|=DOS9_SEARCH_RECURSIVE;

		} else if (!strnicmp("/a", lpToken,2)) {

			/* uses the attributes command */
			lpToken+=2;

            if (*lpToken==':')
                lpToken++;

			wAttr=Dos9_MakeFileAttributes(lpToken);
			iFlag&=~DOS9_SEARCH_NO_STAT;

		} else {

			if (*lpFileName) {
				Dos9_ShowErrorMessageX(pContext,
                                        DOS9_UNEXPECTED_ELEMENT,
                                        lpToken
                                        );
				Dos9_EsFree(lpParam);

				return -1;

			}

			snprintf(lpFileName, FILENAME_MAX, "%s", lpToken);

		}
	}

	if (!*lpFileName) {

		/* if no file or directory name have been specified
		   the put a correct value in it */

		*lpFileName='*';
		lpFileName[1]='\0';

	}

	/* do a little global variable setup before
	   starting file research */

	if (!bSimple)
        fputs(lpDirListTitle, pContext->pStack->out);

    dir_context.wAttr = wAttr;
    dir_context.bSimple = bSimple;

	/* Get a list of file and directories matching to the
	   current filename and options set */
	if (!(Dos9_GetMatchFileCallback(lpFileName, iFlag,
                                Dos9_CmdDirShow, &dir_context))
	    && !bSimple)
		fputs(lpDirNoFileFound, pContext->pStack->out);

	if (!bSimple)
        fprintf(pContext->pStack->out,
                    lpDirFileDirNb, dir_context.iFileNb, dir_context.iDirNb);

	Dos9_EsFree(lpParam);

	return 0;
}
