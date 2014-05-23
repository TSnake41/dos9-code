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
#include <wchar.h>

#include "Dos9_Lang.h"

#include <libintl.h>
#include <libDos9.h>

#include <libw.h>

const wchar_t* lpIntroduction;

const wchar_t* lpMsgEchoOn;
const wchar_t* lpMsgEchoOff;
const wchar_t* lpMsgPause;

const wchar_t* lpHlpMain;

const wchar_t* lpDirNoFileFound;
const wchar_t* lpDirListTitle;
const wchar_t* lpDirFileDirNb;

const wchar_t* lpHlpDeprecated;

const wchar_t* lpDelConfirm;
const wchar_t* lpDelChoices;


const wchar_t* lpAskYn;
const wchar_t* lpAskyN;
const wchar_t* lpAskyn;

const wchar_t* lpAskYna;
const wchar_t* lpAskyNa;
const wchar_t* lpAskynA;
const wchar_t* lpAskyna;

const wchar_t* lpAskYes;
const wchar_t* lpAskYesA;

const wchar_t* lpAskNo;
const wchar_t* lpAskNoA;

const wchar_t* lpAskAll;
const wchar_t* lpAskAllA;

const wchar_t* lpAskInvalid;

void Dos9_LoadStrings(void)
{
	/* this loads strings */
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
		bindtextdomain("Dos9-msg", lpPath);
		free(lpPath);

	} else {

		bindtextdomain("Dos9-msg", "/share/locale");

	}

	bind_textdomain_codeset("Dos9-msg", lpEncoding);
	textdomain("Dos9-msg");

	/* texte des commandes ECHO et PAUSE */
	lpMsgEchoOn=(wchar_t*)gettext("Echo command enabled");
	lpMsgEchoOff=(wchar_t*)gettext("Echo command disabled");
	lpMsgPause=(wchar_t*)gettext("Press any key to continue...");

	/* texte de la commande DIR */
	lpDirNoFileFound=(wchar_t*)gettext("\tNo files found\n");
	lpDirListTitle=(wchar_t*)gettext("\nLast change\t\tSize\tAttr.\tName\n");
	lpDirFileDirNb=(wchar_t*)gettext("\t\t\t\t\t\t%d Files\n\t\t\t\t\t\t%d Folders\n");
	// TRANSLATORS : Don't remove the %s because the program needs it

	lpDelConfirm=(wchar_t*)gettext("Are you sure you want to delete \"%s\" ?");

	lpAskYn=(wchar_t*)gettext(" (Yes/no) ");
	lpAskyN=(wchar_t*)gettext(" (yes/No) ");
	lpAskyn=(wchar_t*)gettext(" (yes/no) ");

	lpAskYna=(wchar_t*)gettext(" (Yes/no/all) ");
	lpAskyNa=(wchar_t*)gettext(" (yes/No/all) ");
	lpAskynA=(wchar_t*)gettext(" (yes/no/All) ");
	lpAskyna=(wchar_t*)gettext(" (yes/no/all) ");

	lpAskYes=(wchar_t*)gettext("YES");
	lpAskYesA=(wchar_t*)gettext("Y");

	lpAskNo=(wchar_t*)gettext("NO");
	lpAskNoA=(wchar_t*)gettext("N");

	lpAskAll=(wchar_t*)gettext("ALL");
	lpAskAllA=(wchar_t*)gettext("A");

	lpAskInvalid=(wchar_t*)gettext("Please enter a correct choice (or type enter to choose default) !\n");

	lpHlpMain=(wchar_t*)gettext("DOS9 COMMAND\n\
\n\
        DOS9 is a free, cross-platform command prompt used for batch scripts \n\
     and command processing.\n\
\n\
        DOS9 is a free software, designed to be as compatible as possible with\n\
     CMD.EXE proprietary software from MICROSOFT.\n\
\n\
SYNOPSIS\n\
\n\
   DOS9 [/v] [/n] [/f] [/e] [file]\n\
\n\
        Run a command file or wait for an user input.\n\
\n\
        - FILE : Path of the batch script to be run.\n\
\n\
        - /V : Enables delayed expansion (see `spec/xvar').\n\
\n\
        - /N : Enables dos9's extension (see `spec/ext').\n\
\n\
        - /F : Enables floating numbers (see `spec/exp').\n\
\n\
        - /E : Disable current directory echoing.\n\
\n\
COMPATIBILITY\n\
\n\
        Compatible with any version of DOS9. Incompatible with CMD.EXE.\n\
\n\
        Dos9 is cross-platform so that it can be run either on MS-WINDOWS or \n\
     GNU/LINUX. Theorically, a port of Dos9 should be possible in any \n\
     POSIX-compatible system just by recompiling sources using a C89 compiler.\n\
\n\
LICENSE\n\
\n\
        DOS9 is a free software distributed under GNU General Public License \n\
     (see `http://www.gnu.org/licenses/gpl.html') terms. For more \n\
     informations about free software's philosophy, see Free Software \n\
     Fundation's (see `http://www.fsf.org') website.\n\
\n\
AUTHOR\n\
\n\
        DOS9 have been written since 2010 by DARKBATCHER (ROMAIN GARBI). It \n\
     uses severals helpful free libraries from the GNU operating system (see \n\
     `http://www.gnu.org/'), mostly libmatheval (see \n\
     `http://www.gnu.org/software/libmatheval/'), gettext (see \n\
     `https://www.gnu.org/software/gettext/').\n\
\n\
        Dos9's windows packages also distribute some binaries from the GnuWin \n\
     project (see `http://gnuwin32.sourceforge.net/'). These binaries are GNU \n\
     iconv (see `iconv') and GNU nano (see `nano').\n\
\n\
SEE ALSO\n\
\n\
        ECHO Command (see `echo'), Command scripts (see `script'), Commands \n\
     list (see `commands') Commands list (see `commands')\n");

}
