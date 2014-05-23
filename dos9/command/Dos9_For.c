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
#include <errno.h>
#include <string.h>
#include <wchar.h>

#include <libDos9.h>
#include <libw.h>

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"
#include "Dos9_For.h"

/*
    The for command

    * For each item in a list.
      Default token are ", " and carriot return.

        FOR %%A IN (ens) DO (

        )

    * For each interger value in a range of values

        FOR /L %%A IN (begin, increment, end) DO (

        )

    * For each file of a directory :

        FOR /R "basedir" %%A IN (selector) DO (

        )

      DEPRECATED : such kind of old fashionned loop
      with unflexible option are processed like the
      following line :

        FOR /F "tokens=*" %%A IN ('dir /B basedir/selector') DO (

        )

    * Process text from a file :

        FOR /F "options" %%A IN (file) DO command
        FOR /F "options" %%A IN ("string") DO command
        FOR /F "options" %%A IN (`string`) DO command

 */

int Dos9_CmdFor(wchar_t* lpLine)
{
	ESTR* lpParam=Dos9_EsInit();
	ESTR* lpDirectory=Dos9_EsInit();
	ESTR* lpInputBlock=Dos9_EsInit();

	BLOCKINFO bkCode;

	FORINFO forInfo= {
		L" ", /* no tokens delimiters, since only one
                token is taken account */
		L";", /* the default delimiter is ; */
		0, /* no skipped line */
		0, /* this is to be fullfiled later (the
                name letter of loop special var) */
		FALSE,
		1, /* the number of tokens we were given */
		{TOHIGH(1)|1} /* get only the first back */
	};

	wchar_t cVarName;
	wchar_t *lpToken=lpLine+3,
			*lpVarName;

	int iForType=FOR_LOOP_SIMPLE;

	while ((lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

		if (!wcscasecmp(Dos9_EsToChar(lpParam), L"/F")) {

			iForType=FOR_LOOP_F;

		} else if (!wcscasecmp(Dos9_EsToChar(lpParam), L"/L")) {

			iForType=FOR_LOOP_L;

		} else if (!wcscasecmp(Dos9_EsToChar(lpParam), L"/R")) {

			iForType=FOR_LOOP_R;

		} else if (!wcscasecmp(Dos9_EsToChar(lpParam), L"/D")) {

			iForType=FOR_LOOP_D;

		} else if (!wcscmp(Dos9_EsToChar(lpParam), L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_FOR);
			goto error;

		} else {

			break;

		}

	}

	while (TRUE) {

		if (lpToken == NULL) {

			/* if the line is not complete */
			Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"FOR", FALSE);

			wprintf("Exiting without reason !");

			goto error;

		}

		if ( *Dos9_EsToChar(lpParam) == L'%' ) {

			/*
			    The user did not specified options, that's not
			    problematic.
			*/

			/* get back the var name */
			lpVarName=Dos9_EsToChar(lpParam)+1;

			if (*lpVarName==L'%')
				lpVarName++;

			cVarName=*lpVarName;

			/* FIXME : Add a conformance test on cVarName */

			/* just get out of that loop */
			break;

		} else if (iForType == FOR_LOOP_SIMPLE
					|| iForType == FOR_LOOP_L
					|| *Dos9_EsToChar(lpDirectory) != L'\0') {

			/* It appears that the users does not specify a variable
			   but try rather to pass options, that are obviously inconsistent
			   in this case */
			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
									Dos9_EsToChar(lpParam),
									FALSE);

			goto error;

		} else {

			switch(iForType) {

				case FOR_LOOP_D:
				case FOR_LOOP_R:

					/* In that case, this is must be the
					   directory parameter */
					Dos9_EsCpyE(lpDirectory, lpParam);
					break;

				case FOR_LOOP_F:
					/* in that case, this must be one of
					   the tokens that contains for-loop parameter.
					   This parameter can be dispatched in many different
					   parameter, as long are they're in a row */
					if ((Dos9_ForMakeInfo(Dos9_EsToChar(lpParam), &forInfo))) {

						return -1;

					}

			}

			lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

		}

	}

	/* if the for uses either /F or /R or even /D
	   store cVarName in the FORINFO structure
	*/

	if (FOR_LOOP_F == iForType || FOR_LOOP_R == iForType) {

		forInfo.cFirstVar=cVarName;

	}


	/* Now the next parameter should be IN */

	lpToken=Dos9_GetNextParameterEs(lpToken, lpParam);

	if (wcscasecmp(Dos9_EsToChar(lpParam), L"IN")
		|| lpToken == NULL ) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"FOR", FALSE);
		goto error;

	}


	/* Now, the program expect to be given the list of argument
	   of the FOR loop, i.e. the part that should be enclosed
	   on parenthesis */

	lpToken=Dos9_SkipBlanks(lpToken);
	/* it is not obvious that the lpToken pointer
	   points precisely to a non space character */

	if (*lpToken != L'(') {

		/* if no parenthesis-enclosed argument have been found
		   then, we return an error */

		Dos9_ShowErrorMessage(DOS9_ARGUMENT_NOT_BLOCK, lpToken, FALSE);
		goto error;

	}

	/* So just get back the block */
	lpToken=Dos9_GetNextBlockEs(lpToken, lpInputBlock);

	/* Now we check if ``DO'' is used */

	if (!(lpToken = Dos9_GetNextParameterEs(lpToken, lpParam))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"FOR", FALSE);
		goto error;

	}

	if ((wcscasecmp(Dos9_EsToChar(lpParam), L"DO"))) {

		/* if ``DO'' is not used */
		Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
		goto error;

	}

	lpToken = Dos9_GetNextBlock(lpToken, &bkCode);

	if (*lpToken) {

		lpToken++;

		lpToken=Dos9_SkipBlanks(lpToken);

		if (*lpToken) {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, Dos9_EsToChar(lpParam), FALSE);
			goto error;

		}

	}

	switch(iForType) {

		case FOR_LOOP_SIMPLE:

			/* this handles simple for */
			Dos9_CmdForSimple(lpInputBlock, &bkCode, cVarName, L" ,:;\n\t");

			break;

		case FOR_LOOP_R:
			Dos9_CmdForDeprecatedWrapper(lpInputBlock, lpDirectory, L"/A:-D", &bkCode, cVarName);
			break;

		case FOR_LOOP_D:
			Dos9_CmdForDeprecatedWrapper(lpInputBlock, lpDirectory, L"/A:D",&bkCode, cVarName);
			break;

		case FOR_LOOP_F:
			Dos9_CmdForF(lpInputBlock, &bkCode, &forInfo);
			break;

		case FOR_LOOP_L:
			Dos9_CmdForL(lpInputBlock, &bkCode, cVarName);
	}

	Dos9_EsFree(lpParam);
	Dos9_EsFree(lpDirectory);
	Dos9_EsFree(lpInputBlock);

	return 0;

