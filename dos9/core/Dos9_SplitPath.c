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

#include <string.h>
#include <wchar.h>

#include <libw.h>

#include "Dos9_Core.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

void Dos9_SplitPath(wchar_t* lpPath,
                    wchar_t* lpDisk, wchar_t* lpDir, wchar_t* lpName, wchar_t* lpExt)
{

	wchar_t* lpNextToken, *lpToken=lpPath;
	wchar_t cSaveChar;

	if (*lpToken) {

		if (!wcsncmp(lpToken+1, L":\\", 2)) {

			DOS9_DBG(L"Found disk_name=\"%c\"\n", *lpToken);

			if (lpDisk)
				snwprintf(lpDisk, _MAX_DRIVE, L"%c:\\", *lpToken);

			lpToken+=3;

		} else {

			if (lpDisk)
				*lpDisk=L'\0';

		}


	} else {

		if (lpDisk)
			*lpDisk=L'\0';

	}

	if ((lpNextToken = Dos9_SearchLastToken(lpToken, L"\\/"))) {

		lpNextToken++;
		cSaveChar=*lpNextToken;
		*lpNextToken=L'\0';

		DOS9_DBG(L"found path=\"%s\"\n", lpToken);

		if (lpDir)
			wcsncpy(lpDir, lpToken, _MAX_DIR);

		*lpNextToken=cSaveChar;
		lpToken=lpNextToken;

	} else {

		if (lpDir)
			*lpDir=L'\0';

	}

	if ((lpNextToken = Dos9_SearchLastChar(lpToken, L'.'))) {

		cSaveChar=*lpNextToken;
		*lpNextToken=L'\0';

		DOS9_DBG(L"found name=\"%s\"\n", lpToken);


		if (lpName)
			wcsncpy(lpName, lpToken, _MAX_FNAME);

		*lpNextToken=cSaveChar;
		lpToken=lpNextToken+1;

		DOS9_DBG(L"found ext=\"%s\"\n", lpToken);

		if (lpExt)
			snwprintf(lpExt, _MAX_EXT, L".%s", lpToken);

	} else {

		if (lpName)
			wcsncpy(lpName, lpToken, _MAX_FNAME);

		if (lpExt)
			*lpExt=L'\0';

	}

}
