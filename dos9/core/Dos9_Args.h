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

#ifndef DOS9_ARGS_H
#define DOS9_ARGS_H

#include <wchar.h>

#include "Dos9_Core.h"

/* a structure that contains boundaries of a block */

typedef struct BLOCKINFO {
	wchar_t* lpBegin;
	wchar_t* lpEnd;
} BLOCKINFO;

wchar_t* Dos9_GetNextParameterEs(wchar_t* lpLine, ESTR* lpReturn);
wchar_t* Dos9_GetNextParameter(wchar_t* lpLine, wchar_t* lpResponseBuffer, int iLength);
int   Dos9_GetParamArrayEs(wchar_t* lpLine, ESTR** lpArray, size_t iLenght);
wchar_t* Dos9_GetNextBlockEs(wchar_t* lpLine, ESTR* lpReturn);
wchar_t* Dos9_GetNextBlock(wchar_t* lpLine, BLOCKINFO* lpbkInfo);
wchar_t* Dos9_GetEndOfLine(wchar_t* lpLine, ESTR* lpReturn);

#endif // DOS9_ARGS_H
