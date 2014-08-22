#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>

#include "../../config.h"
#include "../gettext.h"

#include "Dos9_ShowHelp.h"

/* the main purpose of this functions is to provide little
   help embedded inside the binary, so that user can use
   this little help, even if they have no 'HLP' no
   documentation installed right there with their Dos9
   installation */

static const char* lpInternalHelp[DOS9_HELP_ARRAY_SIZE];
static const char* lpExternalMsg;

void Dos9_LoadInternalHelp(void)
{
	char lpPath[FILENAME_MAX];
	char lpEncoding[15];
	char lpSharePath[FILENAME_MAX];

	Dos9_GetExePath(lpPath, FILENAME_MAX);
	Dos9_GetConsoleEncoding(lpEncoding, sizeof(lpEncoding));

	snprintf(lpSharePath, FILENAME_MAX, "%s/share/locale", lpPath);

	bindtextdomain("Dos9-hlp", lpSharePath);
	bind_textdomain_codeset("Dos9-hlp", lpEncoding);
	textdomain("Dos9-hlp");

	lpInternalHelp[DOS9_HELP_ALIAS]
	    =gettext("Create an alias on a command.\n"
	             "Usage: ALIAS [/f] alias=command\n");

	lpInternalHelp[DOS9_HELP_CD]
	    =gettext("Change current working directory.\n"
	             "Usage: CD  [[/d] path]\n"
	             "       CHDIR [[/d] path]\n");

	lpInternalHelp[DOS9_HELP_CLS]
	    =gettext("Clear console's screen\n"
	             "Usage: CLS\n");

	lpInternalHelp[DOS9_HELP_COLOR]
	    =gettext("Change console's color to the given code.\n"
	             "Usage: COLOR [code]\n");

	lpInternalHelp[DOS9_HELP_COPY]
	    =gettext("Copy file(s) to another location.\n"
	             "Usage: COPY [/-Y | /Y] [/A[:]attributes] source destination\n");

	lpInternalHelp[DOS9_HELP_CALL]
	    =gettext("Call a script or a label from the current context.\n"
	             "Usage: CALL [/e] [file] [:label] [parameters ...]\n");


	lpInternalHelp[DOS9_HELP_DEL]
	    =gettext("Delete file(s).\n"
	             "Usage: DEL [/P] [/F] [/S] [/Q] [/A[:]attributes] name\n"
	             "       ERASE [/P] [/F] [/S] [/Q] [/A[:]attributes] name\n");

	lpInternalHelp[DOS9_HELP_DIR]
	    =gettext("List files in a directory and in subdirectories.\n"
	             "Usage: DIR [/A[:]attributes] [/S] [/B] [path]\n");

	lpInternalHelp[DOS9_HELP_ECHO]
	    =gettext("Print a message, or enable/disable commande echoing.\n"
	             "Usage: ECHO [OFF|ON]\n"
	             "       ECHO text\n"
	             "       ECHO.text\n");

	lpInternalHelp[DOS9_HELP_FOR]
	    =gettext("Perform commands against items (files, strings, command output).\n"
	             "Usage: FOR %%A IN (string) DO (\n"
	             "          :: some commands\n"
	             "       )\n\n"
	             "       FOR /F [options] %%A IN (object) DO (\n"
	             "          :: some commands\n"
	             "       )\n\n"
	             "       FOR /L %%A IN (start,increment,end) DO (\n"
	             "          :: some commands\n"
	             "       )\n");

	lpInternalHelp[DOS9_HELP_GOTO]
	    =gettext("Go to a label in a command script.\n"
	             "Usage: GOTO [:]label [file]\n");

	lpInternalHelp[DOS9_HELP_IF]
	    =gettext("Perform commands on some conditions.\n"
	             "Usage: IF [/i] [NOT] string1==string2 (\n"
	             "          :: code to be ran\n"
	             "       )\n\n"
	             "       IF [/i] string1 cmp string2\n"
	             "          :: code to be ran\n"
	             "       )\n\n"
	             "       IF [NOT] [DEFINED | EXIST | ERRORLEVEL] object (\n"
	             "          :: code to be ran\n"
	             "       )\n");

	lpInternalHelp[DOS9_HELP_MD]
	    =gettext("Create the given directory.\n"
	             "Usage: MD directory\n"
	             "       MKDIR directory\n");


	lpInternalHelp[DOS9_HELP_PAUSE]
	    =gettext("Wait for a keystroke from the user.\n"
	             "Usage: PAUSE\n");


	lpInternalHelp[DOS9_HELP_RD]
	    =gettext("Removes a directory.\n"
	             "Usage: RMDIR [/S] [/Q] directory\n"
	             "       RD [/S] [/Q] directory\n");

	lpInternalHelp[DOS9_HELP_REN]
	    =gettext("Renames a file.\n"
	             "Usage: REN file name.ext\n"
	             "       RENAME file name.ext\n");

	lpInternalHelp[DOS9_HELP_REN]
	    =gettext("Introduce a comment.\n"
	             "Usage: REM comment\n");


	lpInternalHelp[DOS9_HELP_SET]
	    =gettext("Set an environment variable.\n"
	             "Usage: SET variable=content\n"
	             "       SET /a[[:][i|f]] variable=expression\n"
	             "       SET /p variable=question\n");

	lpInternalHelp[DOS9_HELP_SETLOCAL]
	    =gettext("Set options of the Dos9 command prompt.\n"
	             "Usage: SETLOCAL [ENABLEDELAYEDEXPANSION | DISABLEDELAYEDEXPANSION] [ENABLEFLOATS | DISABLEFLOATS] [CMDLYCORRECT | CMDLYINCORRECT]\n");

	lpInternalHelp[DOS9_HELP_TITLE]
	    =gettext("Set console's title.\n"
	             "Usage: TITLE title\n");

	lpInternalHelp[DOS9_HELP_TYPE]
	    =gettext("Print a file.\n"
	             "Usage: TYPE [file]\n"
	             "       MORE [file]\n");

	lpInternalHelp[DOS9_HELP_EXIT]
	    =gettext("Exit from the command prompt.\n"
	             "Usage: EXIT [/b code]\n");

	lpInternalHelp[DOS9_HELP_SHIFT]
	    =gettext("Change positions of command-line parameters.\n"
	             "Usage: SHIFT [/start_number | /s[:]start_number] [/d[:]displacement]\n");

	lpExternalMsg
	    =gettext("This help is voluntary limited help to fit in the binary. If you need to\n"
	             "see the full documentation, please type:\n"
	             "\n"
	             "\tHLP command");

}


void Dos9_ShowInternalHelp(DOS9CONTEXT* pContext, int cmdId)
{

	if ((cmdId >= 0)
	    && (cmdId < DOS9_HELP_ARRAY_SIZE)) {

		fprintf(pContext->pStack->out,"%s\n%s\n",
                lpInternalHelp[cmdId],
                lpExternalMsg
        );

	}

}
