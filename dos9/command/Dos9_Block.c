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

#include "Dos9_Block.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

/* Handle a top-level block

     ( code )

    Code that may mix redirections

 */

int Dos9_CmdBlock(DOS9CONTEXT* pContext, char* lpLine)
{
	BLOCKINFO bkCode;
	char* lpToken;

	lpToken=Dos9_GetNextBlock(lpLine, &bkCode);

	if (!lpToken) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_INVALID_TOP_BLOCK,
                                lpLine
                                );
		return -1;

	}

	if (*lpToken!=')') {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_INVALID_TOP_BLOCK,
                                lpLine
                                );
		return -1;

	}

	lpToken++;

	lpToken=Dos9_SkipBlanks(lpToken);


	if (*lpToken) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_INVALID_TOP_BLOCK,
                                lpLine
                                );
		return -1;

	}

	Dos9_RunBlock(pContext, &bkCode);

	return iErrorLevel;
}

