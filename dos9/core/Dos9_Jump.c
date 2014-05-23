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
 */

#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

#include <errno.h>
#include <string.h>
#include <wchar.h>

#include <libw.h>

int Dos9_JumpToLabel(wchar_t* lpLabelName, wchar_t* lpFileName)
{
	size_t iSize=wcslen(lpLabelName);
	wchar_t* lpName=lpFileName;
	FILE* pFile;
	ESTR* lpLine=Dos9_EsInit();


	if ((lpFileName==NULL)) {
		lpName=ifIn.lpFileName;
	}


	if (!(pFile=wfopen(lpName, L"r"))) {

		Dos9_EsFree(lpLine);

		return -1;
	}

	while (!Dos9_EsGet(lpLine, pFile)) {

		if (!wcsncasecmp(Dos9_EsToChar(lpLine), lpLabelName, iSize)) {

			if (lpFileName) {

				/* at that time, we can assume that lpFileName is not
				   the empty string, because the empty string is not usually
				   a valid file name */

				if (*lpFileName==L'/'
				    || !wcsncasecmp(L":/", lpFileName+1, 2)
				    || !wcsncasecmp(L":\\", lpFileName+1, 2)) {

					/* the path is absolute */
					wcsncpy(ifIn.lpFileName,
							lpFileName,
							sizeof(ifIn.lpFileName)/sizeof(wchar_t)
							);

					ifIn.lpFileName[FILENAME_MAX-1]=L'\0';
					/* wcsncpy do not ensure that a terminating null character
					   is written to string, just that the number of characters
					   copied will not exceed the given size. */

				} else {

					/* the path is relative */
					swprintf(ifIn.lpFileName,
					          sizeof(ifIn.lpFileName)/sizeof(wchar_t),
					          L"%s/%s",
					          Dos9_GetCurrentDir(),
					          lpFileName
					          );

				}

			}

			ifIn.iPos=ftell(pFile);
			ifIn.bEof=feof(pFile);

			fclose(pFile);

			Dos9_EsFree(lpLine);

			return 0;
		}
	}

	fclose(pFile);
	Dos9_EsFree(lpLine);

	return -1;
}

