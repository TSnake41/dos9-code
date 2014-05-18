/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2013 DarkBatcher
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
#include <math.h>
#include <string.h>
#include <wchar.h>

#include <libDos9.h>

#include <libw.h>

#include "Dos9_Conditions.h"
#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"

int Dos9_CmdIf(wchar_t* lpParam)
{
	wchar_t lpArgument[FILENAME_MAX],
			*lpNext,
			*lpToken,
			*lpErrorLvl;

	int iFlag=0,
	    iResult,
	    iExp1,
	    iExp2;

	CMPTYPE cmpCompType;

	ESTR *lpComparison, *lpOtherPart;

	LPFILELIST lpflFileList;

	/* possible syntaxes.
	        IF [/i] [NOT] test==valeur (...)
	        IF [/i] test2 CMP test2 (...)
	        IF [NOT] EXIST fichier (...)
	        IF [NOT] DEFINED var (...)
	        IF [NOT] ERRORLEVEL code (...) -> DEPRECATED !
	*/

	BLOCKINFO bkInfo;

	double x1,
	       x2;

	lpParam+=2;

	if ((lpNext=Dos9_GetNextParameter(lpParam, lpArgument, 11))) {

		if (!wcscmp(lpArgument, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_IF);
			return 0;

		}

		if (!wcscasecmp(lpArgument, L"/i")) {

			/* this is case insensitive */
			iFlag|=DOS9_IF_CASE_UNSENSITIVE;
			lpParam=lpNext;

			if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

				Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
				return 0;

			}

		}

		if (!wcscasecmp(lpArgument, L"NOT")) {

			/* this is negative */
			iFlag|=DOS9_IF_NEGATION;
			lpParam=lpNext;

			if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

				Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
				return 0;

			}

		}

		if (!wcscasecmp(lpArgument, L"EXIST")) {

			iFlag|=DOS9_IF_EXIST;

		} else if (!wcscasecmp(lpArgument, L"DEFINED")) {

			iFlag|=DOS9_IF_DEFINED;

		} else if (!wcscasecmp(lpArgument, L"ERRORLEVEL")) {

			iFlag|=DOS9_IF_ERRORLEVEL;

		}

		if ((iFlag & DOS9_IF_CASE_UNSENSITIVE)
		    && (iFlag & (DOS9_IF_ERRORLEVEL | DOS9_IF_EXIST | DOS9_IF_DEFINED))
		   ) {

			Dos9_ShowErrorMessage(DOS9_INCOMPATIBLE_ARGS,
									L"'/i' (DEFINED, EXIST, ERRORLEVEL)",
									FALSE);
			return 0;

		}

		if (iFlag & (DOS9_IF_EXIST | DOS9_IF_DEFINED | DOS9_IF_ERRORLEVEL))
			lpParam=lpNext;

	}

	if (iFlag & DOS9_IF_ERRORLEVEL) {

		/* the script used 'ERRORLEVEL' Keyword */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
			return -1;

		}

		if (!(lpErrorLvl=wgetenv(L"ERRORLEVEL"))) {

			Dos9_ShowErrorMessage(DOS9_UNABLE_GET_VARIABLE | DOS9_PRINT_C_ERROR,
									L"ERRORLEVEL",
									FALSE);

		}

		iResult=!wcscmp(lpErrorLvl, lpArgument);

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;


	} else if (iFlag & DOS9_IF_EXIST) {

		/* the script used 'EXIST' keyWord */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
			return -1;

		}

		lpflFileList=Dos9_GetMatchFileList(lpArgument, DOS9_SEARCH_DEFAULT);

		iResult=(lpflFileList == NULL) ? FALSE : TRUE;

		if (lpflFileList)
			Dos9_FreeFileList(lpflFileList);

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;

	} else if (iFlag & DOS9_IF_DEFINED) {

		/* the script used 'DEFINED' */
		if (!(lpNext=Dos9_GetNextParameter(lpParam, lpArgument, FILENAME_MAX))) {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
			return -1;

		}

		/* avoid errors from conversion from 64 bits to
		   32 bits (if we get memory beyond 0xFFFFFFFF) */
		iResult=(wgetenv(lpArgument) == NULL) ? FALSE : TRUE;

		if (iFlag & DOS9_IF_NEGATION)
			iResult=!iResult;

		lpParam=lpNext;

	} else {

		/* the script uses normal comparisons */
		lpComparison=Dos9_EsInit();

		if ((lpNext=Dos9_GetNextParameterEs(lpParam, lpComparison))) {

			if ((lpToken=wcsstr(Dos9_EsToChar(lpComparison), L"=="))) {
				/* if scipt uses old c-style comparison */
				*lpToken=L'\0';
				lpToken+=2;

				if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

					iResult=!wcscasecmp(Dos9_EsToChar(lpComparison), lpToken);

				} else {

					iResult=!wcscmp(Dos9_EsToChar(lpComparison), lpToken);
				}

				if (iFlag & DOS9_IF_NEGATION)
					iResult=!iResult;


			} else {
				/* if script uses new basic-like comparison (e.g. EQU, NEQ ...) */

				if (!(lpNext=Dos9_GetNextParameter(lpNext, lpArgument, 11))) {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
					return -1;

				}

				if (!wcscasecmp(lpArgument, L"EQU")) {

					cmpCompType=CMP_EQUAL;

				} else if (!wcscasecmp(lpArgument, L"NEQ")) {

					cmpCompType=CMP_DIFFERENT;

				} else if (!wcscasecmp(lpArgument, L"GEQ")) {

					cmpCompType=CMP_GREATER_OR_EQUAL;

				} else if (!wcscasecmp(lpArgument, L"GTR")) {

					cmpCompType=CMP_GREATER;

				} else if (!wcscasecmp(lpArgument, L"LEQ")) {

					cmpCompType=CMP_LESSER_OR_EQUAL;

				} else if (!wcscasecmp(lpArgument, L"LSS")) {

					cmpCompType=CMP_LESSER;

				} else if (!wcscasecmp(lpArgument, L"FEQ")) {

					cmpCompType=CMP_FLOAT_EQUAL;

				} else {

					Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
											lpArgument,
											FALSE);

					return -1;

				}

				lpOtherPart=Dos9_EsInit();

				if (!(lpNext=Dos9_GetNextParameterEs(lpNext, lpOtherPart))) {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", TRUE);
					return -1;

				}

				switch (cmpCompType) {

					case CMP_EQUAL:

						/* this is to be fixed. First we should ensure that
						   the given tokens are not numbers, if they are
						   actually, then we must ensure that they're equal */

						if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

							iResult=!wcscasecmp(Dos9_EsToChar(lpOtherPart),
													Dos9_EsToChar(lpComparison));

						} else {

							iResult=!wcscasecmp(Dos9_EsToChar(lpOtherPart),
													Dos9_EsToChar(lpComparison));

						}

						break;

					case CMP_DIFFERENT:

						/* this is to be fixed. First we should ensure that
						   the given tokens are not numbers, if they are
						   actually, then we must ensure that they're
						   different */

						if (iFlag & DOS9_IF_CASE_UNSENSITIVE) {

							iResult=wcscasecmp(Dos9_EsToChar(lpOtherPart),
												Dos9_EsToChar(lpComparison));

						} else {

							iResult=wcscmp(Dos9_EsToChar(lpOtherPart),
												Dos9_EsToChar(lpComparison));

						}

						break;

					case CMP_GREATER:

						iResult = (wtof(Dos9_EsToChar(lpComparison))
										> wtof(Dos9_EsToChar(lpOtherPart)));

						break;

					case CMP_GREATER_OR_EQUAL:

						iResult = (wtof(Dos9_EsToChar(lpComparison))
										>= wtof(Dos9_EsToChar(lpOtherPart)));

						break;

					case CMP_LESSER:

						iResult = (wtof(Dos9_EsToChar(lpComparison))
										< wtof(Dos9_EsToChar(lpOtherPart)));

						break;

					case CMP_LESSER_OR_EQUAL:

						iResult = (wtof(Dos9_EsToChar(lpComparison))
										<= wtof(Dos9_EsToChar(lpOtherPart)));

						break;

					case CMP_FLOAT_EQUAL:
						x1 = frexp(wtof(Dos9_EsToChar(lpComparison)), &iExp1);
						x2 = frexp(wtof(Dos9_EsToChar(lpOtherPart)), &iExp2);

						/* majorate the error */
						iResult=(fabs(x1-x2*pow(2, iExp2-iExp1)) <= DOS9_FLOAT_EQUAL_PRECISION);

				}

				Dos9_EsFree(lpOtherPart);
			}

		} else {

			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
			Dos9_EsFree(lpComparison);
			return -1;

		}

		lpParam=lpNext;
		Dos9_EsFree(lpComparison);

	}

	if ((lpNext=Dos9_GetNextBlock(lpParam, &bkInfo))) {

		if (iResult) {

			Dos9_RunBlock(&bkInfo);

		} else {

			if (!wcsncmp(lpNext, L") ELSE (", sizeof(") ELSE (")-1)) {

				lpNext+=(sizeof(") ELSE (")-2);

				if ((lpToken=Dos9_GetNextBlock(lpNext, &bkInfo))) {

					/* if we found an else */
					Dos9_RunBlock(&bkInfo);

				} else {

					Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);

				}
			}
		}

	} else {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"IF", FALSE);
		return -1;

	}

	return 0;
}
