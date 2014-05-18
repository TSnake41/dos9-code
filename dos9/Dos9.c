
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
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <wchar.h>
#include <wctype.h>

#if defined WIN32
#include <conio.h>
#endif

#include <libDos9.h>

#include <libw.h>

#include "errors/Dos9_Errors.h"

#include "lang/Dos9_Lang.h"
#include "lang/Dos9_Help.h"

#include "core/Dos9_Core.h"

/* those are codes of internal commands */

#include "command/Dos9_ScriptCommands.h"
#include "command/Dos9_Conditions.h"
#include "command/Dos9_For.h"
#include "command/Dos9_CommandInfo.h"

#define DOS9_DBG_MODE
#include "core/Dos9_Debug.h"


void Dos9_SigHandler(int c)
{
	/* this is to prevent ctrl-C or SIGINT to close the
	   the command prompt. */
}


int wmain(int argc, wchar_t *argv[])
{
	/*  a function which initializes Dos9's engine,
	    parses the command line argumments,
	    And display starting message
	*/

	wchar_t *lpFileName=L"",
	      lpFileAbs[FILENAME_MAX],
	      lpTmp[FILENAME_MAX],
	      lpTitle[FILENAME_MAX+10]=L"Dos9 [" DOS9_VERSION L"] - ",
	      i,
	      j,
	      c=L'0';

	int bQuiet=FALSE,
	    bGetSwitch=TRUE;

	DOS9_DBG(L"Initializing signal handler...\n");

	signal(SIGINT, Dos9_SigHandler);

	DOS9_DBG(L"Initializing libDos9 ...\n");

	if (Dos9_LibInit() == -1) {

		fputws(L"Error : Unable to load LibDos9. Exiting ...", stderr);
		exit(-1);

	}

	DOS9_DBG(L"Setting UNIX newlines ...\n");

	/* Set new line Mode to UNIX */
	Dos9_SetNewLineMode(DOS9_NEWLINE_UNIX);

	DOS9_DBG(L"Allocating local variable block ... \n");

	lpvLocalVars=Dos9_GetLocalBlock();

	DOS9_DBG(L"Initializing console ...\n");

	Dos9_InitConsole();

	DOS9_DBG(L"Setting locale ...\n");

#ifdef WIN32

	SetThreadLocale(LOCALE_USER_DEFAULT);

#elif defined _POSIX_C_SOURCE

	setlocale(LC_ALL, "");

#endif // WINDOWS

	DOS9_DBG(L"Loading GETTEXT messages... \n");

	/* Load Messages (including errors) */
	Dos9_LoadStrings();
	Dos9_LoadErrors();
	Dos9_LoadInternalHelp();

	DOS9_DBG(L"Loading current directory...\n");

	Dos9_UpdateCurrentDir();

	/* **********************************
	   *   getting Dos9's parameters    *
	   ********************************** */

	if (!argv[0])
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE,
		                      L"Dos9",
							-1
		                     );

	DOS9_DBG(L"Getting command line arguments ... \n");

	/* get command line arguments */
	for (i=1; argv[i]; i++) {

		DOS9_DBG(L"* Got \"%s\" as argument...\n", argv[i]);

		if (*argv[i]==L'/' && bGetSwitch) {
			argv[i]++;
			switch(towupper(*argv[i])) {

				case L'V':
					/* enables delayed expansion */
					bDelayedExpansion=TRUE;
					break;

				case L'F':
					/* enables floats */
					bUseFloats=TRUE;
					break;

				case L'E':
					bEchoOn=FALSE;
					break;

				case L'C':
					/* enable cmd-compatible mode */
					bCmdlyCorrect=TRUE;
					break;

				case L'Q':
					bQuiet=TRUE; // run silently
					break;

				case L'I':
					if (!argv[++i]) {

						Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"Dos9", -1);

					}

					iInputD=wtoi(argv[i]); // select input descriptor
					break;

				case L'O':
					if (!argv[++i]) {

						Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"Dos9", -1);

					}

					iOutputD=wtoi(argv[i]); // select input descriptor
					break;

				case L'?':
					putws(L"DOS9 [" DOS9_VERSION L"] - Build " DOS9_BUILDDATE L"\n"
					     L"Copyright (c) 2010-" DOS9_BUILDYEAR L" " DOS9_AUTHORS L"\n\n"
					     L"This is free software, you can modify and/or redistribute it under "
					     L"the terms of the GNU Genaral Public License.\n");

					putws(lpHlpMain);
					return 0;

				case L'/' :
					/* there is no more switch on the command line.
					   '' */
					bGetSwitch=FALSE;
					break;

				default:
					Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, -1);

			}

		} else {

			if (*lpFileName!=L'\0') {

				/* set parameters for the file currently runned */
				for (j=i ,  c=L'1'; argv[j] && c<=L'9'; i++, c++ , j++ ) {

					Dos9_SetLocalVar(lpvLocalVars, c, argv[j]);

				}

				break;
			}

			lpFileName=argv[i];
			Dos9_SetLocalVar(lpvLocalVars, L'0', lpFileName);
			c='1';
		}

	}

	/* empty remaining special vars */
	for (; c<=L'9'; c++)
		Dos9_SetLocalVar(lpvLocalVars, c , L"");

	/* initialisation du système de génération aléatoire */

	srand(time(NULL));

	colColor=DOS9_COLOR_DEFAULT;
	/* messages affichés */


	DOS9_DBG(L"Setting introduction and DOS9_IS_SCRIPT ...\n");

	if (*lpFileName=='\0') {

		if (!bQuiet)
			Dos9_PrintIntroduction();

		wcscat(lpTitle, L"Command prompt");
		Dos9_PutEnv(L"DOS9_IS_SCRIPT=false");

	} else {

		wcsncat(lpTitle, lpFileName, (sizeof(lpTitle)-sizeof("Dos9 [" DOS9_VERSION "] - "))/sizeof(wchar_t));
		Dos9_PutEnv(L"DOS9_IS_SCRIPT=true");

		DOS9_DBG(L"[dos9] Runing \"%s\"\n", lpFileName);

	}

	if (!bQuiet) {

		Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
		Dos9_SetConsoleTitle(lpTitle);

	}

	DOS9_DBG(L"Getting current executable name ...\n");

	wcscpy(lpTitle, L"DOS9_PATH=");

	Dos9_GetExePath(lpTitle+10, FILENAME_MAX);

	DOS9_DBG(L"\tGot \"%s\" as name ...\n", lpTitle+10);

	lpInitVar[2]=lpTitle;

	DOS9_DBG(L"Initializing variables ...\n");

	Dos9_InitVar(lpInitVar);

	Dos9_PutEnv(L"ERRORLEVEL=0");

	DOS9_DBG(L"Mapping commands ... \n");

	lpclCommands=Dos9_MapCommandInfo(lpCmdInfo, sizeof(lpCmdInfo)/sizeof(COMMANDINFO));

	DOS9_DBG(L"Initializing streams ... \n");

	lppsStreamStack=Dos9_InitStreamStack();


	/* getting input intialised (if they are specified) */

	if (iInputD) {

		Dos9_OpenOutputD(lppsStreamStack, iInputD, DOS9_STDIN);

	}

	if (iOutputD) {

		Dos9_OpenOutputD(lppsStreamStack, iOutputD, DOS9_STDOUT);

	}

	pErrorHandler=Dos9_Exit;

	/* running auto batch initialisation */


	wcscpy(ifIn.lpFileName, lpTitle+10);

	wcscat(ifIn.lpFileName, L"/Dos9_Auto.bat");
	ifIn.iPos=0;
	ifIn.bEof=FALSE;

	DOS9_DBG(L"Running file \"%s\"\n", ifIn.lpFileName);

	Dos9_RunBatch(&ifIn);

	DOS9_DBG(L"\tRan\n");

	if (*lpFileName!='\0') {

		/* generates real path if the path is uncomplete */

		if (Dos9_GetFilePath(lpFileAbs, lpFileName, sizeof(lpFileAbs)/sizeof(wchar_t))==-1)
			Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, -1);

		if (*lpFileAbs) {
			if (wcsncmp(lpFileAbs+1, L":\\", 2)
			    && wcsncmp(lpFileAbs+1, L":/", 2)
			    && *lpFileAbs!='/'
			   ) {
				/* if the file path is relative */

				snwprintf(lpTmp, sizeof(lpFileAbs)/sizeof(wchar_t), L"%s/%s",
							Dos9_GetCurrentDir(), lpFileAbs);
				wcscpy(lpFileAbs, lpTmp);

			}
		}

		lpFileName=lpFileAbs;
	}


	/* run the batch */
	wcscpy(ifIn.lpFileName, lpFileName);
	ifIn.iPos=0;
	ifIn.bEof=FALSE;

	DOS9_DBG(L"Running file \"%s\" ...\n", ifIn.lpFileName);

	Dos9_RunBatch(&ifIn);

	DOS9_DBG(L"\t Ran\nExiting...\n");


	Dos9_Exit();

	Dos9_LibClose();

	return 0;

}