error:
	Dos9_EsFree(lpParam);
	Dos9_EsFree(lpDirectory);
	Dos9_EsFree(lpInputBlock);

	return -1;
}

int Dos9_CmdForSimple(ESTR* lpInput, BLOCKINFO* lpbkCommand, wchar_t cVarName, wchar_t* lpDelimiters)
{

	wchar_t *lpToken=Dos9_EsToChar(lpInput);
	wchar_t *lpNextToken;

	char  cInc=TRUE;

	while (*lpToken) {

		while (*lpToken && (wcschr(lpDelimiters, *lpToken)))
			lpToken++;

		if (!*lpToken)
			break;
		/* skip in raw delimiters */

		if (*lpToken==L'\''  || *lpToken==L'"') {

			/* the tokens begins with either a "'" or a '"'
			   then we try to find the closing delimiter */

			/* no need to increment lpToken since quotes are
			   preserved, as stated by the documentation */
			lpNextToken=lpToken+1;

			while (*lpNextToken) {


				if (*lpNextToken==*lpToken) {

					if ((wcschr(lpDelimiters, *(lpNextToken+1)))) {

						lpNextToken++;
						if (*lpNextToken)
							*(lpNextToken++)=L'\0';

						cInc=FALSE;

						break;

					}

				}

				lpNextToken++;

			}

		} else {

			/* if we do not found quotes, then
			   look for the following delimiter */

			lpNextToken=lpToken;

			while (*lpNextToken && !(wcschr(lpDelimiters, *lpNextToken))) {

				lpNextToken++;

			}

			if (*lpNextToken) {

				*(lpNextToken++)=L'\0';
				cInc=FALSE;

			}

		}

		/* assign the local variable */
		if (!Dos9_SetLocalVar(lpvLocalVars, cVarName, lpToken)) {


			return -1;

		}

		/* run the block */
		Dos9_RunBlock(lpbkCommand);

		if (*lpNextToken && cInc) {

			lpToken=lpNextToken+1;

		} else {

			lpToken=lpNextToken;
			cInc=TRUE;

		}

		/* if a goto as been executed while the for-loop
		   was ran */

		if (bAbortCommand==TRUE)
			break;

	}

	/* delete the special var */
	Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);


	return 0;

}

