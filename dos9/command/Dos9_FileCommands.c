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
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <wchar.h>

#include <libDos9.h>

#include <libw.h>

#ifdef _POSIX_C_SOURCE

#define Dos9_Mkdir(name, mode) wmkdir(name, mode)

#elif defined WIN32

#define Dos9_Mkdir(name, mode) wmkdir(name)

#endif /* defined linux */

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_Help.h"
#include "../errors/Dos9_Errors.h"
#include "../core/Dos9_Core.h"
#include "Dos9_FileCommands.h"

#include "Dos9_Ask.h"

int Dos9_CmdDel(wchar_t* lpLine)
{
	wchar_t *lpNextToken,
			*lpToken;
	ESTR	*lpEstr=Dos9_EsInit();
	char  	bParam=0,
	      	bDel=TRUE;

	short	wAttr=0;

	wchar_t lpName[FILENAME_MAX]=L"\0";

	int iFlag=DOS9_SEARCH_DEFAULT,
	    iChoice;

	FILELIST* lpFileList, *lpSaveList;

	if (!(lpLine=wcschr(lpLine, L' '))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"DEL / ERASE", FALSE);
		Dos9_EsFree(lpEstr);
		return -1;

	}

	while ((lpNextToken=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		lpToken=Dos9_EsToChar(lpEstr);

		if (!wcscasecmp(lpToken, L"/P")) {

			/* Demande une confirmation avant la suppression */

			bParam|=DOS9_ASK_CONFIRMATION;

		} else if (!wcscasecmp(lpToken, L"/F")) {

			/* supprime les fichiers en lecture seule */

			bParam|=DOS9_DELETE_READONLY;

		} else if (!wcscasecmp(lpToken, L"/S")) {

			/* procède récursivement */

			iFlag|=DOS9_SEARCH_RECURSIVE;

		} else if (!wcscasecmp(lpToken, L"/Q")) {

			/* pas de confirmation pour suppression avec caractères génériques */

			bParam|=DOS9_DONT_ASK_GENERIC;

		} else if (!wcscasecmp(lpToken, L"/A", 2)) {

			/* gestion des attributs */
			lpToken+=2;
			if (*lpToken==L':') lpToken++;

			wAttr=Dos9_MakeFileAttributes(lpToken);

			iFlag^=DOS9_SEARCH_NO_STAT;

		} else if (!wcscmp(L"/?", lpToken)) {

			Dos9_ShowInternalHelp(DOS9_HELP_DEL);

			Dos9_EsFree(lpEstr);
			return -1;

		} else {

			if (*lpName != L'\0') {

				/* si un nom a été donné, on affiche, l'erreur */

				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
				Dos9_EsFree(lpEstr);
				return -1;

			}

			wcsncpy(lpName, lpToken, FILENAME_MAX);
		}

		lpLine=lpNextToken;

	}

	if (*lpName== L'\0') {

		/* si aucun nom n'a été donné */

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"DEL / ERASE", FALSE);
		Dos9_EsFree(lpEstr);
		return -1;

	}

	if (!(bParam & DOS9_DELETE_READONLY)) {

		/* par défaut, la recherche ne prend pas en compte la recherche des fichiers en lecture seule */
		wAttr|=DOS9_CMD_ATTR_READONLY_N | DOS9_CMD_ATTR_READONLY;

	}

	/* ne sélectionne que le fichiers */
	wAttr|=DOS9_CMD_ATTR_DIR | DOS9_CMD_ATTR_DIR_N;

	if ((Dos9_StrToken(lpName, L'*') || Dos9_StrToken(lpName, L'?'))
	    && !(bParam & DOS9_DONT_ASK_GENERIC)) {

		/* si la recherche est générique on met la demande de
		  confirmation sauf si `/Q` a été spécifié */
		bParam|=DOS9_ASK_CONFIRMATION;
	}

	wprintf(L"Looking for `%s'\n", lpName);

	if ((lpFileList=Dos9_GetMatchFileList(lpName, iFlag))) {
		lpSaveList=lpFileList;

		while (lpFileList) {

			/* on demande confirmation si la demande de confirmation est
			   active */
			if (bParam & DOS9_ASK_CONFIRMATION) {

				iChoice=Dos9_AskConfirmation(DOS9_ASK_YNA
				                             | DOS9_ASK_INVALID_REASK
				                             | DOS9_ASK_DEFAULT_Y,
				                             lpDelConfirm,
				                             lpFileList->lpFileName
				                            );

				if (iChoice==DOS9_ASK_NO) {
					/* si l'utilisateur refuse la suppression du fichier */
					wprintf(L"Pas suppression !\n");
					lpFileList=lpFileList->lpflNext;
					continue;

				} else if (iChoice==DOS9_ASK_ALL) {
					/* si l'utilisateur autorise tous les fichiers */
					bParam&=~DOS9_ASK_CONFIRMATION;

				}
			}

			/* on vérifie que le fichier possède les bons attributs pour la suppression */
			if ((Dos9_CheckFileAttributes(wAttr, lpFileList))) {

				wprintf(L"<DEBUG> supression de `%s'\n", lpFileList->lpFileName);

			} else {

				wprintf(L"<DEBUG> Fichier `%s' non suprimmé (attributs incorrects)\n", lpFileList->lpFileName);

			}

			/* on passe au fichier suivant */
			lpFileList=lpFileList->lpflNext;
		}

		Dos9_FreeFileList(lpSaveList);

	} else {

		Dos9_ShowErrorMessage(DOS9_FILE_ERROR, lpName, FALSE);
		Dos9_EsFree(lpEstr);

		return -1;

	}

	Dos9_EsFree(lpEstr);
	return 0;
}

