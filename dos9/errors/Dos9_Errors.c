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

#ifdef WIN32
#include <conio.h>
#else
#define getch() getchar()
#endif

#include <libDos9.h>
#include "Dos9_Errors.h"
#include "../core/Dos9_Core.h"

#include <libintl.h>
#include <wchar.h>

#include <libw.h>

const wchar_t* lpErrorMsg[DOS9_ERROR_MESSAGE_NUMBER];
const wchar_t* lpQuitMessage;



void Dos9_LoadErrors(void)
{
	wchar_t lpwPath[FILENAME_MAX];
	wchar_t lpwSharePath[FILENAME_MAX];
	char* lpPath;
	char lpEncoding[15]="UTF-16LE"; /* this is incompatible with various
									   operating systems, but why not use
									   built-in functionnalities */

	Dos9_GetExePath(lpwPath, FILENAME_MAX);

	//Dos9_GetConsoleEncoding(lpEncoding, sizeof(lpEncoding));

	swprintf(lpwSharePath, FILENAME_MAX, L"%s/share/locale", lpwPath);

	if (lpPath=libw_wcstombs(lpwSharePath)) {

		/* the string can be translated to mbs */

		wprintf(L"lpwPath : '%s'\n", lpwSharePath);
		wprintf(L"lpPath : '%S' \n", lpPath);

		bindtextdomain("Dos9-errors", lpPath);

		free(lpPath);

	} else {

		bindtextdomain("Dos9-errors", "/share/locale");

	}

	bind_textdomain_codeset("Dos9-errors", lpEncoding);

	textdomain("Dos9-errors");

	lpErrorMsg[DOS9_FILE_ERROR]=
	    (wchar_t*)gettext("Error : Unable to acces to file \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_DIRECTORY_ERROR]=
	    (wchar_t*)gettext("Error : Unable to find folder \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_COMMAND_ERROR]=
	    (wchar_t*)gettext("Error : \"%s\" is not recognized as an internal or"
	            " external command, an operable program or a batch file.\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_UNEXPECTED_ELEMENT]=
	    (wchar_t*)gettext("Error : \"%s\" was unexpected.\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_BAD_COMMAND_LINE]=
	    (wchar_t*)gettext("Error : Invalid command line.\n");

	lpErrorMsg[DOS9_LABEL_ERROR]=
	    (wchar_t*)gettext("Error : Unable to find label \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_EXTENSION_DISABLED_ERROR]=
	    (wchar_t*)gettext("Warning : Using a Dos9 extension but the CMDLYCORRECT option is set.\n");

	lpErrorMsg[DOS9_EXPECTED_MORE]=
	    (wchar_t*)gettext("Error : \"%s\" expected more arguments.\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_INCOMPATIBLE_ARGS]=
	    (wchar_t*)gettext("Error : Uncompatible arguments %s.\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_UNABLE_RENAME]=
	    (wchar_t*)gettext("Error : Unable to rename \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_MATH_OUT_OF_RANGE]=
	    (wchar_t*)gettext("Error : Overflowed maximum value.\n");

	lpErrorMsg[DOS9_MATH_DIVIDE_BY_0]=
	    (wchar_t*)gettext("Error : Divide by 0.\n");

	lpErrorMsg[DOS9_MKDIR_ERROR]=
	    (wchar_t*)gettext("Error : Unable to create folder \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_RMDIR_ERROR]=
	    (wchar_t*)gettext("Error : Unable to delete folder \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_STREAM_MODULE_ERROR]=
	    (wchar_t*)gettext("Error : Stream module : \"%s\".\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_SPECIAL_VAR_NON_ASCII]=
	    (wchar_t*)gettext("Error : \"%c\" is an invalid character for special variables. "
	            "Special variables require their name to be strict ascii "
	            "characters, exluding controls characters and space "
	            "(0x00 - 0x30).\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpErrorMsg[DOS9_ARGUMENT_NOT_BLOCK]=
	    (wchar_t*)gettext("Error : \"%s\" is not a valid block (should at least be "
	            "enclosed within parenthesis).\n");

	lpErrorMsg[DOS9_FOR_BAD_TOKEN_SPECIFIER]=
	    (wchar_t*)gettext("Error : \"%s\" is not a valid token specifier.\n");

	lpErrorMsg[DOS9_FOR_TOKEN_OVERFLOW]=
	    (wchar_t*)gettext("Error : Attempted to specify more than "
	            "TOKEN_NB_MAX (%d) different tokens\n.");

	lpErrorMsg[DOS9_FOR_USEBACKQ_VIOLATION]=
	    (wchar_t*)gettext("Error : \"%s\" violates the ``Usebackq'' syntax.\n");

	lpErrorMsg[DOS9_FAILED_ALLOCATION]=
	    (wchar_t*)gettext("Error : Unable to allocate memory (in %s).\n");

	lpErrorMsg[DOS9_CREATE_PIPE]=
	    (wchar_t*)gettext("Error : Can't create pipe (in %s).\n");

	lpErrorMsg[DOS9_FOR_LAUNCH_ERROR]=
	    (wchar_t*)gettext("Error : unable to run a subprocess of Dos9 to process "
	            "the given input (\"%s\").\n");

	lpErrorMsg[DOS9_FOR_BAD_INPUT_SPECIFIER]=
	    (wchar_t*)gettext("Error : Invalid token specifier. \"%s\" breaks input "
	            "specifying rules.\n");

	lpErrorMsg[DOS9_FOR_TRY_REASSIGN_VAR]=
	    (wchar_t*)gettext("Error : FOR loop is trying to reassign already used %%%%%c "
	            "variable.\n");

	lpErrorMsg[DOS9_INVALID_EXPRESSION]=
	    (wchar_t*)gettext("Error : \"%s\" is not a valid mathematical expression.\n");

	lpErrorMsg[DOS9_INVALID_TOP_BLOCK]=
	    (wchar_t*)gettext("Error : \"%s\" is not a valid top-level block.\n");

	lpErrorMsg[DOS9_UNABLE_DUPLICATE_FD]=
	    (wchar_t*)gettext("Error : Unable to duplicate file descriptor (%d).\n");

	lpErrorMsg[DOS9_UNABLE_CREATE_PIPE]=
	    (wchar_t*)gettext("Error : Unable to create pipe at function (%s).\n");

	lpErrorMsg[DOS9_UNABLE_SET_ENVIRONMENT]=
	    (wchar_t*)gettext("Error : Unable to set environment string (%s).\n");

	lpErrorMsg[DOS9_INVALID_REDIRECTION]=
	    (wchar_t*)gettext("Error : \"%s\" is an invalid stream redirection.\n");

	lpErrorMsg[DOS9_ALREADY_REDIRECTED]=
	    (wchar_t*)gettext("Error : Redirection for \"%s\" was already specified.\n");

	lpErrorMsg[DOS9_MALFORMED_BLOCKS]=
	    (wchar_t*)gettext("Error : Encountered malformed blocks (\"%s\").\n");

	lpErrorMsg[DOS9_NONCLOSED_BLOCKS]=
	    (wchar_t*)gettext("Error : Blocks left unclosed at end-of-file.\n");

	lpErrorMsg[DOS9_UNABLE_ADD_COMMAND]=
	    (wchar_t*)gettext("Error : Unable to add command \"%s\".\n");

	lpErrorMsg[DOS9_UNABLE_REMAP_COMMANDS]=
	    (wchar_t*)gettext("Error : Unable to remap commands (in function %s).\n");

	lpErrorMsg[DOS9_TRY_REDEFINE_COMMAND]=
		(wchar_t*)gettext("Error : Trying to redefine already used \"%s\" command.\n");

	lpErrorMsg[DOS9_UNABLE_REPLACE_COMMAND]=
		(wchar_t*)gettext("Error : Unable to redefine \"%s\" command.\n");

	lpErrorMsg[DOS9_UNABLE_GET_VARIABLE]=
		(wchar_t*)gettext("Error : Unable to get \"%s\" variable.\n");

	lpErrorMsg[DOS9_FAILED_CONVERSION]=
		(wchar_t*)gettext("Error : Unable to convert \"%s\" to multibyte string\n");

	lpQuitMessage=
	    (wchar_t*)gettext("\nAborting current command, press any key to end Dos9.\n");

}

void Dos9_ShowErrorMessage(unsigned int iErrorNumber,
                           const wchar_t* lpComplement,
                           int iExitCode)
{

	Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IRED);

	if ((iErrorNumber & ~DOS9_PRINT_C_ERROR) < sizeof(lpErrorMsg))
		fwprintf(stderr,
		        lpErrorMsg[iErrorNumber & ~DOS9_PRINT_C_ERROR],
		        lpComplement
		       );

	if (iErrorNumber & DOS9_PRINT_C_ERROR) {

		perror("");

	}

	if (iExitCode) {

		Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);

		if (pErrorHandler)
			pErrorHandler();

		putws(lpQuitMessage);

		exit(iExitCode);

	} else {

		Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);

	}

}