int Dos9_CmdForL(ESTR* lpInput, BLOCKINFO* lpbkCommand, wchar_t cVarName)
{
	int iLoopInfo[3]= {0,0,0}, /* loop information */
		i;

	wchar_t	lpValue[]=L"-3000000000",
			*lpToken;

	lpToken=Dos9_EsToChar(lpInput);
	i=0;

	while (*lpToken && (i < sizeof(iLoopInfo))) {

		iLoopInfo[i]=wcstol(lpToken, &lpToken, 0);
		/* accept either Hexadecimal, Decimal and Octal*/


		if (*lpToken==L',') {

			lpToken++;

		} else if (!*lpToken) {

		} else {

			Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER,
									Dos9_EsToChar(lpInput),
									FALSE
									);

			goto error;

		}

		i++;

	}

	if (*lpToken) {

		Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER,
								Dos9_EsToChar(lpInput),
								FALSE
								);
		goto error;

	}

	/* execute loop */
	for (i=iLoopInfo[DOS9_FORL_BEGIN];
	     i<=iLoopInfo[DOS9_FORL_END];
	     i+=iLoopInfo[DOS9_FORL_INC]) {

		swprintf(lpValue, sizeof(lpValue)/sizeof(wchar_t), L"%d", i);

		Dos9_SetLocalVar(lpvLocalVars, cVarName, lpValue);

		/* execute the code */

		Dos9_RunBlock(lpbkCommand);


		/* if a goto as been executed while the for-loop
		   was ran */
		if (bAbortCommand==TRUE)
			break;

	}

	Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);

	return 0;

error:

	return -1;

}

int Dos9_CmdForF(ESTR* lpInput, BLOCKINFO* lpbkInfo, FORINFO* lpfrInfo)
{
	int iSkip=lpfrInfo->iSkip;

	ESTR* lpInputToP=Dos9_EsInit();

	INPUTINFO inputInfo;

	//printf("Starting for LOOP \n");


	if ((Dos9_ForVarCheckAssignment(lpfrInfo)))
		goto error;

	if ((Dos9_ForMakeInputInfo(lpInput, &inputInfo, lpfrInfo) == -1))
		goto error;

	while (Dos9_ForGetInputLine(lpInputToP, &inputInfo)) {

		/* printf("Processing :\n \"%s\"\n", Dos9_EsToChar(lpInputToP)); */

		if (iSkip > 0) {

			iSkip--;
			/* skip the n-th first lines, as specifiied by
			   ``skip=n'' in the command line */

		} else {

			/* printf("Splitting Token !\n"); */

			Dos9_ForSplitTokens(lpInputToP, lpfrInfo);
			/* split the block on subtokens */

			/* printf("Running Block !\n"); */
			Dos9_RunBlock(lpbkInfo);
			/* split the input into tokens and then run the
			    block */
		}


		/* if a goto as been executed while the for-loop
		   was ran */

		if (bAbortCommand==TRUE)
			break;

	}

	Dos9_ForCloseInputInfo(&inputInfo);
	Dos9_ForVarUnassign(lpfrInfo);

	Dos9_EsFree(lpInputToP);

	return 0;

error:
	Dos9_EsFree(lpInputToP);

	return -1;

}

/* ************************************************************
    FORINFO functions
*/

