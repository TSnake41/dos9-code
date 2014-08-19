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

#include "Dos9_Set.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

#define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

double _Dos9_SetGetVarFloat(DOS9CONTEXT* pContext, const char* lpName)
{
	char* lpContent;

	lpContent=Dos9_GetEnv(pContext->pEnv, lpName);

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

int _Dos9_SetGetVarInt(DOS9CONTEXT* pContext, const char* lpName)
{
	char* lpContent;

	lpContet=Dos9_GetEnv(pContext->pEnv, lpName);

	if (lpContent) {

		return strtol(lpContent, NULL, 0);

	} else {

		return 0;

	}

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

	Things are even worser in case of inline assigments using set /a.
	Problems are introduced by the lack of standard precendence rules for
	assignments. As an example, `set /a var=1+(var3=2)' evaluates to 3
	whereas `set /a var=1+var3=2' evaluates to 2 (That does not make sense
	anyway, can't figure how to get 2 out of this expression). Thus, this
	features are not implemented yet.

	IMPLEMENTATION:
	===============

	Dos9 actually supports the following behaviours on simple set (without
	any switches) :

		* If CMDLYCORRECT is set and the command line is quoted like
	`set "var=exp"', then it behaves *exactly* the same way as cmd does.

		* If CMDLYCORRECT is not set and the command line is quoted
	like `set "var1=exp1" "var2=exp2"', then dos9 assigns both 'var1' and
	'var2'.

	Dos9 behaves exactly like cmd.exe when using the '/p' switch.

	Dos9 behaves mostly like cmd.exe when using the '/a' switch, except
	that inline assignment are not supported. (As stated above, it may
	hard to parse, and I still don't understand aspects)

*/
int Dos9_CmdSet(DOS9CONTEXT* pContext, char *lpLine)
{
	char lpArgBuf[5],
	     *lpArg=lpArgBuf;

	char *lpNextToken;

	int i,
	    bFloats;

	if ((lpNextToken=Dos9_GetNextParameter(pContext, lpLine+3, lpArgBuf,
                                        sizeof(lpArgBuf)))) {

		if (!stricmp(lpArg, "/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_SET);
			goto error;

		} else if (!strnicmp(lpArg,"/a", 2)) {

			lpArg+=2;

            /* use mode set through setlocal */
			bFloats=pContext->iMode & DOS9_CONTEXT_USE_FLOATS;

			if (*lpArg==':') lpArg++;

			switch (toupper(*lpArg)) {

				case 'F' :
					bFloats=TRUE;
					break;

				case 'I' :
					bFloats=FALSE;

			}

			if (Dos9_CmdSetA(pContext, lpNextToken, bFloats))
				goto error;

		} else if (!stricmp(lpArg, "/p")) {

			if ((Dos9_CmdSetP(pContext, lpNextToken)))
				goto error;

		} else {

			/* simple set */

			if ((Dos9_CmdSetS(pContext, lpLine+3)))
				goto error;

		}

	} else {

		/* in default cases, print environment */
		for (i=0; i < pContext->pEnv->index ; i++) {

            fprintf(pContext->pStack->out,
                    "%s=%s",
                    pContext->pEnv->envbuf[i]->name,
                    pContext->pEnv->envbuf[i]->content
                    );

        }
	}

	return 0;

error:
	return -1;
}

/* simple set */
int Dos9_CmdSetS(DOS9CONTEXT* pContext, char* lpLine)
{
	ESTR* lpEsVar=Dos9_EsInit();

	char *lpCh,
		*lpBegin,
		*lpEnd;

	while (lpLine) {


		lpLine=Dos9_SkipBlanks(lpLine);

		if (*lpLine=='\0'){

			break;

		} else if (*lpLine=='"'
                   && pContext->iMode & DOS9_CONTEXT_CMDLYCORRECT) {

			/* use the good old method of cmd.exe' set
			   truncate the line at the last quote
			*/

			lpLine++;

			Dos9_GetEndOfLine(pContext, lpLine, lpEsVar);

			lpLine=NULL;

			if ((lpCh=strrchr(Dos9_EsToChar(lpEsVar), '"')))
				*lpCh='\0';

		} else if (*lpLine=='"') {

			/* use the new behaviour (get the next parameter
			   and loop again */
			lpLine=Dos9_GetNextParameterEsD(pContext, lpLine, lpEsVar,
					"\t\" ");

			DOS9_DBG("GOT Token => \"%s\"\n", Dos9_EsToChar(lpEsVar)
					);

		} else {

			Dos9_GetEndOfLine(pContext, lpLine, lpEsVar);
			lpLine=NULL;

		}

		if (!(lpCh=strchr(Dos9_EsToChar(lpEsVar), '='))) {

			/* The whole line is not a valid token, return
			   on error */

			Dos9_ShowErrorMessageX(pContext,
                                    DOS9_UNEXPECTED_ELEMENT,
                                    Dos9_EsToChar(lpEsVar)
                                    );

			goto error;

		}

		*lpCh='\0';
		lpCh++;

		if (Dos9_SetEnv(pContext->pEnv, Dos9_EsToChar(lpEsVar), lpCh)) {

			Dos9_ShowErrorMessageX(pContext,
                          DOS9_UNABLE_SET_ENVIRONMENT,
					      Dos9_EsToChar(lpEsVar)
					      );
			goto error;

		}

	}

	Dos9_EsFree(lpEsVar);
	return 0;

error:
	Dos9_EsFree(lpEsVar);
	return -1;

}

int Dos9_CmdSetP(DOS9CONTEXT* pContext,char* lpLine)
{

	ESTR* lpEsVar=Dos9_EsInit();
	ESTR* lpEsInput=Dos9_EsInit();
	char* lpEqual;

	lpLine = Dos9_SkipBlanks(lpLine);

	Dos9_GetEndOfLine(pContext, lpLine, lpEsVar);

	if ((lpEqual=strchr(Dos9_EsToChar(lpEsVar), '='))) {

		*lpEqual='\0';
		lpEqual++;

		fputs(lpEqual, pContext->pStack->out);

		Dos9_EsGet(lpEsInput, pContext->pStack->in);

		if ((lpEqual=strchr(Dos9_EsToChar(lpEsInput), '\n')))
			*lpEqual='\0';

		if (Dos9_SetEnv(pContext->pEnv, Dos9_EsToChar(lpEsVar),
				Dos9_EsToChar(lpEsInput))) {

			Dos9_ShowErrorMessageX(pContext
                                    DOS9_UNABLE_SET_ENVIRONMENT,
                                    Dos9_EsToChar(lpEsVar)
                                    );

			goto error;

		}

	} else {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_UNEXPECTED_ELEMENT,
                                Dos9_EsToChar(lpEsVar)
                                );

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

int Dos9_CmdSetA(DOS9CONTEXT* pContext, char* lpLine, int bFloats)
{

	ESTR* lpExpression=Dos9_EsInit();

	lpLine=Dos9_SkipBlanks(lpLine);

	while (lpLine=Dos9_GetNextParameterEsD(pContext, lpLine,
                                                lpExpression , "\",")) {

		switch(bFloats) {

            case 0:
                /* evaluate integer expression */
                if ((Dos9_CmdSetEvalInt(pContext, lpExpression)))
                    goto error;

                break;

            default:
				/* evaluate floating expression */
				if ((Dos9_CmdSetEvalFloat(pContext, lpExpression)))
					goto error;

				break;

		}
	}


	Dos9_EsFree(lpExpression);
	return 0;

error:
	Dos9_EsFree(lpExpression);
	return -1;
}

int Dos9_CmdSetEvalFloat(DOS9CONTEXT* pContext, ESTR* lpExpression)
{
	void* evaluator; /* an evaluator for libmatheval-Dos9 */
	char *lpVarName,
	     *lpEqual,
	     lpResult[30];
	char  cLeftAssign=0;
	double dResult,
	       dVal;

	lpVarName=Dos9_SkipBlanks(Dos9_EsToChar(lpExpression));

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_EXPECTED_MORE,
                                "SET"
                                );
		goto error;

	}

	/* seek an '=' sign */
	if (!(lpEqual=strchr(lpVarName, '='))) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_INVALID_EXPRESSION,
                                lpVarName
                                );
		goto error;

	}

	*lpEqual='\0';

	/* seek a sign like '+=', however, '' might be a valid environment
	   variable name depending on platform */
	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

	/* create evaluator */
	if (!(evaluator=evaluator_create(lpEqual+1))) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_INVALID_EXPRESSION,
                                lpEqual+1);
		goto error;

	}

	dResult=evaluator_evaluate2(evaluator, _Dos9_SetGetVarFloat, pContext);

	evaluator_destroy(evaluator);

	/* clear if operator is recognised */

	switch (cLeftAssign) {

		case '*':
		case '/':
		case '+':
		case '-':
			*(lpEqual-1)='\0';
			/* get the value of the variable */
			dVal=_Dos9_SetGetVarFloat(pContext, lpVarName);

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

	if (Dos9_SetEnv(pContext->pEnv, Dos9_EsToChar(lpExpression), lpResult)) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_UNABLE_SET_ENVIRONMENT,
                                Dos9_EsToChar(lpExpression)
                                );

		goto error;

	}

	return 0;

