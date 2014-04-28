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

#ifndef DOS9_FILEPATH_H
#define DOS9_FILEPATH_H

#include <wchar.h>

#ifdef _POSIX_C_SOURCE

#define DOS9_PATH_DELIMITER ':'

#else

#define DOS9_PATH_DELIMITER ';'

#endif

int			Dos9_GetFilePath(wchar_t* lpFullPath, const wchar_t* lpPartial, size_t iBufSize);
wchar_t*	Dos9_GetPathNextPart(wchar_t* lpPath, ESTR* lpReturn);
int			Dos9_MakePath(ESTR* lpReturn, int nOps, ...);
wchar_t*	Dos9_GetLastChar(ESTR* lpReturn);

#endif // DOS9_FILEPATH_H
