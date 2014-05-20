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
#include <stdarg.h>
#include <wchar.h>

#include <libw.h>
#include <libDos9.h>

#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#include "Dos9_FilePath.h"

int Dos9_GetFilePath(wchar_t* lpFullPath, const wchar_t* lpPartial, size_t iBufSize)
{
	ESTR* lpEsTmp=Dos9_EsInit();
	ESTR* lpEsPart=Dos9_EsInit();
	ESTR* lpEsFinalPath=Dos9_EsInit();

	wchar_t *lpPathToken=wgetenv(L"PATH");

	int   bFirstLoop=TRUE;

#ifdef WIN32

	wchar_t *lpPathExtToken,
	        *lpPathExtBegin=wgetenv(L"PATHEXT");
	int bFirstSubLoop;

#endif // WIN32

	DOS9_DBG(L"[Dos9_GetFilePath()]*** Start research of file : \"%s\"\n\n", lpPartial);
	DOS9_DBG(L"[Dos9_GetFilePath()] PATH variable content=[\n%s\n]\n\n", wgetenv(L"PATH"));
	DOS9_DBG(L"[Dos9_GetFilePath()] PATHEXT variable content=[\n%s\n]\n\n", wgetenv(L"PATHEXT"));

	do {

		if (bFirstLoop) {

			Dos9_EsCpy(lpEsTmp, lpPartial);
			bFirstLoop=FALSE;

		} else {

			Dos9_MakePath(lpEsTmp, 2, Dos9_EsToChar(lpEsPart), lpPartial);

		}

		DOS9_DBG(L"\t[*] trying \"%s\"\n", Dos9_EsToChar(lpEsTmp));

#ifdef WIN32

		bFirstSubLoop=TRUE;
		lpPathExtToken=lpPathExtBegin;

		do {

			if (bFirstSubLoop) {

				Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
				bFirstSubLoop=FALSE;

			} else {

				Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
				Dos9_EsCatE(lpEsFinalPath, lpEsPart);

			}

			DOS9_DBG(L"\t\t[*] trying \"%s\"\n", Dos9_EsToChar(lpEsFinalPath));

			if (Dos9_FileExists(Dos9_EsToChar(lpEsFinalPath)))
				goto file_found;

			if (lpPathExtToken == NULL)
				break;



		} while ((lpPathExtToken=Dos9_GetPathNextPart(lpPathExtToken, lpEsPart)));

#else

		if (Dos9_FileExists(Dos9_EsToChar(lpEsTmp))) {

			Dos9_EsCpyE(lpEsFinalPath, lpEsTmp);
			goto file_found;

		}

#endif

		if (lpPathToken == NULL)
			break;


	} while ((lpPathToken=Dos9_GetPathNextPart(lpPathToken, lpEsPart)));

	Dos9_EsFree(lpEsPart);
	Dos9_EsFree(lpEsTmp);
	Dos9_EsFree(lpEsFinalPath);

	DOS9_DBG(L"[Dos9_GetFilePath()]*** Finished without match (-1)\n");

	return -1;

file_found:

	snwprintf(lpFullPath ,
			  iBufSize,
			  L"%s",
	          Dos9_EsToChar(lpEsFinalPath)
			  );

	DOS9_DBG(L"[Dos9_GetFilePath()]*** Found \"%s\"\n", Dos9_EsToChar(lpEsFinalPath));

	Dos9_EsFree(lpEsPart);
	Dos9_EsFree(lpEsTmp);
	Dos9_EsFree(lpEsFinalPath);

	return 0;
}


wchar_t* Dos9_GetPathNextPart(wchar_t* lpPath, ESTR* lpReturn)
{
	wchar_t*   lpNextToken;

	if (*lpPath==L'\0')
		return NULL;

	if (!(lpNextToken=wcschr(lpPath, DOS9_PATH_DELIMITER))) {

		Dos9_EsCpy(lpReturn, lpPath);

		while (*lpPath)
			lpPath++;

		return lpPath;

	}

	Dos9_EsCpyN(lpReturn,
	            lpPath,
	            lpNextToken - lpPath);

	return ++lpNextToken;
}

int Dos9_MakePath(ESTR* lpReturn, int nOps, ...)
{
	va_list vaList;

	int     i;

	wchar_t *lpBegin,
	        *lpEnd;

	va_start(vaList, nOps);

	Dos9_EsCpy(lpReturn, L"");

	for (i=0; i<nOps; i++) {

		lpBegin=Dos9_EsToChar(lpReturn);

		lpEnd=Dos9_GetLastChar(lpReturn);

		if (*lpBegin!=L'\0'
		    && *lpEnd!=L'\\'
		    && *lpEnd!=L'/') {

			/* if there are no dir terminating characters and still
			   arguments, just cat a '/' */

			Dos9_EsCat(lpReturn, L"/");

		}

		Dos9_EsCat(lpReturn, va_arg(vaList,wchar_t*));

	}

	va_end(vaList);

	return 0;
}

wchar_t* Dos9_GetLastChar(ESTR* lpReturn)
{

	wchar_t* lpCh=Dos9_EsToChar(lpReturn);

	if (*lpCh==L'\0')
		return lpCh;

	while (*(lpCh+1))
		lpCh++;

	return lpCh;

}
