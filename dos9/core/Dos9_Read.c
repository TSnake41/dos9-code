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
#include <string.h>
#include <wchar.h>

#include <libDos9.h>
#include <libw.h>

#include "Dos9_Core.h"


//#define DOS9_DBG_MODE

#include "Dos9_Debug.h"

int Dos9_GetLine(ESTR* lpesLine, INPUT_FILE* pIn)
{
	FILE* pFile;
	ESTR* lpesTmp=Dos9_EsInit();

	wchar_t bCorrectBlocks=FALSE;
	wchar_t* lpCh;
	int   res;

	if (*(pIn->lpFileName) == L'\0') {

		pFile=stdin;

	} else if (!(pFile=wfopen(pIn->lpFileName, L"r"))) {

		Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
		                      pIn->lpFileName,
		                      FALSE);
		goto error;

	} else {

		fseek(pFile, pIn->iPos, SEEK_SET);

	}

	*(Dos9_EsToChar(lpesLine))=L'\0';

	while (!(res=Dos9_EsGet(lpesTmp, pFile))) {

		lpCh=Dos9_SkipAllBlanks(Dos9_EsToChar(lpesTmp));

		/* split comments label and void lines from input */
		if (*lpCh!=':' && *lpCh!=L'\n')
			Dos9_EsCatE(lpesLine, lpesTmp);

		if (Dos9_CheckBlocks(lpesLine) == TRUE) {

			bCorrectBlocks=TRUE;
			break;

		}
	}

	DOS9_DBG(L"-------------------[READ]--------------------------\n"
	         L"%s\n"
	         L"---------------------------------------------------\n",
	         Dos9_EsToChar(lpesLine)
	        );

	//getch();

	if (*(pIn->lpFileName)!=L'\0') {

		pIn->bEof=res;

		if (!res)
			pIn->iPos=ftell(pFile);

		fclose(pFile);

	}



	if (!*Dos9_EsToChar(lpesLine)) {

		Dos9_EsFree(lpesTmp);
		return 1;

	}

	if (!bCorrectBlocks) {

		Dos9_ShowErrorMessage(DOS9_NONCLOSED_BLOCKS, NULL, FALSE);
		goto error;

	}

	Dos9_EsFree(lpesTmp);

	return 0;

error:
	pIn->bEof=TRUE;
	Dos9_EsFree(lpesTmp);
	return -1;

}

int Dos9_CheckBlocks(ESTR* lpesLine)
{
	wchar_t *lpBlock,
			*lpCh=Dos9_EsToChar(lpesLine);

	if (!*lpCh)
		return TRUE;

	DOS9_DBG(L"--------------------------------------------------------------\n");

	if (!(lpBlock=Dos9_GetNextBlockBegin(lpCh))) {

		/* There's no block, thus all is fine */

		if (wcschr(lpCh, L'\n')
		    && !Dos9_SearchChar(lpCh, L'\n'))
			return FALSE;

		return TRUE;

	}

	/* lpBlockBegin we have is the highest level block
	   that is avaliable, just perform some check for it */

	if (!(lpCh=Dos9_GetBlockLineEnd(lpBlock))) {

		/* the syntax is broken */

		return FALSE;

	}

	if (wcschr(lpCh, L'\n')
	    && !Dos9_SearchChar(lpCh, L'\n'))
		return FALSE;

	return TRUE;

}

void Dos9_RmTrailingNl(wchar_t* lpLine)
{
	wchar_t cLastChar=0;

	while (*lpLine) {

		cLastChar=*(lpLine++);

	}

	if (cLastChar=='\n')
		*(lpLine-1)='\0';

}
