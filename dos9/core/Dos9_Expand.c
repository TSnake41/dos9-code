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
#include <wchar.h>
#include <wctype.h>

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

void Dos9_ExpandSpecialVar(ESTR* ptrCommandLine)
{

	ESTR* lpVarContent=Dos9_EsInit();
	ESTR* lpExpanded=Dos9_EsInit();

	wchar_t *lpToken=Dos9_EsToChar(ptrCommandLine),
		 	*lpNextToken,
		 	*lpPreviousToken=lpToken,
		 	*lpTokenBegin;

	while ((lpNextToken=Dos9_SearchChar(lpToken, L'%'))) {

		lpTokenBegin=lpNextToken+1;

		if ((lpTokenBegin=Dos9_GetLocalVar(lpvLocalVars, lpTokenBegin, lpVarContent))) {

			/* si la variable est bien définie */
			*lpNextToken=L'\0';

			Dos9_EsCat(lpExpanded, lpPreviousToken);
			Dos9_EsCatE(lpExpanded, lpVarContent);

			lpToken=lpTokenBegin;
			lpPreviousToken=lpTokenBegin;

		} else {

			/* si la variable n'est pas définie */
			lpToken=lpNextToken+1;

		}


	}

	Dos9_EsCat(lpExpanded, lpPreviousToken);
	Dos9_EsCpyE(ptrCommandLine, lpExpanded);

	Dos9_EsFree(lpVarContent);
	Dos9_EsFree(lpExpanded);

	DOS9_DBG(L"[ExpandSpecialVar] : \"%s\".\n", Dos9_EsToChar(ptrCommandLine));

}

void Dos9_ExpandVar(ESTR* ptrCommandLine, wchar_t cDelimiter)
{

	ESTR* lpExpanded=Dos9_EsInit();
	ESTR* lpVarContent=Dos9_EsInit();

	wchar_t *ptrToken=Dos9_EsToChar(ptrCommandLine),
	        *ptrNextToken,
	        *ptrEndToken;

	wchar_t lpDelimiter[3]= {cDelimiter, 0, 0};

	/* initialisation du buffer de sortie */
	Dos9_EsCpy(lpExpanded,L"");

	while ((ptrNextToken=Dos9_SearchChar(ptrToken, cDelimiter))) {

		DEBUG(ptrToken);
		*ptrNextToken=L'\0';
		ptrNextToken++; // on passe au caractère suivant

		if ((*ptrNextToken==cDelimiter)) {

			// si un % est échappé via %%

			/* on supprime le caractère qui peut éventuellement
			   trainer dans ce buffer */
			lpDelimiter[1]=L'\0';

			Dos9_EsCat(lpExpanded, ptrToken);
			Dos9_EsCat(lpExpanded, lpDelimiter);
			ptrToken=ptrNextToken+1;
			continue;

		} else if (iswdigit(*ptrNextToken) || *ptrNextToken==L'~') {

			/* if the variable is one of the parameter variables,
			   then skip, for compatibility purpose
			 */
			lpDelimiter[1]=*ptrNextToken;
			Dos9_EsCat(lpExpanded, ptrToken);
			Dos9_EsCat(lpExpanded, lpDelimiter);
			ptrToken=ptrNextToken+1;
			continue;


		} else if ((ptrEndToken=Dos9_StrToken(ptrNextToken, cDelimiter))) {

			*ptrEndToken=L'\0';

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

	Dos9_EsCat(lpExpanded, ptrToken); // si pas de séquence détectée
	Dos9_EsCpy(ptrCommandLine, Dos9_EsToChar(lpExpanded));

	DOS9_DBG(L"[ExpandVar] : '%s'\n",
	         Dos9_EsToChar(ptrCommandLine)
	        );

	Dos9_EsFree(lpExpanded);
	Dos9_EsFree(lpVarContent);

}

void Dos9_DelayedExpand(ESTR* ptrCommandLine, char cEnableDelayedExpansion)
{
	Dos9_ExpandSpecialVar(ptrCommandLine);

	if (cEnableDelayedExpansion) {

		Dos9_ExpandVar(ptrCommandLine, L'!');

	}
}

char* Dos9_StrToken(wchar_t* lpString, wchar_t cToken)
{
	if (lpString == NULL)
		return NULL;

	for (; *lpString!=L'\0'; lpString++) {

		if (*lpString==cToken) return lpString;

	}

	return NULL;

}
