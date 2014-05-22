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

#include <libDos9.h>

#include "Dos9_Core.h"
#include "Dos9_ShowIntro.h"
#include "../lang/Dos9_Lang.h"

#ifdef WIN32
#include <windows.h>

void Dos9_InitConsole(void)
{
	CONSOLE_CURSOR_INFO cciCursorInfo;
	cciCursorInfo.dwSize=100;
	cciCursorInfo.bVisible=TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cciCursorInfo);
}

#else

void Dos9_InitConsole(void) {}

#endif


void Dos9_PrintIntroduction(void)
{
	wchar_t lpDesc[]=L"                                         ";
	wchar_t lpAuth[]=L"                                      ";

	size_t iSize=sizeof(lpDesc)/sizeof(wchar_t);

#ifdef DOS9_BUILDCOMMENTS

#define DOS9_DESC_TOKEN L"Dos9 [" DOS9_VERSION L"] - " DOS9_BUILDCOMMENTS

#else

#define DOS9_DESC_TOKEN L"Dos9 [" DOS9_VERSION L"]"

#endif

	if (sizeof(DOS9_DESC_TOKEN)/sizeof(wchar_t)-1 < iSize)
		iSize=sizeof(DOS9_DESC_TOKEN)/sizeof(wchar_t)-1;

	wcsncpy(lpDesc, DOS9_DESC_TOKEN , iSize);


	iSize=sizeof(lpAuth);
	if (sizeof(DOS9_AUTHORS)/sizeof(wchar_t)-1 < iSize)
		iSize=sizeof(DOS9_AUTHORS)/sizeof(wchar_t)-1;

	wcsncpy(lpAuth, DOS9_AUTHORS, iSize);

	Dos9_SetConsoleTextColor(DOS9_BACKGROUND_IBLUE | DOS9_FOREGROUND_IYELLOW);

	wprintf(L"  ______   _______  _______   _____                                            \n\
 (  __  \\ (  ___  )(  ____ \\ / ___ \\  %s\n\
 | (  \\  )| (   ) || (    \\/( (   ) ) Copyright (c) 2010-" DOS9_BUILDYEAR "                  \n\
 | |   ) || |   | || (_____ ( (___) |    %s\n\
 | |   | || |   | |(_____  ) \\____  |                                          \n\
 | |   ) || |   | |      ) |      ) | This is free software, you can modify    \n\
 | (__/  )| (___) |/\\____) |/\\____) ) and/or redistribute it under the terms   \n\
 (______/ (_______)\\_______)\\______/  of the GNU Genaral Public License V3     \n\
                                                                               \n",
	       lpDesc,
	       lpAuth
	      );

	Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
	printf("\n");
}
