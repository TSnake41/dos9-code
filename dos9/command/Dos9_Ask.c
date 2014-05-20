#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include <libDos9.h>
#include <libw.h>

#include "Dos9_Ask.h"
#include "../lang/Dos9_Lang.h"

int Dos9_AskConfirmation(int iFlags, const wchar_t* lpMsg, ...)
{
	va_list vaArgs;
	const wchar_t *lpChoices=NULL;
	wchar_t *lpLf;

	int iRet;

	ESTR* lpInput=Dos9_EsInit();

	if (iFlags & DOS9_ASK_YNA) {

		/* if the user choosed to display YES NO ALL
		   options */

		switch(iFlags & (DOS9_ASK_DEFAULT_Y | DOS9_ASK_DEFAULT_N |
		                 DOS9_ASK_DEFAULT_A)) {


			case DOS9_ASK_DEFAULT_Y:
				lpChoices=lpAskYna;
				break;

			case DOS9_ASK_DEFAULT_N:
				lpChoices=lpAskyNa;
				break;

			case DOS9_ASK_DEFAULT_A:
				lpChoices=lpAskynA;
				break;

			default:
				lpChoices=lpAskyna;
				break;


		}

	} else {

		/* if the user choosed to display only YES and NO */

		if (iFlags & DOS9_ASK_DEFAULT_A) {

			Dos9_EsFree(lpInput);
			return DOS9_ASK_INVALID;

		}

		switch (iFlags & (DOS9_ASK_DEFAULT_Y || DOS9_ASK_DEFAULT_N)) {

			case DOS9_ASK_DEFAULT_Y:
				lpChoices=lpAskYn;
				break;

			case DOS9_ASK_DEFAULT_N:
				lpChoices=lpAskyN;
				break;

			default:
				lpChoices=lpAskyn;

		}

	}

	va_start(vaArgs, lpMsg);

	/* loop until the user answer correctly */
	do {

		/* Print the message along with args */
		vfwprintf(stderr, lpMsg, vaArgs);

		/* Print choices */
		fputws(lpChoices, stderr);

		Dos9_EsGet(lpInput, stdin);

		if ((lpLf=wcschr(Dos9_EsToChar(lpInput), L'\n')))
			*lpLf=L'\0';

		if (!wcscasecmp(Dos9_EsToChar(lpInput), lpAskYes)
		    || !wcscasecmp(Dos9_EsToChar(lpInput), lpAskYesA)) {

			iRet=DOS9_ASK_YES;

		} else if (!wcscasecmp(Dos9_EsToChar(lpInput), lpAskNo)
		           || !wcscasecmp(Dos9_EsToChar(lpInput), lpAskNoA)) {

			iRet=DOS9_ASK_NO;

		} else {

			if ((iFlags & DOS9_ASK_YNA) && (
			        !wcscasecmp(Dos9_EsToChar(lpInput), lpAskAll)
			        || !wcscasecmp(Dos9_EsToChar(lpInput), lpAskAllA))) {


				iRet=DOS9_ASK_ALL;

			} else if ((iFlags & (DOS9_ASK_DEFAULT_Y | DOS9_ASK_DEFAULT_N
			                      | DOS9_ASK_DEFAULT_A))
			           && *Dos9_EsToChar(lpInput)==L'\0') {

				iRet=iFlags & (DOS9_ASK_DEFAULT_Y | DOS9_ASK_DEFAULT_N
				               | DOS9_ASK_DEFAULT_A);

			} else if (!(iFlags & DOS9_ASK_INVALID_REASK)) {

				/* If we are not requested to ask again if the
				   answer is invalid, return an invalid value
				 */

				iRet=DOS9_ASK_INVALID;

			} else {

				fputws(lpAskInvalid, stderr);

			}

		}

	} while (iRet==0);

	va_end(vaArgs);

	Dos9_EsFree(lpInput);
	return iRet;
}
