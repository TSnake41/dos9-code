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
#include <wchar.h>
#include <wctype.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>
#include <libw.h>

#include "../core/Dos9_Core.h"

#include "Dos9_ScriptCommands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#ifdef _POSIX_C_SOURCE

/* if we actually use a POSIX-operating system, the environ variable
       should be marked as external */

extern char** environ;

#endif



int Dos9_CmdEcho(wchar_t* lpLine)
{

	ESTR* lpEsParameter;

	lpLine+=4;

	if (*lpLine!=L' '
	    && !iswpunct(*lpLine)
	    && *lpLine!=L'\0') {

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR, lpLine-4, FALSE);
		return -1;

	}

	lpEsParameter=Dos9_EsInit();

	if (iswpunct(*lpLine)) {

		Dos9_GetEndOfLine(lpLine+1, lpEsParameter);
		puts(Dos9_EsToChar(lpEsParameter));


	} else if (Dos9_GetNextParameterEs(lpLine, lpEsParameter)) {

		if (!wcscasecmp(Dos9_EsToChar(lpEsParameter), L"OFF")) {

			bEchoOn=FALSE;

		} else if (!wcscasecmp(Dos9_EsToChar(lpEsParameter), L"ON")) {

			bEchoOn=TRUE;

		} else if (!wcscmp(Dos9_EsToChar(lpEsParameter), L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_ECHO);

		} else {

			Dos9_GetEndOfLine(lpLine+1, lpEsParameter);
			puts(Dos9_EsToChar(lpEsParameter));

		}

	} else {

		/* si rien n'est entré on affiche l'état de la commannd echo */
		if (bEchoOn) putws(lpMsgEchoOn);
		else putws(lpMsgEchoOff);

	}


	Dos9_EsFree(lpEsParameter);

	return 0;
}

int Dos9_CmdExit(wchar_t* lpLine)
{
	wchar_t lpArg[]=L"-3000000000";
	wchar_t* lpNextToken;

	if ((lpNextToken=Dos9_GetNextParameter(lpLine+4, lpArg, 11))) {

		if (!wcscmp(lpArg, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_EXIT);
			return 0;

		} else if (!wcscasecmp(lpArg, L"/b")) {

			if ((lpNextToken=Dos9_GetNextParameter(lpNextToken, lpArg, 11))) {

				exit(wtoi(lpArg));

			} else {

				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, L"/b", FALSE);
				return 1;

			}

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpArg, FALSE);
			return 1;

		}
	}

	exit(0);

	return 0;
}

int Dos9_CmdPause(wchar_t* lpLine)
{
	if (!wcscmp(lpLine+6, L"/? ")) {

		Dos9_ShowInternalHelp(DOS9_HELP_PAUSE);
		return 0;
	}

	putws(lpMsgPause);

	getch();

	return 0;
}

double _Dos9_SetGetVarFloatW(const wchar_t* lpwName)
{
	wchar_t* lpContent;

	lpContent=wgetenv(lpwName);

	if (lpContent) {

		if (!wcscmp(lpContent, L"nan")) {

			return NAN;

		} else if (!wcscmp(lpContent, L"inf")) {

			return INFINITY;

		} else {

			return wtof(lpContent);

		}

	} else {

		return 0.0;

	}

}


double _Dos9_SetGetVarFloat(const char* lpName)
{
	wchar_t	*lpwName;
	double	*iRet;

	if (!(lpwName=libw_mbstowcs(lpName))) {

		Dos9_ShowErrorMessage(DOS9_FAILED_CONVERSION,
							  L(__FILE__) "/_Dos9_SetGetVarFloat",
							  FALSE
							  );

		return -1;

	}

	iRet=_Dos9_SetGetVarFloatW(lpwName);

	free(lpwName);

	return iRet;

}

int _Dos9_SetGetVarIntW(const wchar_t* lpwName)
{
	wchar_t* lpContent;

	lpContent=wgetenv(lpwName);

	if (lpContent) {

		return wtoi(lpContent);

	} else {

		return 0;

	}

}

int _Dos9_SetGetVarInt(const char* lpName)
{
	wchar_t* lpwName;
	int 	 iRet;

	if (!(lpwName=libw_mbtowc(lpName))) {

		Dos9_ShowErrorMessage(DOS9_FAILED_CONVERSION,
								L(__FILE__) "/_Dos9_SetGetVarInt",
								FALSE
								);

		return -1;

	}

	iRet=_Dos9_SetGetVarIntW(lpwName);

	free(lpwName);

	return iRet;
}

