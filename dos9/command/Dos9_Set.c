/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

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

#include "Dos9_Set.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

double _Dos9_SetGetVarFloat(const char* lpName)
{
	char* lpContent;

	lpContent=Dos9_GetEnv(lpeEnv, lpName);

	if (lpContent) {

		if (!strcmp(lpContent, "nan")) {

			return NAN;

		} else if (!strcmp(lpContent, "inf")) {

			return INFINITY;

		} else {

			return atof(lpContent);
		}

	} else {

		return 0.0;

	}

}

double _Dos9_SetSetVarFloat(char* name, double f)
{
	char value[FILENAME_MAX];

	sprintf(value, "%.16g", f);

	Dos9_SetEnv(lpeEnv, name, value);

    return f;
}

int _Dos9_SetGetVarInt(char* lpName)
{
	char* lpContent;

	lpContent=Dos9_GetEnv(lpeEnv, lpName);

	if (lpContent) {

		return atoi(lpContent);

	} else {

		return 0;

	}

}

int _Dos9_SetSetVarInt(char* lpName, int v)
{
	char lpContent[FILENAME_MAX];
    char *tok;

    /* BIG HACK */
    if ((tok = strchr(lpName, '=')))
        *tok = '\0';

    snprintf(lpContent, sizeof(lpContent), "%d", v);
	Dos9_SetEnv(lpeEnv, lpName, lpContent);

    return v;
}


/*

	SET [/A[:][f|i]] [/P] var=exp

	Set an environment variable.

	- /A : select the mathematic mode ('f' for floating-point and 'i' for
	integers)

	- /P : prompts the user with exp1


	Cmd.exe' set usually behaves strangely toward quotes:

		* If the expression is quoted, just like `set var="exp1" exp2',
	what is assigned is `var="exp1" exp2'.

		* If the name and expression are quoted, just like
	`set "var=exp1" exp2', cmd.exe just look for the last quote on the line
	and strip the following characters. Thus what we get will be `var=exp1'.
	This behaviour introduces concerns about how the line is actually parsed
	because remaining characters are ignored.

	However, for set /a, the behaviour is a little bit different; Various
	variables may be defined through the use of `,' and quotes. This syntax
	is really fragile because equivalent signs may not work when replaced.
	Basically, `set "var1=1","var2=2"' may assign various variables, while
	using a space may not.

	Things are even worse in case of inline assignments using set /a.
	Problems are introduced by the lack of standard precedence rules for
	assignments. As an example, `set /a var=1+(var3=2)' evaluates to 3
	whereas `set /a var=1+var3=2' evaluates to 2 (That does not make sense
	anyway, can't figure how to get 2 out of this expression).

	IMPLEMENTATION:
	===============

	Dos9 actually supports the following behaviours on simple set (without
	any switches) :

    The set simple mode and behaves *exactly* the same way as cmd does.

	Dos9 behaves exactly like cmd.exe when using the '/p' switch.

	Dos9 behaves mostly like cmd.exe when using the '/a' switch, except
	that inline assignment are supported but is not guaranteed to be 100%
	compatible as inline assignment are a bit complicated to understand
	under cmd. (As stated above, it may hard to parse)

*/
int Dos9_CmdSet(char *lpLine)
{
	char lpArgBuf[5],
	     *lpArg=lpArgBuf;

	char *lpNextToken;

	int i,
        nok = 0,
	    bFloats,
	    status = DOS9_NO_ERROR;

	if ((lpNextToken=Dos9_GetNextParameter(lpLine+3, lpArgBuf,
            sizeof(lpArgBuf))))  {

		if (!stricmp(lpArg, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_SET);
			goto error;

		} else if (!strnicmp(lpArg, "/a", 2)) {

			lpArg+=2;

			bFloats=bUseFloats;
			/* use mode set through setlocal */

			if (*lpArg==':') lpArg++;

			switch (toupper(*lpArg)) {

				case 'F' :
					bFloats=TRUE;
					break;

				case 'I' :
					bFloats=FALSE;
			}

			/* get the floats */
			if (status = Dos9_CmdSetA(lpNextToken, bFloats))
				goto error;

		} else if (!stricmp(lpArg, "/p")) {

			if (status = Dos9_CmdSetP(lpNextToken))
				goto error;

		} else {

			/* simple set */

			nok = Dos9_CmdSetS(lpLine+3);

		}

	} else {

        nok = 1;

	}

	if (nok == 1) {

		lpLine = Dos9_SkipBlanks(lpLine+3);

		/* if a variable have been found,
		   this variable vary usually
		   when using a prefix
		*/
		char found = 0;

		/* in default cases, print environment
		   which match with the prefix (if defined)
		*/
		for (i=0; i < lpeEnv->index; i++) {
			if (*lpLine == '\0'
                || ((lpeEnv->envbuf[i]->name != NULL)
                    && !strnicmp(lpeEnv->envbuf[i]->name, lpLine, strlen(lpLine)))) {

				found = TRUE;

				/* variable valid */
	            fprintf(fOutput, "%s=%s" DOS9_NL, lpeEnv->envbuf[i]->name,
	                                    lpeEnv->envbuf[i]->content);
			}
		}

		if (!found) {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpLine, FALSE);
			status = DOS9_UNEXPECTED_ELEMENT;

			goto error;

		}
	}