int iDirNb,
    iFileNb;
short wAttr;
char bSimple;

void Dos9_CmdDirShow(FILELIST* lpElement)
{
	wchar_t lpType[]=L"D RHSA ",
			lpSize[16];

	struct tm* lTime;

	/* This structures get only one argument at a time,
	   thus ``lpElement->lpNext'' is inconsistent */

	if (Dos9_CheckFileAttributes(wAttr, lpElement)) {

		/* if the file has right attributes */

		if (!bSimple) {

			wcscpy(lpType, L"       ");

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_DIR) {

				lpType[0]=L'D';
				iDirNb++;

			} else {

				iFileNb++;

			}

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_READONLY)
				lpType[2]=L'R';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_HIDDEN)
				lpType[3]=L'H';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_SYSTEM)
				lpType[4]=L'S';

			if (Dos9_GetFileMode(lpElement) & DOS9_FILE_ARCHIVE)
				lpType[5]=L'A';

			/* !! Recyclage de la variable lpFilename pour afficher la taille du fichier */
			Dos9_FormatFileSize(lpSize, 16, Dos9_GetFileSize(lpElement));

			lTime=localtime(&Dos9_GetModifTime(lpElement));

			wprintf(L"%02d/%02d/%02d %02d:%02d\t%s\t%s\t%s\n",lTime->tm_mday ,
																lTime->tm_mon+1,
																1900+lTime->tm_year,
																lTime->tm_hour,
																lTime->tm_min,
																lpSize,
																lpType,
																lpElement->lpFileName
																);

		} else {

			wprintf(L"%s\n", lpElement->lpFileName);

		}
	}
}

int Dos9_CmdDir(wchar_t* lpLine)
{
	wchar_t *lpNext,
			*lpToken,
	         lpFileName[FILENAME_MAX]= {0};

	int iFlag=DOS9_SEARCH_DEFAULT | DOS9_SEARCH_DIR_MODE;

	ESTR* lpParam=Dos9_EsInit();

	lpNext=lpLine+3;

	wAttr=DOS9_CMD_ATTR_ALL;
	bSimple=FALSE;

	while ((lpNext=Dos9_GetNextParameterEs(lpNext, lpParam))) {

		lpToken=Dos9_EsToChar(lpParam);

		if (!wcscmp(lpToken, L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_DIR);
			return 0;

		} else if (!wcscasecmp(L"/b", lpToken)) {

			/* use the simple dir output */
			bSimple=TRUE;
			if (!wAttr) iFlag|=DOS9_SEARCH_NO_STAT;
			iFlag|=DOS9_SEARCH_NO_PSEUDO_DIR;

		} else if (!wcscasecmp(L"/s", lpToken)) {

			/* set the command to recusive */
			iFlag|=DOS9_SEARCH_RECURSIVE;

		} else if (!strnicmp(L"/a", lpToken,2)) {

			/* uses the attributes command */
			lpToken+=2;
			if (*lpToken==L':')
				lpToken++;

			wAttr=Dos9_MakeFileAttributes(lpToken);
			iFlag&=~DOS9_SEARCH_NO_STAT;

		} else {

			if (*lpFileName) {

				Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, lpToken, FALSE);
				Dos9_EsFree(lpParam);

				return -1;
			}

			wcsncpy(lpFileName, lpToken, FILENAME_MAX);

		}
	}

	if (!*lpFileName) {
		/* if no file or directory name have been specified
		   the put a correct value on it */

		*lpFileName=L'*';
		lpFileName[1]=L'\0';

	}

	/* do a little global variable setup before
	   starting file research */
	iDirNb=0;
	iFileNb=0;

	if (!bSimple) puts(lpDirListTitle);

	/* Get a list of file and directories matching to the
	   current filename and options set */
	if (!(Dos9_GetMatchFileCallback(lpFileName, iFlag, Dos9_CmdDirShow))
	    && !bSimple)
		putws(lpDirNoFileFound);

	if (!bSimple) wprintf(lpDirFileDirNb, iFileNb, iDirNb);

	Dos9_EsFree(lpParam);

	return 0;
}


