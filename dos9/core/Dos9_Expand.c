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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"
void Dos9_ExpandSpecialVar(ESTR* ptrCommandLine, ESTR** buf)
{

	ESTR *lpVarContent=buf[0],
         *lpExpanded=buf[1];

	char *lpToken=Dos9_EsToChar(ptrCommandLine),
	      *lpNextToken,
	      *lpPreviousToken=lpToken,
	       *lpTokenBegin;

	while ((lpNextToken=Dos9_SearchChar(lpToken, '%'))) {

		lpTokenBegin=lpNextToken+1;

		if ((lpTokenBegin=Dos9_GetLocalVar(lpvLocalVars, lpTokenBegin, lpVarContent))) {

			/* If we have a defined variable */
			*lpNextToken='\0';

			Dos9_EsCat(lpExpanded, lpPreviousToken);
			Dos9_EsCatE(lpExpanded, lpVarContent);

			lpToken=lpTokenBegin;
			lpPreviousToken=lpTokenBegin;

		} else {

			/* If the variable is not defined */
			lpToken=lpNextToken+1;

		}


	}

	Dos9_EsCat(lpExpanded, lpPreviousToken);
	Dos9_EsCpyE(ptrCommandLine, lpExpanded);

	DOS9_DBG("[ExpandSpecialVar] : \"%s\".\n", Dos9_EsToChar(ptrCommandLine));

}

void Dos9_ExpandVar(ESTR* ptrCommandLine, char cDelimiter, ESTR** buf)
{

	ESTR *lpExpanded=buf[0],
         *lpVarContent=buf[1];

	char *ptrToken=Dos9_EsToChar(ptrCommandLine),
	      *ptrNextToken,
	      *ptrEndToken;

    char lpDelimiter[] = {cDelimiter, 0};

	/* initialisation du buffer de sortie */

	while ((ptrNextToken=Dos9_SearchChar(ptrToken, cDelimiter))) {

		DEBUG(ptrToken);
		*ptrNextToken='\0';
		ptrNextToken++;

		if (*ptrNextToken==cDelimiter) {

            /* a delimiter is escaped using double delimiter */

			Dos9_EsCat(lpExpanded, ptrToken);
			Dos9_EsCat(lpExpanded, lpDelimiter);
			ptrToken=ptrNextToken+1;
			continue;

		} else if ((cDelimiter == '%' && (isdigit(*ptrNextToken)
                    || *ptrNextToken == '*'
                    || *ptrNextToken == '+'
                    || *ptrNextToken=='~'))
                    && (ptrEndToken=Dos9_GetLocalVar(lpvArguments,
                                                      ptrNextToken, lpVarContent))) {

			/* We encountered a parameter variable that must be
			   develloped right here because it owns priority over
			   existing variables (eg, something like '2cent') whose
			   name start with a number or an Asterix */
            Dos9_EsCat(lpExpanded, ptrToken);
			Dos9_EsCatE(lpExpanded, lpVarContent);
			ptrToken=ptrEndToken;
			continue;


		} else if ((ptrEndToken=Dos9_StrToken(ptrNextToken, cDelimiter))) {

			*ptrEndToken='\0';

			Dos9_EsCat(lpExpanded, ptrToken);

			if ((Dos9_GetVar(ptrNextToken, lpVarContent))) {

				/* add the content of the variable only if
				   it is really defined */
				Dos9_EsCatE(lpExpanded, lpVarContent);

			}

			ptrToken=ptrEndToken+1;

			continue;

		} else {

			/* there is only a single %, that does not
			   match any following %, then, split it
			   from the input.
			 */

			Dos9_EsCat(lpExpanded, ptrToken);
			ptrToken=ptrNextToken;

		}
	}

	Dos9_EsCat(lpExpanded, ptrToken);
	Dos9_EsCpy(ptrCommandLine, Dos9_EsToChar(lpExpanded));

	DOS9_DBG("[ExpandVar] : '%s'\n",
	         Dos9_EsToChar(ptrCommandLine)
	        );

}

void Dos9_ReplaceVars(ESTR* lpEsStr)
{
    ESTR* buf[2] = {Dos9_EsInit(), Dos9_EsInit()};

    Dos9_ExpandVar(lpEsStr, '%', buf);

    Dos9_EsFree(buf[0]);
    Dos9_EsFree(buf[1]);

}


void Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion)
{
    ESTR* buf[2] = {Dos9_EsInit(), Dos9_EsInit()};

	Dos9_ExpandSpecialVar(ptrCommandLine, buf);

	*Dos9_EsToChar(buf[0]) = '\0';
    *Dos9_EsToChar(buf[1]) = '\0';

	if (cEnableDelayedExpansion) {

		Dos9_ExpandVar(ptrCommandLine, '!', buf);

	}

    Dos9_EsFree(buf[0]);
    Dos9_EsFree(buf[1]);
}

void Dos9_RemoveEscapeChar(char* lpLine)
{
	/* this function is designed to remove the escape characters
	   (e.g. char '^') from the command line */
	char  lastEsc=FALSE;
	char* lpPosition=lpLine;
	for (; *lpLine; lpLine++,lpPosition++) {
		if (*lpLine=='^' && lastEsc!=TRUE) {
			lpPosition--;
			lastEsc=TRUE;
			continue;
		}
		lastEsc=FALSE;
		if (lpPosition != lpLine) *lpPosition=*lpLine;
	}
	*lpPosition='\0';
}

char* Dos9_StrToken(char* lpString, char cToken)
{
	if (lpString == NULL)
		return NULL;

	for (; *lpString!='\0'; lpString++) {

		if (*lpString==cToken) return lpString;

	}

	return NULL;

}