int Dos9_CmdSet(wchar_t *lpLine)
{
	wchar_t lpArgBuf[5],
			*lpArg=lpArgBuf;

	wchar_t *lpNextToken;

	int i,
	    bFloats;

	if ((lpNextToken=Dos9_GetNextParameter(lpLine+3, lpArgBuf, sizeof(lpArgBuf)/sizeof(wchar_t)))) {

		if (!wcscmp(lpArg, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_SET);
			goto error;

		} else if (!wcsncasecmp(lpArg, L"/a", 2)) {

			lpArg+=2;

			bFloats=bUseFloats;
			/* use mode set through setlocal */

			if (*lpArg==L':') lpArg++;

			switch (towupper(*lpArg)) {

				case L'F' :
					bFloats=TRUE;
					break;

				case L'I' :
					bFloats=FALSE;
			}

			/* get the floats */
			if (Dos9_CmdSetA(lpNextToken, bFloats))
				goto error;

		} else if (!wcscasecmp(lpArg, L"/p")) {

			if ((Dos9_CmdSetP(lpNextToken)))
				goto error;

		} else {

			/* simple set */

			if ((Dos9_CmdSetS(lpLine+3)))
				goto error;

		}

	} else {

		/* in default cases, print environment */
		for (i=0; environ[i]; i++) puts(environ[i]);

	}

	return 0;

error:
	return -1;
}

/* simple set */
int Dos9_CmdSetS(wchar_t* lpLine)
{
	ESTR* lpEsVar=Dos9_EsInit();

	lpLine=Dos9_SkipBlanks(lpLine);

	Dos9_GetEndOfLine(lpLine, lpEsVar);

	if (Dos9_PutEnv(Dos9_EsToChar(lpEsVar))) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
		                      Dos9_EsToChar(lpEsVar),
		                      FALSE);
		goto error;

	}

	Dos9_EsFree(lpEsVar);
	return 0;

error:
	Dos9_EsFree(lpEsVar);
	return -1;

}

int Dos9_CmdSetP(wchar_t* lpLine)
{

	ESTR* lpEsVar=Dos9_EsInit();
	ESTR* lpEsInput=Dos9_EsInit();
	wchar_t* lpEqual;

	lpLine=Dos9_SkipBlanks(lpLine);

	Dos9_GetEndOfLine(lpLine, lpEsVar);

	if ((lpEqual=wcschr(Dos9_EsToChar(lpEsVar), L'='))) {

		*lpEqual=L'\0';
		lpEqual++;

		putws(lpEqual);

		Dos9_EsGet(lpEsInput, stdin);

		Dos9_EsCat(lpEsVar, L"=");
		Dos9_EsCatE(lpEsVar, lpEsInput);

		if ((lpEqual=wcschr(Dos9_EsToChar(lpEsVar), L'\n')))
			*lpEqual=L'\0';

		if (Dos9_PutEnv(Dos9_EsToChar(lpEsVar))) {

			Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
			                      Dos9_EsToChar(lpEsVar),
			                      FALSE);

			goto error;

		}

	} else {

		Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
		                      Dos9_EsToChar(lpEsVar),
		                      FALSE);

		goto error;

	}

	Dos9_EsFree(lpEsVar);
	Dos9_EsFree(lpEsInput);
	return 0;

error:
	Dos9_EsFree(lpEsVar);
	Dos9_EsFree(lpEsInput);
	return -1;
}

int Dos9_CmdSetA(wchar_t* lpLine, int bFloats)
{

	ESTR* lpExpression=Dos9_EsInit();

	lpLine=Dos9_SkipBlanks(lpLine);

	/* get the expression back */
	Dos9_GetEndOfLine(lpLine, lpExpression);

	switch(bFloats) {

		case TRUE:
			/* evaluate floating expression */
			if ((Dos9_CmdSetEvalFloat(lpExpression)))
				goto error;

			break;

		case FALSE:
			/* evaluate integer expression */
			if ((Dos9_CmdSetEvalInt(lpExpression)))
				goto error;

			break;

	}


	Dos9_EsFree(lpExpression);
	return 0;

error:
	Dos9_EsFree(lpExpression);
	return -1;
}