int Dos9_CmdMove(wchar_t* lpLine)
{
	return 0;
}

int Dos9_CmdCopy(wchar_t* lpLine)
{
	return 0;
}

int Dos9_CmdRen(wchar_t* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();
	wchar_t lpFileName[FILENAME_MAX]= {0},
			lpFileDest[FILENAME_MAX]= {0};
	wchar_t* lpToken;

	if (!(lpLine=wcschr(lpLine, L' '))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"DEL / ERASE", FALSE);
		Dos9_EsFree(lpEstr);
		return -1;

	}

	if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		wcsncpy(lpFileName, Dos9_EsToChar(lpEstr), FILENAME_MAX);
		lpFileName[FILENAME_MAX-1]=L'\0';
		/* can't assume that what was buffered is NULL-terminated
		   see the C-89,99,11 standards for further informations */

		wcscpy(lpFileDest, lpFileName);

		if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

			/* removing old filename */
			lpLine=wcsrchr(lpFileDest, L'\\');
			lpToken=wcsrchr(lpFileDest, L'/');

			if (lpToken>lpLine) {
				lpLine=lpToken;
			}

			if (lpLine) {
				lpLine++;
				*lpLine=L'\0';
			}

			/* cat with new name */
			wcsncat(lpFileDest, Dos9_EsToChar(lpEstr), FILENAME_MAX-strlen(lpFileDest));

			if (!wprintf("<DEBUG> renaming `%s` to `%s`\n", lpFileName, lpFileDest)) {

				Dos9_ShowErrorMessage(DOS9_UNABLE_RENAME, lpFileName, FALSE);
				Dos9_EsFree(lpEstr);
				return -1;

			}

			return 0;

		}

	}

	Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "REN / RENAME", FALSE);
	Dos9_EsFree(lpEstr);
	return -1;
}

int Dos9_CmdRmdir(wchar_t* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();

	if (!(lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "RD/RMDIR", FALSE);
		goto error;

	}

	if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		if (!wcscmp(Dos9_EsToChar(lpEstr), L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_RD);

		} else if (!wcscasecmp(Dos9_EsToChar(lpEstr), L"/Q")) {


		} else {

			if (wrmdir(Dos9_EsToChar(lpEstr))) {

				Dos9_ShowErrorMessage(DOS9_MKDIR_ERROR, Dos9_EsToChar(lpEstr), FALSE);
				goto error;

			}

		}

	}

	Dos9_EsFree(lpEstr);
	return 0;

error:
	Dos9_EsFree(lpEstr);
	return -1;
}

int Dos9_CmdMkdir(wchar_t* lpLine)
{
	ESTR* lpEstr=Dos9_EsInit();

	if (!(lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, L"MD/MKDIR", FALSE);
		goto error;

	}

	if ((lpLine=Dos9_GetNextParameterEs(lpLine, lpEstr))) {

		if (!wcscmp(Dos9_EsToChar(lpEstr), L"/?")) {

			Dos9_ShowInternalHelp(DOS9_HELP_MD);

		} else {

			/* this is not really good for *Nixes. The creation of
			   such a folder may be refused because of user's right.
			 */

			if (Dos9_Mkdir(Dos9_EsToChar(lpEstr), 0777)) {

				Dos9_ShowErrorMessage(DOS9_MKDIR_ERROR, Dos9_EsToChar(lpEstr), FALSE);
				goto error;

			}

		}

	}

	Dos9_EsFree(lpEstr);
	return 0;

error:
	Dos9_EsFree(lpEstr);
	return -1;

}