error:
	return status;
}

/* simple set */
int Dos9_CmdSetS(char* lpLine)
{
	ESTR* lpEsVar=Dos9_EsInit();

	char *lpCh,
		*lpBegin,
		*lpEnd;

	while (lpLine) {

		lpLine=Dos9_SkipBlanks(lpLine);

		if (*lpLine=='\0'){

			break;

		} else {

			/* use the good old method of cmd.exe' set
			   truncate the line at the last quote
			*/

			Dos9_GetEndOfLine(lpLine, lpEsVar);

            /* Strip any pair of '"' if encountered */
			if (*(lpEsVar->str) == '"'
                && (lpEnd = strrchr(lpEsVar->str + 1, '"'))) {

                *lpEnd = '\0';
                lpBegin = lpEsVar->str + 1;

            } else {

                lpBegin = lpEsVar->str;

            }

			lpLine=NULL;


		}

		if (!(lpCh=strchr(lpBegin, '='))) {

			/* Well, apparently this line is not well constructed,
			   however, we do not display error, and backtrack to
			   the original function  */

			Dos9_EsFree(lpEsVar);
			return 1;

		}

		*lpCh='\0';
		lpCh++;

		Dos9_SetEnv(lpeEnv, lpBegin, lpCh);

	}

	Dos9_EsFree(lpEsVar);
	return 0;
}

int Dos9_CmdSetP(char* lpLine)
{

	ESTR* lpEsVar=Dos9_EsInit();
	ESTR* lpEsInput=Dos9_EsInit();
	char* lpEqual;
	int status = DOS9_NO_ERROR;

	lpLine = Dos9_SkipBlanks(lpLine);

    /* If the expression starts with an ", use get parameter
       to process the token in a clean way */
	if (*lpLine == '"') {

        Dos9_GetNextParameterEs(lpLine, lpEsVar);

    } else {

        Dos9_GetEndOfLine(lpLine, lpEsVar);

    }


	if ((lpEqual=strchr(Dos9_EsToChar(lpEsVar), '='))) {

		*lpEqual='\0';
		lpEqual++;

		fprintf(fOutput, "%s", lpEqual);

		INPUT_FILE file;
        *(file.lpFileName) = '\0';

		Dos9_GetLine(lpEsInput, &file);

		if ((lpEqual=strchr(Dos9_EsToChar(lpEsInput), '\n')))
			*lpEqual='\0';

		Dos9_SetEnv(lpeEnv, Dos9_EsToChar(lpEsVar),
				Dos9_EsToChar(lpEsInput));

	} else {

		Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
		                      Dos9_EsToChar(lpEsVar),
		                      FALSE);

        status = DOS9_UNEXPECTED_ELEMENT;

		goto error;

	}

error:
	Dos9_EsFree(lpEsVar);
	Dos9_EsFree(lpEsInput);
	return status;
}

int Dos9_CmdSetA(char* lpLine, int bFloats)
{

	ESTR* lpExpression=Dos9_EsInit();
	int status = DOS9_NO_ERROR;

	lpLine=Dos9_SkipBlanks(lpLine);

	while ((lpLine=Dos9_GetNextParameterEsD(lpLine, lpExpression , "\","))) {

		/* get the expression back */
		//Dos9_GetEndOfLine(lpLine, lpExpression);

		switch(bFloats) {

        case TRUE:
            /* evaluate floating expression */
            if (status = Dos9_CmdSetEvalFloat(lpExpression))
                goto error;

            break;

		case FALSE:
			/* evaluate integer expression */
			if (status = Dos9_CmdSetEvalInt(lpExpression))
				goto error;

			break;

		}
    }

error:
	Dos9_EsFree(lpExpression);
	return status;
}

