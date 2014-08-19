/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 Romain Garbi (DarkBatcher)
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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "libDos9.h"

#include "Dos9_Core.h"

#include "../errors/Dos9_Errors.h"

int Dos9_TestLocalVarName(char cVar)
{
	if ((cVar & 0x80) || (cVar <= 0x20)) {

		/* if the name is not strict ASCII, it
		   is not conformant */

		if (cVar && cVar!='\n' && cVar!='\t' && cVar!=' ')
			Dos9_ShowErrorMessage(DOS9_SPECIAL_VAR_NON_ASCII,
			                      (const char*)((int)cVar),
			                      FALSE);

		return -1;
	}

	return 0;
}



char* Dos9_GetLocalVarPointer(LOCAL_VAR_BLOCK* lpvBlock, char cVarName)
{
	if (Dos9_TestLocalVarName(cVarName))
		return NULL;

	return lpvBlock[(int)cVarName];

}

int Dos9_SetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char cVarName, char* cVarContent)
{

	/* Perform test on value cName, to test its
	   specification conformance, i.e. the character must be
	   a strict ASCII character, exculuding control characters
	   and space (code range from 0x00 to 0x20 included) */

	if (Dos9_TestLocalVarName(cVarName))
		return FALSE;

	/* Free the current content of the variable if it is
	   already allocated */

	if (lpvBlock[(int)cVarName])
		free(lpvBlock[(int)cVarName]);

	if (cVarContent) {
		lpvBlock[(int)cVarName]=strdup(cVarContent);
	} else {
		lpvBlock[(int)cVarName]=NULL;
	}


	return TRUE;
}