int Dos9_CmdSetEvalFloat(ESTR* lpExpression)
{
	void* 	evaluator; /* an evaluator for libmatheval-Dos9 */
	wchar_t *lpVarName,
			*lpEqual,
			lpResult[30];

	wchar_t cLeftAssign=0;

	char*   lpExp;

	double 	dResult,
	       	dVal;

	lpVarName=Dos9_EsToChar(lpExpression);

	lpVarName=Dos9_SkipBlanks(lpVarName);

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"SET", FALSE);
		goto error;

	}

	/* seek an '=' sign */
	if (!(lpEqual=wcschr(lpVarName, L'='))) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpVarName, FALSE);
		goto error;

	}

	*lpEqual=L'\0';

	/* seek a sign like '+=', however, '' might be a valid environment
	   variable name depending on platform */
	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

	if (!(lpExp=libw_wcstombs(lpEqual+1))) {

		Dos9_ShowErrorMessage(DOS9_FAILED_CONVERSION,
							  lpEqual+1,
							  FALSE
							  );

		goto error;

	}

	/* create evaluator */
	if (!(evaluator=evaluator_create(lpExp))) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
		goto error;

	}

	free(lpExp)

	dResult=evaluator_evaluate2(evaluator, _Dos9_SetGetVarFloat);

	evaluator_destroy(evaluator);

	/* clear if operator is recognised */

	switch (cLeftAssign) {

		case L'*':
		case L'/':
		case L'+':
		case L'-':

			*(lpEqual-1)=L'\0';
			/* get the value of the variable */
			dVal=_Dos9_SetGetVarFloatW(lpVarName);

			switch(cLeftAssign) {

				case L'*':
					dVal*=dResult;
					break;

				case L'/':
					dVal/=dResult;
					break;

				case L'+':
					dVal+=dResult;
					break;

				case L'-':
					dVal-=dResult;

			}

			break;

		default:
			dVal=dResult;

	}

	snwprintf(lpResult, sizeof(lpResult), L"=%.16g", dVal);

	Dos9_EsCat(lpExpression, lpResult);

	if (Dos9_PutEnv(Dos9_EsToChar(lpExpression))) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
		                      Dos9_EsToChar(lpExpression),
		                      FALSE);

		goto error;

	}

	return 0;

error:
	return -1;
}

/* evaluate an interger expression */
int Dos9_CmdSetEvalInt(ESTR* lpExpression)
{
	wchar_t *lpVarName,
			*lpEqual,
	     	lpResult[30],
			cLeftAssign=0;

	int		iResult,
	      	iVal,
	      	bDouble=FALSE;

	char*	lpExp;

	Dos9_EsCat(lpExpression, L"\n");

	lpVarName=Dos9_EsToChar(lpExpression);

	lpVarName=Dos9_SkipBlanks(lpVarName);

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"SET", FALSE);
		goto error;

	}

	/* seek an '=' sign */
	if (!(lpEqual=wcschr(lpVarName, L'='))) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpVarName, FALSE);
		goto error;

	}

	*lpEqual=L'\0';

	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

	if (!(lpExp=libw_wcstombs(lpEqual+1))) {

		Dos9_ShowErrorMessage(DOS9_FAILED_CONVERSION,
								lpEqual+1,
								FALSE
								);

		goto error;

	}

	iResult=IntEval_Eval(lpExp);

	free(lpExp);

	if (IntEval_Error != INTEVAL_NOERROR) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
		goto error;

	}

	switch(cLeftAssign) {

		case L'*':
		case L'/':
		case L'+':
		case L'-':
		case L'^':

			*(lpEqual-1)=L'\0';
			iVal=_Dos9_SetGetVarIntW(lpVarName);

			switch(cLeftAssign) {

				case L'*':
					iVal*=iResult;
					break;

				case L'/':
					iVal/=iResult;
					break;

				case L'+':
					iVal+=iResult;
					break;

				case L'-':
					iVal/=iResult;
					break;

				case L'^':
					iVal^=iResult;
					break;

			}

			break;

		case L'&':
		case L'|':

			/* more complicated, it need to be
			   resolved */

			*(lpEqual-1)=L'\0';
			iVal=_Dos9_SetGetVarIntW(lpVarName);

			if (lpVarName != (lpEqual-1)) {

				if (*(lpEqual-2) == cLeftAssign) {

					bDouble=TRUE;
					*(lpEqual-2)=L'\0';

				}

			}

			switch (cLeftAssign) {

				case L'|':
					if (bDouble) {

						iVal=iVal || iResult;

					} else {

						iVal|=iResult;

					}

					break;

				case L'&':
					if (bDouble) {

						iVal=iVal && iResult;

					} else {

						iVal&=iResult;

					}

			}

			break;

		default:
			iVal=iResult;

	}

	snwprintf(lpResult, sizeof(lpResult), L"=%d", iVal);

	Dos9_EsCat(lpExpression, lpResult);

	if (Dos9_PutEnv(Dos9_EsToChar(lpExpression))) {

		Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
		                      Dos9_EsToChar(lpExpression),
		                      FALSE);

		goto error;

	}

	return 0;

