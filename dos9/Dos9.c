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


#if defined WIN32
#include <conio.h>
#endif

#include <libDos9.h>

#include "errors/Dos9_Errors.h"

#include "lang/Dos9_Lang.h"
#include "lang/Dos9_ShowHelp.h"

#include "core/Dos9_Core.h"

/* those are codes of internal commands */

#include "command/Dos9_Commands.h"
#include "command/Dos9_CommandInfo.h"

//#define DOS9_DBG_MODE
#include "core/Dos9_Debug.h"

#include "../config.h"

void Dos9_SigHandler(int c)
{
	/* this is to prevent ctrl-C or SIGINT to close the
	   the command prompt. */
}


int main(int argc, char *argv[])
{
	/*  a function which initializes Dos9's engine,
	    parses the command line argumments,
	    And display starting message
	*/

	char *lpFileName="",
	      lpFileAbs[FILENAME_MAX],
	      lpTmp[FILENAME_MAX],
	      lpTitle[FILENAME_MAX]="Dos9 [" DOS9_VERSION "] - ";


	int i,
	    j,
	    c='0',
	    bQuiet=FALSE,
	    bGetSwitch=TRUE,
	    iMode=DOS9_CONTEXT_ECHO_ON;

    DOS9CONTEXT* pContext;

	DOS9_DBG("Initializing signal handler...\n");

	signal(SIGINT, Dos9_SigHandler);

	DOS9_DBG("Initializing libDos9 ...\n");

	if (Dos9_LibInit() == -1) {

		puts("Error : Unable to load LibDos9. Exiting ...");
		exit(-1);
	}

	DOS9_DBG("Setting UNIX newlines ...\n");

	/* Set new line Mode to UNIX */
	Dos9_SetNewLineMode(DOS9_NEWLINE_UNIX);

	DOS9_DBG("Initializing console ...\n");

	Dos9_InitConsole();

	DOS9_DBG("Setting locale ...\n");

#ifdef WIN32

	SetThreadLocale(LOCALE_USER_DEFAULT);

#elif defined _POSIX_C_SOURCE

	setlocale(LC_ALL, "");

#endif // WINDOWS

	DOS9_DBG("Loading GETTEXT messages... \n");

	/* Load Messages (including errors) */
	Dos9_LoadStrings();
	Dos9_LoadErrors();
	Dos9_LoadInternalHelp();

	/* **********************************
	   *   getting Dos9's parameters    *
	   ********************************** */

	if (!argv[0])
		Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE,
		                      "Dos9",
		                      -1
		                     );

	DOS9_DBG("Getting command line arguments ... \n");

	/* get command line arguments */
	for (i=1; argv[i]; i++) {

		DOS9_DBG("* Got \"%s\" as argument...\n", argv[i]);

		if (*argv[i]=='/' && bGetSwitch) {
			argv[i]++;
			switch(toupper(*argv[i])) {

				case 'V':
					/* enables delayed expansion */
					iMode |= DOS9_CONTEXT_DELAYED_EXPANSION;
					break;

				case 'F':
					/* enables floats */
					iMode |= DOS9_CONTEXT_USE_FLOATS;
					break;

				case 'E':
					iMode &= ~DOS9_CONTEXT_ECHO_ON;
					break;

				case 'C':
                    iMode |= DOS9_CONTEXT_CMDLYCORRECT;
					break;

				case 'Q':
					bQuiet=TRUE; // run silently
					break;

				case 'I':
					if (!argv[++i]) {

						Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "Dos9", -1);

					}

					iInputD=atoi(argv[i]); // select input descriptor
					break;

				case 'O':
					if (!argv[++i]) {

						Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "Dos9", -1);

					}

					iOutputD=atoi(argv[i]); // select input descriptor
					break;

				case '?':
					puts("DOS9 [" DOS9_VERSION "] (" DOS9_HOST ") - " DOS9_BUILDDATE "\n"
					     "Copyright (c) 2010-" DOS9_BUILDYEAR " " DOS9_AUTHORS "\n\n"
					     "This is free software, you can modify and/or redistribute it under "
					     "the terms of the GNU Genaral Public License v3 (or any later version).\n");

					puts(lpHlpMain);

					puts("Feel free to report bugs and submit suggestions at : <" DOS9_BUGREPORT ">\n"
						 "For more informations see : <" DOS9_URL ">");
					return 0;

				case '/' :
					/* there is no more switch on the command line.
					   '' */
					bGetSwitch=FALSE;
					break;

				default:
					Dos9_ShowErrorMessage(DOS9_BAD_COMMAND_LINE, NULL, -1);

			}

		} else {

			if (*lpFileName!='\0') {

				/* set parameters for the file currently runned */
				for (j=i ,  c='1'; argv[j] && c<='9'; i++, c++ , j++ ) {

					Dos9_SetLocalVar(pContext->pLocalVars, c, argv[j]);

				}

				break;
			}

			lpFileName=argv[i];
			Dos9_SetLocalVar(pContext->pLocalVars, '0', lpFileName);
			c='1';
		}

	}

	/* empty remaining special vars */
	for (; c<='9'; c++)
		Dos9_SetLocalVar(pContext->pLocalVars, c , "");

	/* initialisation du syst�me de g�n�ration al�atoire */

	srand(time(NULL));

	colColor=DOS9_COLOR_DEFAULT;
	/* messages affich�s */

    pContext = Dos9_InitContext(lpCmdInfo, sizeof(lpCmdInfo)/sizeof(COMMANDINFO));
    pContext->iMode = iMode;

	DOS9_DBG("Setting introduction and DOS9_IS_SCRIPT ...\n");

	if (*lpFileName=='\0') {

		if (!bQuiet)
			Dos9_PrintIntroduction();

		strcat(lpTitle, "Command prompt");
		Dos9_setenv("DOS9_IS_SCRIPT", "false");

	} else {

		strncat(lpTitle, lpFileName, sizeof(lpTitle)-sizeof("Dos9 [" DOS9_VERSION "] - "));
		Dos9_setenv("DOS9_IS_SCRIPT","true");

		DOS9_DBG("[dos9] Runing \"%s\"\n", lpFileName);

	}

	if (!bQuiet) {

		Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
		Dos9_SetConsoleTitle(lpTitle);

	}

	DOS9_DBG("Getting current executable name ...\n");

	Dos9_GetExePath(lpTitle, FILENAME_MAX);

	DOS9_DBG("\tGot \"%s\" as name ...\n", lpTitle);

    pContext = Dos9_InitContext(lpCmdInfo, sizeof(lpCmdInfo)/sizeof(COMMANDINFO));

	lpInitVar[4]="DOS9_PATH";
	lpInitVar[5]=lpTitle;


	DOS9_DBG("Initializing variables ...\n");

	Dos9_InitVar(pContext, lpInitVar);
    Dos9_SetEnv(pContext->pEnv, "ERRORLEVEL", "0");

	DOS9_DBG("Mapping commands ... \n");


	/* getting input intialised (if they are specified) */

	if (iInputD) {

		Dos9_OpenOutputD(lppsStreamStack, iInputD, DOS9_STDIN);

	}

	if (iOutputD) {

		Dos9_OpenOutputD(lppsStreamStack, iOutputD, DOS9_STDOUT);

	}

	/* running auto batch initialisation */

	strcat(lpTitle, "/Dos9_Auto.bat");

	strcpy(pContext->pIn->lpFileName, lpTitle);

	DOS9_DBG("Running file \"%s\"\n", pContext->pIn->lpFileName);

	Dos9_RunBatch(pContext);

	DOS9_DBG("\tRan\n");

	if (*lpFileName!='\0') {

		/* generates real path if the path is uncomplete */

		if (Dos9_GetFilePath(lpFileAbs, lpFileName, sizeof(lpFileAbs))==-1)
			Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpFileName, -1);

		if (*lpFileAbs
			&& strncmp(lpFileAbs+1, ":\\", 2)
            && strncmp(lpFileAbs+1, ":/", 2)
            && *lpFileAbs!='/'
            ) {

            /* if the file path is relative */

            snprintf(lpTmp, sizeof(lpFileAbs), "%s/%s", pContext->lpCurrentDir, lpFileAbs);
            strcpy(lpFileAbs, lpTmp);

		}

		lpFileName=lpFileAbs;
	}

	/* run the batch */
	strcpy(pContext->pIn->lpFileName, lpFileName);
	pContext->pIn->bEof = 0;
	pContext->pIn->iPos = 0;

	DOS9_DBG("Running file \"%s\" ...\n", ifIn.lpFileName);

	Dos9_RunBatch(pContext);

	DOS9_DBG("\t Ran\nExiting...\n");

    Dos9_FreeContext(pContext);

	Dos9_Exit();

	Dos9_LibClose();

	return 0;

}
