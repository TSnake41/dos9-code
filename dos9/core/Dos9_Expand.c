/* Dos9_Expand.c - Dos9 copyleft (c) DarkBatcher 2012 - Some rights reserved

   The following file contain definition of funtions that are used for expansion of vars
   (including delayed expansion) and some functions used for parsing these variable.

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

	DOS9_DBG("[ExpandSpecialVar] : \"%s\".\n", Dos9_EsToChar(ptrCommandLine));

}

void Dos9_ExpandVar(ESTR* ptrCommandLine, char cDelimiter, ESTR** buf)
{

	ESTR *lpExpanded=buf[0],
         *lpVarContent=buf[1];

	char *ptrToken=Dos9_EsToChar(ptrCommandLine),
	      *ptrNextToken,
	      *ptrEndToken;

	char lpDelimiter[3]= {cDelimiter, 0, 0};

	/* initialisation du buffer de sortie */

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

		} else if (isdigit(*ptrNextToken)
                    || *ptrNextToken == '*'
                    || *ptrNextToken=='~') {

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

	Dos9_EsCat(lpExpanded, ptrToken); // si pas de s�quence d�tect�e
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