int Dos9_ForMakeInfo(wchar_t* lpOptions, FORINFO* lpfiInfo)
/*
    Fill the FORINFO structure with the appropriate
    informations
*/
{
	ESTR* lpParam=Dos9_EsInit();
	wchar_t* lpToken;
	int iReturn=0;

	while ((lpOptions = Dos9_GetNextParameterEs(lpOptions, lpParam))) {

		if (!(wcscasecmp(Dos9_EsToChar(lpParam), L"usebackq"))) {
			/* if the script specifies "usebackq" it will change dos9 behaviour
			    of the for /f loop */
			lpfiInfo->bUsebackq=TRUE;

			continue;

		}



		if ((lpToken = wcschr(Dos9_EsToChar(lpParam), L'='))) {

			*lpToken=L'\0';
			lpToken++;

		} else {

			/* if no '=' was found, then the entry are just
			   wrong */
			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
									Dos9_EsToChar(lpParam),
									FALSE
									);

			goto error;

		}

		/* switching to choice the appropriate parameter */

		if (!(wcscasecmp(Dos9_EsToChar(lpParam), L"delims"))) {

			Dos9_ForAdjustParameter(lpOptions, lpParam);

			wcsncpy(lpfiInfo->lpDelims,
					lpToken,
					sizeof(lpfiInfo->lpDelims)/sizeof(wchar_t)
					);

			lpfiInfo->lpDelims[sizeof(lpfiInfo->lpDelims)/sizeof(wchar_t)-1]
				= L'\0';
			/* see C89 or later standard */


		} else if (!(wcscasecmp(Dos9_EsToChar(lpParam), L"skip"))) {

			lpfiInfo->iSkip=wcstol(lpToken, NULL, 0);


		} else if (!(wcscasecmp(Dos9_EsToChar(lpParam), L"eol"))) {

			Dos9_ForAdjustParameter(lpOptions, lpParam);

			wcsncpy(lpfiInfo->lpEol,
					lpToken,
					sizeof(lpfiInfo->lpEol)/sizeof(wchar_t));

			lpfiInfo->lpEol[sizeof(lpfiInfo->lpEol)/sizeof(wchar_t)-1] = L'\0';

		} else if (!(wcscasecmp(Dos9_EsToChar(lpParam), L"tokens"))) {

			/* make the options */
			if (Dos9_ForMakeTokens(lpToken, lpfiInfo)==-1) {

				goto error;

			}

		} else {

			Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT,
									Dos9_EsToChar(lpParam),
									FALSE);

			goto error;

		}

	}


	Dos9_EsFree(lpParam);

	return iReturn;

error:
	Dos9_EsFree(lpParam);
	return -1;

}

int  Dos9_ForMakeTokens(wchar_t* lpToken, FORINFO* lpfrInfo)
{
	/* this is used to determine the useffull tokens for
	   the parsing */

	wchar_t* const lpOriginTok=lpToken;
	wchar_t* lpNextToken;

	int iTokenNb,
		isCat=FALSE,
		iPrevTok,
		i=0;

	lpfrInfo->lpToken[0]=0;

	while (lpToken) {

		iTokenNb=wcstol(lpToken, &lpNextToken, 0);

		switch (*lpNextToken) {

			case L',':
				/* just save the token to high low order word */

				if (!isCat) {

					lpfrInfo->lpToken[i]|=TOHIGH(iTokenNb)+TOLOW(iTokenNb);

				} else {

					lpfrInfo->lpToken[i]|=TOLOW(iTokenNb);
					isCat=FALSE;

					break;
				}

				if (i >= TOKENINFO_NB_MAX ) {

					Dos9_ShowErrorMessage(DOS9_FOR_TOKEN_OVERFLOW,
											(wchar_t*)TOKENINFO_NB_MAX,
											FALSE);

					return -1;

				}



				lpfrInfo->lpToken[++i]=0;

				lpNextToken++;

				break;


			case L'-':

				if (isCat) {

					Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER,
											lpOriginTok,
											FALSE
											);

					return -1;

				}


				isCat=TRUE;

				lpfrInfo->lpToken[i]|=TOHIGH(iTokenNb);

				lpNextToken++;

				break;

			case L'\0':

				if (iTokenNb) {

					if (!isCat) {

						lpfrInfo->lpToken[i]=TOHIGH(iTokenNb)+TOLOW(iTokenNb);

					} else {

						lpfrInfo->lpToken[i]|=TOLOW(iTokenNb);

					}

				} else {

					if (isCat) {

						Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER,
													lpOriginTok,
													FALSE);

						return -1;

					}

				}


				lpfrInfo->lpToken[++i]=0;

				lpNextToken=NULL;
				break;

			case L'*':

				/* some unlawful syntaxes are allowed

				    4-* -> in this case isCat is TRUE
				    4* -> in this case isCat is set to false and iTokenNb=4
				    3,* -> in this case, the token 3 is select, and next tokens
				    	   too.

				*/

				if (!isCat) {

					if (iTokenNb != 0) {

						lpfrInfo->lpToken[i]|=TOHIGH(iTokenNb);


					} else if (i > 0) {

						iPrevTok=LOWWORD(lpfrInfo->lpToken[i-1]);

						if (iPrevTok == -1) iPrevTok=0;


						lpfrInfo->lpToken[i]|=TOHIGH(iPrevTok+1);

					} else {

						lpfrInfo->lpToken[i]|=TOHIGH(1);

					}

				}

				lpfrInfo->lpToken[i]|=TOLOW(ALL_TOKEN);

				lpfrInfo->lpToken[++i]=0;

				lpNextToken++;

				if (!*lpNextToken) break;

				if (*lpNextToken!=L',') {

					Dos9_ShowErrorMessage(DOS9_FOR_BAD_TOKEN_SPECIFIER, lpNextToken, FALSE);
					return -1;

				}

				lpNextToken++;

				break;

			default:

				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
				return -1;

		}

		lpToken=lpNextToken;

	}


	lpfrInfo->iTokenNb=i;


	return 0;

}