char* Dos9_GetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char* lpName, ESTR* lpRecieve)
{
	char *lpPos, *lpNext;
	char lpDrive[_MAX_DRIVE], lpDir[_MAX_DIR], lpFileName[_MAX_FNAME], lpExt[_MAX_EXT];
	char cFlag[DOS9_VAR_MAX_OPTION+1]= {DOS9_ALL_PATH};
	char lpBuffer[FILENAME_MAX];
	char bSeekFile=FALSE, bSplitPath=FALSE;
	char cVarName,
	     cValidName=TRUE;

	struct tm* lTime;
	struct stat stFileInfo;

	int i=0;

	Dos9_EsCpy(lpRecieve, "");


	if (*lpName!='~') {

		/* this is a conventionnal special variable */

		if (Dos9_TestLocalVarName(*lpName))
			return NULL;

		if (!lpvBlock[(int)*lpName])
			return NULL;

		Dos9_EsCpy(lpRecieve, lpvBlock[(int)*lpName]);

		return lpName+1;
	}

	lpName++;

	/* this is an extended special variable */

	if (!*lpName) return NULL;

	for (; *(lpName) && strchr("dnpxzta", *(lpName)) && i<DOS9_VAR_MAX_OPTION; lpName++) {

		switch(*lpName) {
		case 'd':

			cFlag[i]='d';
			i++;
			bSplitPath=TRUE;
			break;

		case 'n':

			cFlag[i]='n';
			i++;
			bSplitPath=TRUE;
			break;

		case 'p':

			cFlag[i]='p';
			i++;
			bSplitPath=TRUE;
			break;

		case 'x':

			cFlag[i]='x';
			i++;
			bSplitPath=TRUE;
			break;

		case 'z':

			cFlag[i]='z';
			i++;
			bSeekFile=TRUE;
			break;

		case 't':

			cFlag[i]='t';
			i++;
			bSeekFile=TRUE;
			break;

		case 'a':

			cFlag[i]='a';
			i++;
			bSeekFile=TRUE;
			break;

		}
	}




	if ((*lpName & 0x80) || (*lpName <= 0x20)) {

		/* the varname is not valid */
		cValidName=FALSE;

	} else if (!lpvBlock[(int)*lpName]) {

		cValidName=FALSE;

	}

	if (!cValidName) {

		cVarName=0;

		/* if not, make a descending test */
		while (i>0) {

			i--;
			lpName--;

			if (lpvBlock[(int)cFlag[i]]) {
				/* the flag are all valid varnames */

				cVarName=cFlag[i];
				break;

			}

		}

		if (cVarName==0)
			return NULL;

	} else {

		/* if the var is defined */
		cVarName=*lpName;

	}

	Dos9_EsCpy(lpRecieve, lpvBlock[(int)cVarName]);
	lpPos=Dos9_EsToChar(lpRecieve);

	if (*lpPos=='"' || *lpPos=='\'') {

		if ((lpNext=strrchr(lpPos, *lpPos)))
			*lpNext='\0';

		while (*(lpPos+1)) {

			*(lpPos)=*(lpPos+1);
			lpPos++;

		}

		*lpPos='\0';

	}



	if (bSeekFile) {
		stat(lpPos, &stFileInfo);

#if defined WIN32
		stFileInfo.st_mode=GetFileAttributes(lpPos);
#endif
	}

	if (bSplitPath) {

		Dos9_SplitPath(lpPos, lpDrive, lpDir, lpFileName, lpExt);

		/* printf("lpDrive=`%s`\n"
		       "lpDir=`%s`\n"
		       "lpFileName=`%s`\n"
		       "lpExt=`%s`\n",
		       lpDrive,
		       lpDir,
		       lpFileName,
		       lpExt
		       ); */

	}

	if (cFlag[0]!=DOS9_ALL_PATH) {
		Dos9_EsCpy(lpRecieve, "");
		for (i=0; cFlag[i]!=0; i++) {
			switch (cFlag[i]) {

			case 'd':
				Dos9_EsCat(lpRecieve, lpDrive);

				if (cFlag[i+1]!=0 && cFlag[i+1]!='p')
					Dos9_EsCat(lpRecieve, "\t");

				break;

			case 'p':
				Dos9_EsCat(lpRecieve, lpDir);

				if (cFlag[i+1]!=0 && cFlag[i+1]!='n')
					Dos9_EsCat(lpRecieve, "\t");

				break;

			case 'n':
				Dos9_EsCat(lpRecieve, lpFileName);

				if (cFlag[i+1]!=0 && cFlag[i+1]!='x')
					Dos9_EsCat(lpRecieve, "\t");

				break;

			case 'x':
				Dos9_EsCat(lpRecieve, lpExt);
				if (cFlag[i+1]!=0) Dos9_EsCat(lpRecieve, "\t");
				break;

			case 'z':
				sprintf(lpBuffer, "%d%c", (int)stFileInfo.st_size, (cFlag[i+1]!=0 ? '\t' : '\0'));
				Dos9_EsCat(lpRecieve, lpBuffer);
				break;

			case 't':
				lTime=localtime(&stFileInfo.st_atime);

				sprintf(lpBuffer,
				        "%02d/%02d/%02d %02d:%02d%c",
				        lTime->tm_mday ,
				        lTime->tm_mon+1,
				        1900+lTime->tm_year,
				        lTime->tm_hour,
				        lTime->tm_min,
				        (cFlag[i+1]!=0 ? '\t' : '\0')
				       );

				Dos9_EsCat(lpRecieve, lpBuffer);
				break;

			case 'a':
				sprintf(lpBuffer, "       %c", (cFlag[i+1]!=0 ? '\t': '\0'));
				if (stFileInfo.st_mode & DOS9_FILE_DIR) lpBuffer[0]='D';
				if (stFileInfo.st_mode & DOS9_FILE_READONLY) lpBuffer[2]='R';
				if (stFileInfo.st_mode & DOS9_FILE_HIDDEN) lpBuffer[3]='H';
				if (stFileInfo.st_mode & DOS9_FILE_SYSTEM) lpBuffer[4]='S';
				if (stFileInfo.st_mode & DOS9_FILE_ARCHIVE) lpBuffer[5]='A';
				Dos9_EsCat(lpRecieve, lpBuffer);
				break;
			}
		}
	}

	return lpName+1;
}