error:
	return -1;
}

int Dos9_CmdSetLocal(wchar_t* lpLine)
{
	wchar_t lpName[FILENAME_MAX];
	wchar_t* lpNext=lpLine+8;

	while ((lpNext=Dos9_GetNextParameter(lpNext, lpName, FILENAME_MAX))) {

		if (!wcscasecmp(lpName, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_SETLOCAL);
			return 0;

		} else if (!wcscasecmp(lpName, L"ENABLEDELAYEDEXPANSION")) {

			bDelayedExpansion=TRUE;

		} else if (!wcscasecmp(lpName, L"ENABLEFLOATS")) {

			bUseFloats=TRUE;

		} else if (!wcscasecmp(lpName, L"CMDLYCORRECT")) {

			bCmdlyCorrect=TRUE;

		} else if (!wcscasecmp(lpName, L"CMDLYINCORRECT")) {

			bCmdlyCorrect=FALSE;

		} else if (!wcscasecmp(lpName, L"DISABLEFLOATS")) {

			bUseFloats=FALSE;

		} else if (!wcscasecmp(lpName, L"DISABLEDELAYEDEXPANSION")) {

			bDelayedExpansion=FALSE;

		} else if (!wcscasecmp(lpName, L"ENABLEEXTENSIONS")
					!wcscasecmp(lpName, L"DISABLEEXTENSION")) {

			/* provided for backward compatibility. The ENABLEEXTENSIONS
			   option was used to block some NT features to make scripts portables
			   to MS-DOS based prompt. This is not interesting anymore (at most
			   interest it too few people), so it is just ignored, since many NT
			   designed script use ENABLEEXTENSIONS to enable cmd.exe features
			 */

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpName, FALSE);
			return 1;

		}

	}

	return 0;
}

int Dos9_CmdHelp(wchar_t* lpLine)
{

	putws(lpHlpDeprecated);
	return 0;

}

int Dos9_CmdRem(wchar_t* lpLine)
{
	/* well a help message for this function is included,
	   however, is it really usefull, because, logically,

	        REM /?

	   should not be interpreted because /? is indeed a
	   comment */

	return 0;

}


int Dos9_CmdCls(wchar_t* lpLine)
{
	wchar_t lpArg[4];

	if (Dos9_GetNextParameter(lpLine+3, lpArg, 4)) {

		if (!wcscmp(lpArg, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_CLS);
			return 0;

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpArg, FALSE);
			return -1;

		}

	}

	Dos9_ClearConsoleScreen();

	return 0;
}

int Dos9_CmdColor(wchar_t* lpLine)
{
	wchar_t lpArg[4];

	if (Dos9_GetNextParameter(lpLine+5, lpArg, 4)) {

		if (!wcscmp(lpArg, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_COLOR);

		} else {

			colColor=strtol(lpArg, NULL, 16);
			Dos9_SetConsoleColor(colColor);

		}
		return 0;

	}

	colColor=DOS9_COLOR_DEFAULT;
	Dos9_SetConsoleColor(DOS9_COLOR_DEFAULT);

	return 0;
}

int Dos9_CmdTitle(wchar_t* lpLine)
{
	char lpArg[3];
	ESTR* lpEsTitle=Dos9_EsInit();

	lpLine+=5;

	if (Dos9_GetNextParameter(lpLine, lpArg, 3)) {

		if (!wcscmp(lpArg, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_TITLE);

		} else {

			Dos9_GetEndOfLine(lpLine, lpEsTitle);
			Dos9_SetConsoleTitle(Dos9_EsToChar(lpEsTitle));

		}

		Dos9_EsFree(lpEsTitle);

		return 0;
	}

	Dos9_EsFree(lpEsTitle);

	return -1;
}

