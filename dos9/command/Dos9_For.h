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

#ifndef DOS9_FOR_H
#define DOS9_FOR_H

#include <wchar.h>

#define TOKENINFO_NB_MAX 128-32
#define MAX_UNICODE_CHAR 5
#define TOKENNB_ALL_REMAINING 0xFFFF
#define ALL_TOKEN 0xFFFF

#define HIGHWORD(nb) (((nb) & 0xFFFF0000) >> 16)
#define LOWWORD(nb) ((nb) & 0xFFFF)
#define TOHIGH(nb) ((nb) <<16)
#define TOLOW(nb) ((nb) & 0xFFFF)

#define FOR_LOOP_SIMPLE 0
#define FOR_LOOP_R 1
#define FOR_LOOP_F 2
#define FOR_LOOP_L 4
#define FOR_LOOP_D 3

typedef struct FORINFO {
	wchar_t lpDelims[FILENAME_MAX];
	/*  the delims */
	wchar_t lpEol[FILENAME_MAX];
	/* the eol character, in a unicode character */
	int iSkip;
	/* the number of lines to be skipped */
	wchar_t cFirstVar;
	/* position of the first special var */
	int bUsebackq;
	int iTokenNb;
	/* the max value of token information */

	int	lpToken[TOKENINFO_NB_MAX];
	/* an array that describes characteristics of the
	   different special var. The syntax is the following :

	    [high order word] [low order word]
	    [     start     ] [     end      ]

	    - if the high order word is set, then the token to
	      be considered should be the cast of a range of tokens,
	      starting from the p-th token up to n-th token, where
	      p is the number stored in the high order word, and n
	      the number stored in the low order word

	    - if the low order word is set to -1 (0xFFFF), then
	      the token is the cast of the p-th and all remaining
	      tokens.

		This supposes that int are at least 32bits long, but it
		does not requires int to be exactly 32bits long.

	*/

} FORINFO;

typedef struct STRINGINFO {
	wchar_t* lpString;
	/* the location of the original string. Used
	   for freeing memory */
	wchar_t* lpToken;
	/* a token to be processed */
} STRINGINFO;

#define INPUTINFO_TYPE_STREAM 0
#define INPUTINFO_TYPE_STRING 1
#define INPUTINFO_TYPE_COMMAND 2 // unused in the struct

union _INPUTINFO_UNION {
	STRINGINFO StringInfo;
	FILE* pInputFile;
};

typedef struct INPUTINFO {
	char cType;
	union _INPUTINFO_UNION Info;
} INPUTINFO;


int Dos9_CmdFor(wchar_t* lpCommand);
/* This is the function used for running
   ``for'' loops */

int Dos9_CmdForSimple(ESTR* lpInput, BLOCKINFO* lpbkCommand, wchar_t cVarName, wchar_t* lpDelimiters);
/* this handles simple ``for'' loops */

int Dos9_CmdForF(ESTR* lpInput, BLOCKINFO* lpbkCommand, FORINFO* lpfrInfo);
/* this handles more comple ``for'' loops
   ie. For /F
*/

#define DOS9_FORL_BEGIN 0
#define DOS9_FORL_INC   1
#define DOS9_FORL_END   2

int Dos9_CmdForL(ESTR* lpInput, BLOCKINFO* lpbkCommand, wchar_t cVarName);
/* this handle the ``for /L'' loop */


int Dos9_ForMakeInfo(wchar_t* lpOptions, FORINFO* lpfrInfo);
/* this makes token from a parameter command
   i.e. from a string like "tokens=1,3,4 delims=, "
*/

void Dos9_ForAdjustParameter(wchar_t* lpOptions, ESTR* lpParam);

int  Dos9_ForMakeTokens(wchar_t* lpToken, FORINFO* lpfrInfo);

void Dos9_ForSplitTokens(ESTR* lpContent, FORINFO* lpfrInfo);
/*
   this split a line into different tokens
   for parsing with tokens

*/

void Dos9_ForGetToken(ESTR* lpContent, FORINFO* lpfrInfo, int iPos, ESTR* lpReturn);
/*
    gets token from lpContent, and in
    the order of iRange.
*/


int Dos9_ForMakeInputInfo(ESTR* lpInput, INPUTINFO* lpipInfo, FORINFO* lpfrInfo);

int Dos9_ForAdjustInput(wchar_t* lpInput);

int Dos9_ForInputProcess(ESTR* lpInput, INPUTINFO* lpipInfo, int* iPipeFdIn, int* iPipeFdOut);

int Dos9_ForGetInputLine(ESTR* lpReturn, INPUTINFO* lpipInfo);

void Dos9_ForCloseInputInfo(INPUTINFO* lpipInfo);

int Dos9_ForGetStringInput(ESTR* lpReturn, STRINGINFO* lpsiInfo);

void Dos9_ForVarUnassign(FORINFO* lpfrInfo);

int  Dos9_ForVarCheckAssignment(FORINFO* lpfrInfo);

int  Dos9_CmdForDeprecatedWrapper(ESTR* lpMask, ESTR* lpDir, wchar_t* lpAttribute, BLOCKINFO* lpbkCode, wchar_t cVarName);

#endif /* DOS9_FOR_H */
