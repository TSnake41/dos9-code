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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

void Dos9_ExpandSpecialVar(DOS9CONTEXT* pContext, ESTR* ptrCommandLine)
{

	ESTR* lpVarContent=Dos9_EsInit();
	ESTR* lpExpanded=Dos9_EsInit();

	char *lpToken=Dos9_EsToChar(ptrCommandLine),
	      *lpNextToken,
	      *lpPreviousToken=lpToken,
	       *lpTokenBegin;

	while ((lpNextToken=Dos9_SearchChar(lpToken, '%'))) {

		lpTokenBegin=lpNextToken+1;

		if ((lpTokenBegin=Dos9_GetLocalVar(pContext->pLocalVars,
                                    lpTokenBegin, lpVarContent))) {

			/* si la variable est bien d�finie */
			*lpNextToken='\0';

			Dos9_EsCat(lpExpanded, lpPreviousToken);
			Dos9_EsCatE(lpExpanded, lpVarContent);

			lpToken=lpTokenBegin;
			lpPreviousToken=lpTokenBegin;

		} else {

			/* si la variable n'est pas d�finie */
			lpToken=lpNextToken+1;

		}


	}

	Dos9_EsCat(lpExpanded, lpPreviousToken);
	Dos9_EsCpyE(ptrCommandLine, lpExpanded);

	Dos9_EsFree(lpVarContent);
	Dos9_EsFree(lpExpanded);

	DOS9_DBG("[ExpandSpecialVar] : \"%s\".\n", Dos9_EsToChar(ptrCommandLine));

}

void Dos9_ExpandVar(DOS9CONTEXT* pContext, ESTR* ptrCommandLine, char cDelimiter)
{

	ESTR* lpExpanded=Dos9_EsInit();
	ESTR* lpVarContent=Dos9_EsInit();

	char *ptrToken=Dos9_EsToChar(ptrCommandLine),
	      *ptrNextToken,
	      *ptrEndToken;

	char lpDelimiter[3]= {cDelimiter, 0, 0};

	/* initialisation du buffer de sortie */
	Dos9_EsCpy(lpExpanded,"");

	while ((ptrNextToken=Dos9_SearchChar(ptrToken, cDelimiter))) {

		DEBUG(ptrToken);
		*ptrNextToken='\0';
		ptrNextToken++; // on passe au caract�re suivant

		if ((*ptrNextToken==cDelimiter)) {

			// si un % est �chapp� via %%

			/* on supprime le caract�re qui peut �ventuellement
			   trainer dans ce buffer */
			lpDelimiter[1]='\0';

			Dos9_EsCat(lpExpanded, ptrToken);
			Dos9_EsCat(lpExpanded, lpDelimiter);
			ptrToken=ptrNextToken+1;
			continue;

		} else if (isdigit(*ptrNextToken) || *ptrNextToken=='~') {

			/* if the variable is one of the parameter variables,
			   then skip, for compatibility purpose
			 */
			lpDelimiter[1]=*ptrNextToken;
			Dos9_EsCat(lpExpanded, ptrToken);
			Dos9_EsCat(lpExpanded, lpDelimiter);
			ptrToken=ptrNextToken+1;
			continue;


		} else if ((ptrEndToken=Dos9_StrToken(ptrNextToken, cDelimiter))) {

			*ptrEndToken='\0';

			Dos9_EsCat(lpExpanded, ptrToken);

			if ((Dos9_GetVar(pContext, ptrNextToken, lpVarContent))) {

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

	Dos9_EsCat(lpExpanded, ptrToken); // si pas de s�quence d�tect�e
	Dos9_EsCpy(ptrCommandLine, Dos9_EsToChar(lpExpanded));

	DOS9_DBG("[ExpandVar] : '%s'\n",
	         Dos9_EsToChar(ptrCommandLine)
	        );

	Dos9_EsFree(lpExpanded);
	Dos9_EsFree(lpVarContent);

}

void Dos9_DelayedExpand(DOS9CONTEXT* pContext, ESTR* ptrCommandLine)
{
	Dos9_ExpandSpecialVar(pContext, ptrCommandLine);

	if (pContext->iMode & DOS9_CONTEXT_DELAYED_EXPANSION) {

		Dos9_ExpandVar(pContext, ptrCommandLine, '!');

	}
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