int Dos9_CmdType(wchar_t* lpLine)
{
	wchar_t lpFileName[FILENAME_MAX];

	FILE* pFile;

	if (Dos9_GetNextParameter(lpLine+4, lpFileName, FILENAME_MAX)) {

		if (!wcscmp(lpFileName, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_TYPE);
			return 0;

		} else if ((pFile=wfopen(lpFileName, L"r"))) {

			while (fgetws(lpFileName, FILENAME_MAX, pFile))
				wprintf("%s", lpFileName);

			fclose(pFile);
			return 0;

		}

	}

	Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, FALSE);

	return -1;
}

int Dos9_CmdGoto(wchar_t* lpLine)
{
	wchar_t lpLabelName[FILENAME_MAX],
			lpFileName[FILENAME_MAX],
			lpFile=NULL;

	int 	bEchoError=TRUE;

	lpLine+=4;
	*lpLine=L':';

	if (*(lpLine+1)==':')
		lpLine++;

	if ((lpLine=Dos9_GetNextParameter(lpLine, lpLabelName, FILENAME_MAX))) {

		if (!wcscmp(lpLabelName, L":/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_GOTO);
			return 0;

		}

		if ((lpLine=Dos9_GetNextParameter(lpLine ,lpFileName, FILENAME_MAX))) {

			if (!wcscasecmp(lpFileName, L"/Q")) {

				/* on a choisi de rendre l'erreux muette */
				bEchoError=FALSE;

				if (!(Dos9_GetNextParameter(lpLine, lpFileName, FILENAME_MAX)))
					goto next;

			}

			if (bCmdlyCorrect) {

				/* if the user has set the CMDLYCORRECT flag, the capabilities of
				   calling another file's label is prohibited */

				Dos9_ShowErrorMessage(DOS9_EXTENSION_DISABLED_ERROR, NULL, FALSE);

				return -1;

			} else {

				lpFile=lpFileName;

			}

		}

	next:

		/* Now we have a valid label name, thus  we are about to find a label
		   in the specified file.
		   If we do have a valid file name, the search will be made in specified
		   file */

		DOS9_DBG(L"Jump to Label \"%s\" in \"%s\"", lpLabelName, lpFile);

		if (!wcscmp(lpLabelName,  L":EOF")) {

			/* do not even look for ``:EOF'', just abort the command. The value -1
			   is used to tell upper level function that the script should
			   be aborted */

			bAbortCommand=-1;

			return 0;

		} else if (Dos9_JumpToLabel(lpLabelName, lpFile)==-1) {

			if (!bEchoError)
				Dos9_ShowErrorMessage(DOS9_LABEL_ERROR, lpLabelName, FALSE);

			return -1;

		}

	}


	/* let's set a this global variable to let the other functions
	   know that they should reload an entire line. They tell the
	   interpreter to abort the current block, and nested block,
	   until we are back to to level functions */
	bAbortCommand=TRUE;

	return 0;
}

int Dos9_CmdCd(wchar_t* lpLine)
{
	wchar_t* lpNext;
	ESTR* lpEsDir=Dos9_EsInit();

	if (!(lpLine=strchr(lpLine, ' '))) {
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, FALSE);
		goto error;
	}

	if ((lpNext=Dos9_GetNextParameterEs(lpLine, lpEsDir))) {

		if (!wcscmp(Dos9_EsToChar(lpEsDir), L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_CD);
			goto error;

		} else if (!wcscasecmp(Dos9_EsToChar(lpEsDir), L"/d")) {

			lpLine=lpNext;

		}

		while (*lpLine==L' ' || *lpLine==L'\t') lpLine++;

		Dos9_GetEndOfLine(lpLine, lpEsDir);

		lpLine=Dos9_EsToChar(lpEsDir);

		lpNext=NULL;

		while (*lpLine) {

			switch(*lpLine) {
				case L'\t':
				case L' ':

					if (!lpNext) lpNext=lpLine;
					break;

				default:
					lpNext=NULL;
			}

			lpLine++;

		}

		if (lpNext) *lpNext=L'\0';

		errno=0;

		lpLine=Dos9_EsToChar(lpEsDir);

		DOS9_DBG(L"Changing directory to : \"%s\"\n", lpLine);

		wchdir(lpLine);

		if (errno ==  0) {

			/* update the current directory buffer */

			Dos9_UpdateCurrentDir();

		} else {

			/* do not perform errno checking
			   as long as the most important reason for
			   chdir to fail is obviously the non existence
			   or the specified directory

			   However, it appears that this is inconsistant
			   using windows as it does not returns on failure
			   every time a non-existing folder is passed to the
			   function, tried with '.. ' on my system

			*/

			Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, lpLine, FALSE);
			goto error;

		}

	} else {

		putws(Dos9_GetCurrentDir());

	}

	Dos9_EsFree(lpEsDir);

	DOS9_DBG(L"Returning from \"cd\" on success\n");
	return 0;