void Dos9_ForAdjustParameter(wchar_t* lpOptions, ESTR* lpParam)
{
	wchar_t lpTemp[2]= {0,0};

	/* this is old-style options specifications ie,
	       specifying every options on the same argument
	       like

	        "tokens=1,2 delims=,; eol=,#"

	 */


	if (*lpOptions != L'\0') {

		if (*(lpOptions+1) == L' ' || *(lpOptions+1) == L'\t') {

			lpTemp[0]=*lpOptions;

			Dos9_EsCpy(lpParam, lpTemp);

			if (*(lpOptions+2) == L' ' || *(lpOptions+2) == L'\t') {

				lpTemp[0]=*lpOptions;

				Dos9_EsCpy(lpParam, lpOptions);

			}
		}
	}
}


/* ************************************************************
    tokenization function
*/


void Dos9_ForSplitTokens(ESTR* lpContent, FORINFO* lpfrInfo)
{

	ESTR* lpVarContent=Dos9_EsInit();

	int i;
	wchar_t *lpToken,
	     	*lpEol=lpfrInfo->lpEol,
	     	cVarName=lpfrInfo->cFirstVar;


	if ((lpToken=wcspbrk(Dos9_EsToChar(lpContent), lpEol))) {

		*lpToken=L'\0';

	}
	/* Truncate line at the first apparition of a end-of-line
	   charcaters

	   As specified by ``eol=;'' for example

	   */

	for (i=0; i<lpfrInfo->iTokenNb; i++) {


		/* printf("Getting token n. %d \n", i); */
		Dos9_ForGetToken(lpContent, lpfrInfo, i, lpVarContent);
		/* printf("Returned : \"%s\"\n", Dos9_EsToChar(lpVarContent */
		/* get the token descibed by the token info number i */

		Dos9_SetLocalVar(lpvLocalVars, cVarName, Dos9_EsToChar(lpVarContent));
		/* set the variable */

		cVarName++;


		/* the following code is propably to be removed in following Dos9
		   revisions, since, as we do not use multibyte characters anymore,
		   there is no need for such restrictions */
		if (cVarName & 0x80) {

			cVarName=0x21;

		}

	}

	Dos9_EsFree(lpVarContent);

}

void Dos9_ForGetToken(ESTR* lpContent, FORINFO* lpfrInfo, int iPos, ESTR* lpReturn)
{
	wchar_t *lpToken=Dos9_EsToChar(lpContent),
			*lpDelims=lpfrInfo->lpDelims,
	        *lpNextToken,
			*lpBeginToken=NULL,
			*lpEndToken=NULL;

	int   iLength,
	      iTokenPos=1,
	      iTokenBegin=HIGHWORD(lpfrInfo->lpToken[iPos]),
	      iTokenEnd=LOWWORD(lpfrInfo->lpToken[iPos]);

	Dos9_EsCpy(lpReturn, L"");

	/*  the type of line that arrives here is already truncated
	    at the first character specified in eof parameter */

	/* printf("Getting token nb. %d\n"
	       "\t* iTokenBegin = %d\n"
	       "\t* iTokenEnd   = %d\n"
	       "\t* lpDelims    = \"%s\"\n\n", iPos, iTokenBegin, iTokenEnd, lpDelims);

	*/

	while ((lpNextToken = wcspbrk(lpToken, lpDelims))) {
		/* get next first occurence of a lpDelims character */

		if (lpNextToken == lpToken) {

			/* this is already a delimiter
			   just ignore it */

			lpToken=lpNextToken+1;

			continue;

		}

		/* we skipped all delimiters we are now in
		   processing */

		if (iTokenPos == iTokenBegin) {

			lpBeginToken=lpToken;

		}

		if (iTokenPos == iTokenEnd) {

			lpEndToken=lpNextToken;
			break;

		}

		iTokenPos++;
		lpToken=lpNextToken+1;

	}

	if ((iTokenPos==iTokenBegin) && !lpBeginToken) {

		lpBeginToken=lpToken;
	}

	/* printf("Get data loop Ended\n");

	printf("lpBeginToken=\"%s\"\n", lpBeginToken);
	if (lpEndToken)
	    printf("lpEndToken=\"%s\"\n", lpEndToken); */

	if (lpBeginToken) {

		if (lpEndToken) {

			iLength = lpEndToken - lpBeginToken;

			Dos9_EsCpyN(lpReturn, lpBeginToken, iLength);


		} else {

			Dos9_EsCpy(lpReturn, lpBeginToken);

		}

	}

	/* printf("Returned from token"); */

}