int Dos9_CmdSetEvalFloat(ESTR* lpExpression)
{
	void* evaluator; /* an evaluator for libmatheval-Dos9 */
	char *lpVarName,
	     *lpEqual,
	     lpResult[30];
	char  cLeftAssign=0;
	double dResult,
	       dVal;
    int status = DOS9_NO_ERROR;

	lpVarName=Dos9_EsToChar(lpExpression);

	lpVarName=Dos9_SkipBlanks(lpVarName);

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "SET", FALSE);
		return DOS9_EXPECTED_MORE;

	}

	/* seek an '=' sign */
	if (!(lpEqual=strchr(lpVarName, '='))) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpVarName, FALSE);
		return DOS9_INVALID_EXPRESSION;

	}

	*lpEqual='\0';

	/* seek a sign like '+=', however, '' might be a valid environment
	   variable name depending on platform */
	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

    if (Dos9_LockMutex(&mSetFLock))
        Dos9_ShowErrorMessage(DOS9_LOCK_MUTEX_ERROR,
                              __FILE__ "/Dos9_CmdSetEvalFloat()" , -1);

	/* create evaluator */
	if (!(evaluator=evaluator_create(lpEqual+1))) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
		status = DOS9_INVALID_EXPRESSION;

		goto error;

	}

    evaluator_set_functions(_Dos9_SetGetVarFloat, _Dos9_SetSetVarFloat);
	dResult=evaluator_evaluate(evaluator);

	evaluator_destroy(evaluator);

	/* clear if operator is recognised */

	switch (cLeftAssign) {

		case '*':
		case '/':
		case '+':
		case '-':
			*(lpEqual-1)='\0';
			/* get the value of the variable */
			Dos9_AdjustVarName(lpVarName);
			dVal=_Dos9_SetGetVarFloat(lpVarName);

			switch(cLeftAssign) {

				case '*':
					dVal*=dResult;
					break;

				case '/':
					dVal/=dResult;
					break;

				case '+':
					dVal+=dResult;
					break;

				case '-':
					dVal-=dResult;

			}

			break;

		default:
			dVal=dResult;

	}

	snprintf(lpResult, sizeof(lpResult), "%.16g", dVal);

    if (!bIsScript)
        fprintf(fOutput, "%.16g", dVal);

	Dos9_SetEnv(lpeEnv, Dos9_EsToChar(lpExpression), lpResult);

error:
    if (Dos9_ReleaseMutex(&mSetFLock))
        Dos9_ShowErrorMessage(DOS9_RELEASE_MUTEX_ERROR,
                              __FILE__ "/Dos9_CmdSetEvalFloat()" , -1);

	return status;
}

/* evaluate an interger expression */
int Dos9_CmdSetEvalInt(ESTR* lpExpression)
{
	char *lpVarName,
	     *lpEqual,
	     lpResult[30];
	char  cLeftAssign=0;
	int   iResult,
	      iVal,
	      bDouble=FALSE,
	      status = 0;

	Dos9_EsCat(lpExpression, "\n");

	lpVarName=Dos9_EsToChar(lpExpression);

	while (*lpVarName==' ' || *lpVarName=='\t') lpVarName++;

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "SET", FALSE);
        return DOS9_EXPECTED_MORE;

	}

	/* seek an '=' sign */
	if (!(lpEqual=strchr(lpVarName, '='))) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpVarName, FALSE);
		return DOS9_EXPECTED_MORE;

	}

	*lpEqual='\0';

	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

	if (Dos9_LockMutex(&mSetILock))
        Dos9_ShowErrorMessage(DOS9_LOCK_MUTEX_ERROR,
                              __FILE__ "/Dos9_CmdSetEvalInt()" , -1);

    IntEval_Set_Fn(_Dos9_SetGetVarInt, _Dos9_SetSetVarInt);
	iResult=IntEval_Eval(lpEqual+1);

	if (IntEval_Error != INTEVAL_NOERROR) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
		status = DOS9_INVALID_EXPRESSION;

		goto error;

	}

	switch(cLeftAssign) {

		case '*':
		case '/':
		case '+':
		case '-':
		case '^':

			*(lpEqual-1)='\0';

			Dos9_AdjustVarName(lpVarName);
			iVal=_Dos9_SetGetVarInt(lpVarName);

			switch(cLeftAssign) {

				case '*':
					iVal*=iResult;
					break;

				case '/':
					iVal/=iResult;
					break;

				case '+':
					iVal+=iResult;
					break;

				case '-':
					iVal-=iResult;
					break;

				case '^':
					iVal^=iResult;
					break;

			}

			break;

		case '&':
		case '|':

			/* more complicated, it need to be
			   resolved */

			*(lpEqual-1)='\0';

			Dos9_AdjustVarName(lpVarName);
			iVal=_Dos9_SetGetVarInt(lpVarName);

			if (lpVarName != (lpEqual-1)) {

				if (*(lpEqual-2) == cLeftAssign) {

					bDouble=TRUE;
					*(lpEqual-2)='\0';

				}

			}

			switch (cLeftAssign) {

				case '|':
					if (bDouble) {

						iVal=iVal || iResult;

					} else {

						iVal|=iResult;

					}

					break;

				case '&':
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

	snprintf(lpResult, sizeof(lpResult), "%d", iVal);

    if (!bIsScript)
        fprintf(fOutput, "%d", iVal);

	Dos9_SetEnv(lpeEnv, Dos9_EsToChar(lpExpression), lpResult);

error:
    if (Dos9_ReleaseMutex(&mSetILock))
            Dos9_ShowErrorMessage(DOS9_RELEASE_MUTEX_ERROR,
                                  __FILE__ "/Dos9_CmdSetEvalInt()" , -1);

	return status;

}