error:

	Dos9_EsFree(lpEsDir);
	return -1;
}

int Dos9_CmdBlock(wchar_t* lpLine)
{
	BLOCKINFO bkCode;
	wchar_t* lpToken;
	ESTR* lpNextBlock=Dos9_EsInit();

	Dos9_GetNextBlockEs(lpLine, lpNextBlock);

	lpToken=Dos9_GetNextBlock(lpLine, &bkCode);

	if (!lpToken) {

		Dos9_ShowErrorMessage(DOS9_INVALID_TOP_BLOCK, lpLine, FALSE);
		return -1;

	}

	if (*lpToken!=L')') {

		Dos9_ShowErrorMessage(DOS9_INVALID_TOP_BLOCK, lpLine, FALSE);
		return -1;

	}

	lpToken++;

	lpToken=Dos9_SkipBlanks(lpToken);


	if (*lpToken) {

		Dos9_ShowErrorMessage(DOS9_INVALID_TOP_BLOCK, lpLine, FALSE);
		return -1;

	}

	Dos9_RunBlock(&bkCode);

	return iErrorLevel;
}

int Dos9_CmdShift(wchar_t* lpLine)
{
	ESTR* 	lpEsArg=Dos9_EsInit();
	wchar_t *lpToken;
	int iBegin=0,        /* the first parameter to be displaced */
	    iDisplacement=1; /* the displacement of parameters on the left */

	lpLine+=5;

	while (lpLine=Dos9_GetNextParameterEs(lpLine, lpEsArg)) {

		lpToken=Dos9_EsToChar(lpEsArg);

		if ((*lpToken)==L'/') {

			lpToken++;

			switch (*lpToken) {
				case L'0':
				case L'1':
				case L'2':
				case L'3':
				case L'4':
				case L'5':
				case L'6':
				case L'8':
				case L'9':
					iBegin=*lpToken-L'0'; /* well, we assume that all numbers
                                            are folowing 0 */

					break;

					/* following switchs are Dos9-specific */
				case L's':
					lpToken++;

					if (*lpToken==':')
						lpToken++;

					if (!(*lpToken>='0' && *lpToken<='9')) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;

					}

					iBegin=*lpToken-L'0';
					lpToken++;

					if (*lpToken) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;


					}

					break;

				case L'd':
					lpToken++;

					if (*lpToken==L':')
						lpToken++;

					if (!(*lpToken>=L'0' && *lpToken<=L'9')) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;

					}

					iDisplacement=*lpToken-L'0';
					lpToken++;

					if (*lpToken) {

						Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
						                      lpToken,
						                      FALSE);

						goto error;


					}

					break;

				case L'?':
					Dos9_ShowInternalHelp(DOS9_HELP_SHIFT);
					goto error;

				default:
					Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
					                      lpToken,
					                      FALSE
					                     );

					goto error;


			}

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
			                      lpToken,
			                      FALSE);

			goto error;

		}

	}

	/* Displace command-line arguments */
	while ((iBegin+iDisplacement) < 10) {

		Dos9_SetLocalVar(lpvLocalVars,
		                 L'0'+iBegin,
		                 Dos9_GetLocalVarPointer(lpvLocalVars,
		                         L'0'+iBegin+iDisplacement
		                                        )
		                );

		iBegin++;

	}

	/* empty the remaining arguments */
	while (iBegin < 10) {

		Dos9_SetLocalVar(lpvLocalVars, L'0'+iBegin, L"");

		iBegin++;

	}

	Dos9_EsFree(lpEsArg);
	return 0;

error:

	Dos9_EsFree(lpEsArg);
	return -1;

}