void Dos9_ForVarUnassign(FORINFO* lpfrInfo)
{
	int i,
	    iMax=lpfrInfo->iTokenNb;
	wchar_t cVarName=lpfrInfo->cFirstVar;

	for (i=0; i<iMax; i++) {

		Dos9_SetLocalVar(lpvLocalVars, cVarName, NULL);

		if (cVarName & 0x80)
			break;
	}
}

int Dos9_ForVarCheckAssignment(FORINFO* lpfrInfo)
{
	int i,
	    iMax=lpfrInfo->iTokenNb;

	wchar_t cVarName=lpfrInfo->cFirstVar;

	for (i=0; i<iMax; i++) {

		if ((Dos9_GetLocalVarPointer(lpvLocalVars, cVarName))) {

			Dos9_ShowErrorMessage(DOS9_FOR_TRY_REASSIGN_VAR,
									(wchar_t*)((size_t)cVarName),
									FALSE
									);

			return -1;

		}

		if (cVarName & 0x80)
			break;
	}

	return 0;

}

/* ******************************************************************
   INPUTINFO Functions
*/

int Dos9_ForMakeInputInfo(ESTR* lpInput, INPUTINFO* lpipInfo, FORINFO* lpfrInfo)
{
	int bUsebackq=lpfrInfo->bUsebackq,
	    iInputType,
	    iPipeFdIn[2],
	    iPipeFdOut[2];

	wchar_t *lpToken=Dos9_EsToChar(lpInput);


	switch (*lpToken) {

		case L'\'':
			/* the given input is a command line to be executed */

			if (bUsebackq) {

				iInputType=INPUTINFO_TYPE_STRING;

			} else {

				iInputType=INPUTINFO_TYPE_COMMAND;

			}

			break;

		case L'`':
			/* the given input is a command line using Usebackq */
			if (!bUsebackq) {

				Dos9_ShowErrorMessage(DOS9_FOR_USEBACKQ_VIOLATION,
										L"`",
										FALSE);

				return -1;

			}

			iInputType=INPUTINFO_TYPE_COMMAND;

			break;

		case L'"':
			/* the given input is a string */
			if (bUsebackq) {

				Dos9_ShowErrorMessage(DOS9_FOR_USEBACKQ_VIOLATION,
										L"\"",
										FALSE
										);

				return -1;

			}

			iInputType=INPUTINFO_TYPE_STRING;

			break;

		default:
			/* the input given is a filename */
			iInputType=INPUTINFO_TYPE_STREAM;
	}

	if (iInputType!=INPUTINFO_TYPE_STREAM) {

		if (Dos9_ForAdjustInput(lpToken))
			return -1;

	}

	switch (iInputType) {

		case INPUTINFO_TYPE_STREAM:

			lpipInfo->cType=INPUTINFO_TYPE_STREAM;

			if (!(lpipInfo->Info.pInputFile=wfopen(lpToken, L"r"))) {

				Dos9_ShowErrorMessage(DOS9_FILE_ERROR | DOS9_PRINT_C_ERROR,
										lpToken,
										FALSE);

				return -1;

			}

			break;

		case INPUTINFO_TYPE_STRING:

			lpipInfo->cType=INPUTINFO_TYPE_STRING;

			if (!(lpipInfo->Info.StringInfo.lpString=wcsdup(lpToken))) {

				Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
				                      L(__FILE__) "/Dos9_MakeInputInfo()",
				                      TRUE);

			}

			lpipInfo->Info.StringInfo.lpToken=lpipInfo->Info.StringInfo.lpString;

			break;

		case INPUTINFO_TYPE_COMMAND:
			/* this is a litte bit more difficult (indeed, platform dependent)
			   the program should open a pipe, print the script and then get the output

			   The command is

			        Dos9 /Q /N /E /V /D n

			   The /Q avoid the presentation to be printed and the title to be change

			*/

			lpipInfo->cType=INPUTINFO_TYPE_COMMAND;


			if (_Dos9_Pipe(iPipeFdIn, 1024, O_TEXT) == -1) {

				Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR,
										L(__FILE__) "/Dos9_MakeInputInfo()",
										FALSE
										);

				return -1;

			}

			if (_Dos9_Pipe(iPipeFdOut, 1024, O_TEXT) == -1) {

				Dos9_ShowErrorMessage(DOS9_CREATE_PIPE | DOS9_PRINT_C_ERROR,
										L(__FILE__) "/Dos9_MakeInputInfo()",
										FALSE
										);

				return -1;

			}

			if (Dos9_ForInputProcess(lpInput, lpipInfo, iPipeFdIn, iPipeFdOut)==-1) {

				return -1;

			}

	}

	return 0;

}