error:
	return -1;
}

/* evaluate an interger expression */
int Dos9_CmdSetEvalInt(DOS9CONTEXT* pContext, ESTR* lpExpression)
{
	char *lpVarName,
	     *lpEqual,
	     lpResult[30];
	char  cLeftAssign=0;
	int   iResult,
	      iVal,
	      bDouble=FALSE;

	Dos9_EsCat(lpExpression, "\n");

	lpVarName=Dos9_SkipBlanks(Dos9_EsToChar(lpExpression));

	/* if we don't have expression, end-up with an error */
	if (!*lpVarName) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_EXPECTED_MORE,
                                "SET"
                                );
		goto error;

	}

	/* seek an '=' sign */
	if (!(lpEqual=strchr(lpVarName, '='))) {

		Dos9_ShowErrorMessage(pContext,
                                DOS9_INVALID_EXPRESSION,
                                lpVarName
                                );
		goto error;

	}

	*lpEqual='\0';

	if (lpEqual != lpVarName) {

		cLeftAssign=*(lpEqual-1);

	}

	iResult=IntEval_Eval(lpEqual+1);

	if (IntEval_Error != INTEVAL_NOERROR) {

		Dos9_ShowErrorMessage(DOS9_INVALID_EXPRESSION, lpEqual+1, FALSE);
		goto error;

	}

	switch(cLeftAssign) {

		case '*':
		case '/':
		case '+':
		case '-':
		case '^':

			*(lpEqual-1)='\0';
			iVal=_Dos9_SetGetVarInt(pContext, lpVarName);

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
					iVal/=iResult;
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
			iVal=_Dos9_SetGetVarInt(pContext, lpVarName);

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

	if (Dos9_SetEnv(pContext, Dos9_EsToChar(lpExpression), lpResult)) {

		Dos9_ShowErrorMessageX(pContext,
                                DOS9_UNABLE_SET_ENVIRONMENT,
                                Dos9_EsToChar(lpExpression)
                                );

		goto error;

	}

	return 0;

error:
	return -1;
}

