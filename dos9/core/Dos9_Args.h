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

#include "Dos9_Core.h"

/* a structure that contains boundaries of a block */

typedef struct BLOCKINFO {
	char* lpBegin;
	char* lpEnd;
} BLOCKINFO;

int   Dos9_GetParameterPointers(char** lpPBegin, char** lpPEnd,
				 const char* lpDelims, const char* lpLine);

#define Dos9_GetNextParameterEs(pContext, lpLine, lpReturn) \
    Dos9_GetNextParameterEsD(pContext, lpLine, lpReturn, " ;,\t\"")

char* Dos9_GetNextParameterEsD(DOS9CONTEXT* pContext, char* lpLine,
                                ESTR* lpReturn, const char* lpDelims);

char* Dos9_GetNextParameter(DOS9CONTEXT* pContent,
                    char* lpLine, char* lpResponseBuffer, int iLength);

int   Dos9_GetParamArrayEs(DOS9CONTEXT* pContext, char* lpLine,
                                ESTR** lpArray, size_t iLenght);

char* Dos9_GetNextBlockEs(DOS9CONTEXT* pContext, char* lpLine,
                                                        ESTR* lpReturn);

char* Dos9_GetNextBlock(char* lpLine, BLOCKINFO* lpbkInfo);

char* Dos9_GetEndOfLine(DOS9CONTEXT* pContext, char* lpLine,
                                                        ESTR* lpReturn);

#endif // DOS9_ARGS_H