int Dos9_ForAdjustInput(wchar_t* lpInput)
{
	const wchar_t* lpBegin=lpInput;
	wchar_t* lpToken=lpInput+1;
	wchar_t cBeginToken=*lpInput;

	if (cBeginToken!=L'"' && cBeginToken!=L'\'' && cBeginToken!=L'`') {

		Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER, lpBegin, FALSE);

		return -1;

	}

	while (*lpToken && *lpToken!=cBeginToken)
		*(lpInput++)=*(lpToken++);

	*lpInput=L'\0';

	if (*lpToken==cBeginToken) {

		return 0;

	} else {

		Dos9_ShowErrorMessage(DOS9_FOR_BAD_INPUT_SPECIFIER, lpBegin, FALSE);

		return -1;

	}

}

int Dos9_ForInputProcess(ESTR* lpInput, INPUTINFO* lpipInfo, int* iPipeFdIn, int* iPipeFdOut)
{
	wchar_t *lpArgs[10],
			lpInArg[16],
			lpOutArg[16];

	FILE* pFile;

	int i=0;

#ifdef _POSIX_C_SOURCE

	/* on POSIX systems, we need to know the child process ID in order
	   to process to fork */
	int iPid;

#endif

	lpArgs[i++]=L"dos9";
	lpArgs[i++]=L"/Q";
	lpArgs[i++]=L"/E";

	if (bDelayedExpansion)
		lpArgs[i++]=L"/V";

	if (bCmdlyCorrect)
		lpArgs[i++]=L"/C";

	lpArgs[i++]=L"/I";

	swprintf(lpInArg, sizeof(lpInArg)/sizeof(wchar_t), L"%d", iPipeFdIn[0]);

	lpArgs[i++]=lpInArg;

	lpArgs[i++]=L"/O";

	swprintf(lpOutArg,
				sizeof(lpOutArg)/sizeof(wchar_t),
				L"%d",
				iPipeFdOut[1]
				);

	lpArgs[i++]=lpOutArg;

	/* give the input descriptor to the child */

	lpArgs[i]=NULL;

#ifdef WIN32

	_spawnvp(_P_NOWAIT, lpArgs[0], lpArgs);

	if (errno == ENOENT) {

		Dos9_ShowErrorMessage(DOS9_FOR_LAUNCH_ERROR | DOS9_PRINT_C_ERROR,
		                      Dos9_EsToChar(lpInput),
		                      FALSE);

		return -1;

	}

#elif defined _POSIX_C_SOURCE

	iPid=fork();

	if (iPid == 0 ) {
		/* if we are in the son */

		if ( execvp(lpArgs[0], lpArgs) == -1) {

			Dos9_ShowErrorMessage(DOS9_FOR_LAUNCH_ERROR | DOS9_PRINT_C_ERROR,
			                      Dos9_EsToChar(lpInput),
			                      FALSE);
			return -1;

		}

		exit(0);

	} else {
		/* if we are in the father */

		if (iPid == (pid_t)-1) {

			/* the execution failed */
			Dos9_ShowErrorMessage(DOS9_FOR_LAUNCH_ERROR | DOS9_PRINT_C_ERROR,
			                      Dos9_EsToChar(lpInput),
			                      FALSE);
			return -1;

		}

	}

#endif // WIN32

	if (!(pFile=fdopen(iPipeFdIn[1], "w"))) {

		close(iPipeFdIn[1]);
		goto error;

	}

	fputws(Dos9_EsToChar(lpInput), pFile);
	fputws(L" & exit\n", pFile);
	fflush(pFile);
	/* write the command given on the for command */

	fclose(pFile);

	if (!(pFile=wfdopen(iPipeFdOut[0], L"r"))) {

		goto error;

	}

	close(iPipeFdIn[1]);
	close(iPipeFdOut[1]);

	lpipInfo->Info.pInputFile=pFile;

	return 0;

error:
	close(iPipeFdIn[0]);
	close(iPipeFdOut[1]);
	close(iPipeFdOut[0]);

	return -1;
}

int Dos9_ForGetStringInput(ESTR* lpReturn, STRINGINFO* lpsiInfo)
{
	wchar_t *lpToken=lpsiInfo->lpToken,
			*lpBeginToken=lpToken;
	size_t iLength;

	if (!*lpToken)
		return 1;

	while (*lpToken && *lpToken!=L'\n') lpToken++;

	iLength = lpToken - lpBeginToken;

	Dos9_EsCpyN(lpReturn, lpBeginToken, iLength);

	if (*lpToken)
		lpToken++;

	lpsiInfo->lpToken=lpToken;

	return 0;

}

int Dos9_ForGetInputLine(ESTR* lpReturn, INPUTINFO* lpipInfo)
{

	int iReturn=0;
	wchar_t* lpToken;

loop_begin:

	switch (lpipInfo->cType) {

		case INPUTINFO_TYPE_COMMAND:
		case INPUTINFO_TYPE_STREAM:
			iReturn=!Dos9_EsGet(lpReturn, lpipInfo->Info.pInputFile);
			break;

		case INPUTINFO_TYPE_STRING:
			iReturn=!Dos9_ForGetStringInput(lpReturn,
											&(lpipInfo->Info.StringInfo)
											);

	}

	if (bCmdlyCorrect) {

		lpToken=Dos9_EsToChar(lpReturn);
		lpToken=Dos9_SkipBlanks(lpToken);

		if ((*lpToken==L'\0'
		     || *lpToken==L'\n')
		    && iReturn) {

			/* this is a blank line, and the CMDLYCORRECT
			   is enabled, that means that we *must* loop
			   back and get another line, since cmd.exe
			   strips blank lines from the input.
			 */

			goto loop_begin;

		}

	}

	if ((lpToken=wcschr(Dos9_EsToChar(lpReturn), L'\n')))
		*lpToken=L'\0';

	return iReturn;
}

void Dos9_ForCloseInputInfo(INPUTINFO* lpipInfo)
{

	switch(lpipInfo->cType) {

		case INPUTINFO_TYPE_STREAM:
		case INPUTINFO_TYPE_COMMAND:

			fclose(lpipInfo->Info.pInputFile);
			break;


		case INPUTINFO_TYPE_STRING:

			free(lpipInfo->Info.StringInfo.lpString);

	}

}

/* Wrapper for deprecated old FOR /R */
int  Dos9_CmdForDeprecatedWrapper(ESTR* lpMask, ESTR* lpDir, wchar_t* lpAttribute, BLOCKINFO* lpbkCode, wchar_t cVarName)
{
	FORINFO forInfo= {
		L" ", /* no tokens delimiters, since only one
                token is taken account */
		L"", /* no end-of-line delimiters */
		0, /* no skipped line */
		cVarName, /* this is to be fullfiled later (the
                name letter of loop special var) */
		FALSE,
		1, /* the number of tokens we were given */
		{TOHIGH(1)|TOLOW(-1)} /* get all the token
                                    back */

	};

	ESTR* lpCommandLine=Dos9_EsInit();

	/* create equivalent for loop using for / */
	Dos9_EsCpy(lpCommandLine, L"'DIR /b /s ");
	Dos9_EsCat(lpCommandLine, lpAttribute);
	Dos9_EsCat(lpCommandLine, L" ");
	Dos9_EsCatE(lpCommandLine, lpDir);
	Dos9_EsCat(lpCommandLine, L"/");
	Dos9_EsCatE(lpCommandLine, lpMask);
	Dos9_EsCat(lpCommandLine, L"'");

	if (Dos9_CmdForF(lpCommandLine, lpbkCode, &forInfo))
		goto error;

	Dos9_EsFree(lpCommandLine);
	return 0;

error:

	Dos9_EsFree(lpCommandLine);
	return -1;

}
