/*
 *
 *   libDos9 - The Dos9 project
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

#include <wctype.h>

#include "../LibDos9.h"

#ifdef WIN32

typedef _WDIR WDIR;
#define dirent _wdirent
#define wopendir _wopendir
#define wreaddir  _wreaddir
#define wclosedir _wclosedir

#endif // WIN32

LIBDOS9 int Dos9_RegExpMatch(wchar_t* lpRegExp, wchar_t* lpMatch)
{
	wchar_t* lpNextPos;

	if (!(lpRegExp && lpMatch)) return FALSE;

	for (; *lpRegExp && *lpMatch;
	     lpRegExp++,
	     lpMatch++
	    ) {

		switch(*lpRegExp) {
		case L'?':

			break;

		case L'*':

			lpRegExp++;

			while (*lpRegExp==L'*' && *lpRegExp==L'?') lpRegExp++;

			if (!*lpRegExp) return TRUE;


			if ((lpNextPos=_Dos9_SeekPatterns(lpMatch,lpRegExp))) {

				lpMatch=lpNextPos; /* search if matching patterns exist*/

			} else {

				return FALSE;

			}

			if (!*lpMatch) return TRUE; /* if there is no match string to test, the match is true */

			if (*lpRegExp) while (*lpRegExp!=L'*' && *lpRegExp!=L'?' && *lpRegExp)
				lpRegExp++;
			/* skip the next symbols, i.e. two '**' or '*?' are useless and so ignored */
			lpRegExp--;

			break;

		default:
			if (*lpRegExp != *lpMatch) return FALSE;

		}
	}

	if (*lpRegExp) lpRegExp++;

	if (!(!*lpMatch && !*lpRegExp)) return FALSE;
	return TRUE;
}

LIBDOS9 int Dos9_RegExpCaseMatch(wchar_t* lpRegExp, wchar_t* lpMatch)
{
	wchar_t* lpNextPos;

	if (!(lpRegExp && lpMatch)) return FALSE;

	for (; *lpRegExp && *lpMatch;
			lpRegExp++,lpMatch++
	    ) {
		switch(*lpRegExp) {
		case L'?':

			break;

		case L'*':

			lpRegExp++;

			while (*lpRegExp==L'*' && *lpRegExp==L'?') lpRegExp++;

			if (!*lpRegExp) return TRUE;

			if ((lpNextPos=_Dos9_SeekCasePatterns(lpMatch,lpRegExp))) lpMatch=lpNextPos; /* search if matching patterns exist*/
			else return FALSE;

			if (!*lpMatch) return TRUE; /* if there is no match string to test, the match is true */

			if (*lpRegExp) while (*lpRegExp!=L'*' && *lpRegExp!=L'?' && *lpRegExp) lpRegExp++;
			/* skip the next symbols, i.e. two '**' or '*?' are useless and so ignored */
			lpRegExp--;
			break;

		default:
			if (toupper(*lpRegExp) != toupper(*lpMatch)) return FALSE;
		}
	}

	if (*lpRegExp) lpRegExp++;
	if (!(!*lpMatch && !*lpRegExp)) return FALSE;
	return TRUE;
}


LIBDOS9 int Dos9_FormatFileSize(wchar_t* lpBuf, int iLength, unsigned int iFileSize)
{
	int i=0, iLastPart=0;
	wchar_t* lpUnit[]= {L"o", L"ko", L"mo", L"go"};
	/* Note : We conform to SI prefixes. We could have used
	   O, Kio, Mio, Gio instead.*/


	if (!iFileSize) {
		return (int)wcsncpy(lpBuf, L"", iLength);
	}

	while (iFileSize>=1000) {
		iLastPart=iFileSize % 1000;
		iFileSize/=1000;
		i++;
	}

	if (iFileSize>=100)
		return snwprintf(lpBuf, iLength, L" %d %s", iFileSize, lpUnit[i%4]);

	if (iFileSize>=10)
		return snwprintf(lpBuf, iLength, L"  %d %s", iFileSize, lpUnit[i%4]);

	iLastPart=iLastPart/10;
	return snwprintf(lpBuf, iLength, L"%d,%.2d %s",
					 iFileSize, iLastPart , lpUnit[i%4]);

}

LIBDOS9 int         Dos9_GetMatchFileCallback(wchar_t* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*))
{
	int iDepth; /* to store depth of  regexp */
	wchar_t lpStaticPart[FILENAME_MAX],
			lpMatchPart[FILENAME_MAX];

	int iFileDescriptors[2];
	size_t iReturn;

	THREAD hThread;

	FILEPARAMETER fpParam= {TRUE, /* include informations on the
                                    FILEPARAMETER Structures returned */
	                        0,     /* defines the descriptor through which
                                    the callback funtion will read */

	                        pCallBack /* the callback routine in order to benefit from
                                        from the use of two thread. (this is somewhat
                                        like asynchronious version of the next function
                                     */
	                       };

	if (iFlag & DOS9_SEARCH_NO_STAT) {
		fpParam.bStat=FALSE;
	}

	_Dos9_SplitMatchPath(lpPathMatch, lpStaticPart, FILENAME_MAX, lpMatchPart, FILENAME_MAX);
	/* Split the string in two blocks :
	    - lpStaticPart : Part that does not include any
	      regexp symbol.

	    - lpMatchPart : Part that include regexp symbols

	*/

	iDepth=_Dos9_GetMatchDepth(lpMatchPart);

	/* Set pipe arguments */
	if (_Dos9_Pipe(iFileDescriptors, 1024, O_BINARY)==-1) return 0;
	fpParam.iInput=iFileDescriptors[0];

	/* start the thread that gather answers */
	Dos9_BeginThread(&hThread,
	                 (void(*)(void*))_Dos9_WaitForFileListCallBack,
	                 0,
	                 (void*)(&fpParam)
	                );

	if (!*lpStaticPart && *lpMatchPart) {

		/* add the current directory to the default
		   static path
		*/

		*lpStaticPart=L'.';
		lpStaticPart[1]=L'\0';

	}

	if (*lpStaticPart && !*lpMatchPart) {

		if (Dos9_DirExists(lpStaticPart) && (iFlag & DOS9_SEARCH_DIR_MODE)) {

			/* if the regexp is trivial but corresponds to a directory and
			   if the dir-compliant mode has been set, then the regexp will
			   browse the directory */

			*lpMatchPart=L'*';
			lpMatchPart[1]=L'\0';

		} else if (Dos9_DirExists(lpStaticPart) || Dos9_FileExists(lpStaticPart)) {

			/* if the regexp is trivial (ie. no regexp characters)
			   then perform direct test and add it if go to end */

			write(iFileDescriptors[1], lpStaticPart, FILENAME_MAX*sizeof(wchar_t));
			goto Dos9_GetMatchFileList_End;

		}

	}


	/* Start regexp-based file research */
	_Dos9_SeekFiles(lpStaticPart,
	                lpMatchPart,
	                1,
	                iDepth,
	                iFileDescriptors[1],
	                iFlag);

Dos9_GetMatchFileList_End:


	if (write(iFileDescriptors[1], L"\1", sizeof(wchar_t))==-1)
		return 0;

	Dos9_WaitForThread(&hThread, &iReturn);

	close(iFileDescriptors[0]);
	close(iFileDescriptors[1]);

	return iReturn;

}

LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(wchar_t* lpPathMatch, int iFlag)
{
	int iDepth; /* to store depth of  regexp */
	wchar_t lpStaticPart[FILENAME_MAX],
			lpMatchPart[FILENAME_MAX];

	int iFileDescriptors[2];

	THREAD hThread;
	LPFILELIST lpReturn;

	FILEPARAMETER fpParam= {TRUE, /* include informations on the
                                    FILEPARAMETER Structures returned */
	                        0,    /* defines the descriptor through which
                                    the callback funtion will read */
	                        NULL  /* no callback */
	                       };

	if (iFlag & DOS9_SEARCH_NO_STAT) {
		fpParam.bStat=FALSE;
	}

	_Dos9_SplitMatchPath(lpPathMatch, lpStaticPart, FILENAME_MAX, lpMatchPart, FILENAME_MAX);
	/* Split the string in two blocks :
	    - lpStaticPart : Part that does not include any
	      regexp symbol.

	    - lpMatchPart : Part that include regexp symbols

	*/

	iDepth=_Dos9_GetMatchDepth(lpMatchPart);

	/* Set pipe arguments */
	if (_Dos9_Pipe(iFileDescriptors, 1024, O_BINARY)==-1) return NULL;
	fpParam.iInput=iFileDescriptors[0];


	/* start the thread that gather answers */
	Dos9_BeginThread(&hThread,
	                 (void(*)(void*))_Dos9_WaitForFileList,
	                 0,
	                 (void*)(&fpParam)
	                );

	if (!*lpStaticPart && *lpMatchPart) {

		/* add the current directory to the default
		   static path
		*/

		*lpStaticPart=L'.';
		lpStaticPart[1]=L'\0';

	}

	if (*lpStaticPart && !*lpMatchPart) {

		if (Dos9_DirExists(lpStaticPart) && (iFlag & DOS9_SEARCH_DIR_MODE)) {

			/* if the regexp is trivial but corresponds to a directory and
			   if the dir-compliant mode has been set, then the regexp will
			   browse the directory */

			*lpMatchPart=L'*';
			lpMatchPart[1]=L'\0';

		} else if (Dos9_DirExists(lpStaticPart) || Dos9_FileExists(lpStaticPart)) {

			/* if the regexp is trivial (ie. no regexp characters)
			   then perform direct test and add it if go to end */

			write(iFileDescriptors[1], lpStaticPart, FILENAME_MAX*sizeof(wchar_t));
			goto Dos9_GetMatchFileList_End;

		}

	}

	/* Start regexp-based file research */
	_Dos9_SeekFiles(lpStaticPart,
	                lpMatchPart,
	                1,
	                iDepth,
	                iFileDescriptors[1],
	                iFlag);

Dos9_GetMatchFileList_End:

	if (write(iFileDescriptors[1], L"\1", sizeof(wchar_t))==-1) return NULL;

	Dos9_WaitForThread(&hThread, &lpReturn);

	close(iFileDescriptors[0]);
	close(iFileDescriptors[1]);

	return lpReturn;

}

LPFILELIST _Dos9_SeekFiles(wchar_t* lpDir, wchar_t* lpRegExp, int iLvl, int iMaxLvl, int iOutDescriptor, int iSearchFlag)
{
	WDIR* pDir;
	/* used to browse the directory */
	struct dirent* lpDirElement;
	/* used to browse the directory elements */
	wchar_t lpRegExpLvl[FILENAME_MAX];
	/* used to get the part of the path to deal with */
	wchar_t lpFilePath[FILENAME_MAX];
	/* used to make file path */
	int iFlagRecursive=FALSE,
	    iIsPseudoDir;

	if (iSearchFlag & DOS9_SEARCH_RECURSIVE) {
		iFlagRecursive=(iLvl == iMaxLvl);
		/*  verfify wether the research on the path is ended (i.e. the top level at least been reached) */
	}

	_Dos9_GetMatchPart(lpRegExp, lpRegExpLvl, FILENAME_MAX, iLvl);
	/* returns the part of path that must the file must match */

	if ((!wcscmp(lpRegExpLvl, L".")) && (iLvl == iMaxLvl)) {

		*lpRegExpLvl=L'\0';
		/*
		    If the regexp is trivial (ie. '.') and at top level
		    (including for recursive search), this is replaced
		    by the equivalent symbol '' (all match accepted)

		 */

	}

	if ((pDir=wopendir(lpDir))) {

		while ((lpDirElement=wreaddir(pDir))) {


			if ((
			        !iFlagRecursive
			        && Dos9_RegExpCaseMatch(lpRegExpLvl, lpDirElement->d_name)

			        /* if the search is non-recursive (or simply not recursive yet),
			           and the file name matches up with the level of regexp */

			    ) || (

			        iFlagRecursive

			        /*
			            if the search is recursive don't care wether the element
			            matches the regular expression. Just check that the
			            element is neither '.' nor '..', since it will probably
			            cause circular filesystem search.
			        */
			    )) {

				_Dos9_MakePath(lpDir, lpDirElement->d_name, lpFilePath, FILENAME_MAX);

				iIsPseudoDir=!(wcscmp(L".", lpDirElement->d_name)
				               && wcscmp(L"..", lpDirElement->d_name));

				switch(iFlagRecursive) {

				case TRUE:
					/* If the mode is reccursive:

					    - try to add the element, if the element matches the
					      regexp.

					    - try to browse subdirectories.

					 */

					if (Dos9_RegExpCaseMatch(lpRegExpLvl, lpDirElement->d_name)) {

						if ((iSearchFlag & DOS9_SEARCH_NO_PSEUDO_DIR)
						    && iIsPseudoDir)
							goto Dos9_DirRecursive;

						if (write(iOutDescriptor, lpFilePath, FILENAME_MAX*sizeof(wchar_t))==-1)
							return NULL;

						if (iSearchFlag & DOS9_SEARCH_GET_FIRST_MATCH)
							return NULL;

					}

Dos9_DirRecursive:

					if (!iIsPseudoDir && Dos9_DirExists(lpFilePath))
						_Dos9_SeekFiles(lpFilePath,
						                lpRegExp,
						                iLvl,
						                iMaxLvl,
						                iOutDescriptor,
						                iSearchFlag);

					break;

				case FALSE:
					/* If the mode is not recussive */

					if (iLvl == iMaxLvl) {

						if ((iSearchFlag & DOS9_SEARCH_NO_PSEUDO_DIR)
						    && iIsPseudoDir)
							break;


						if (write(iOutDescriptor, lpFilePath, FILENAME_MAX*sizeof(wchar_t))==-1)
							return NULL;

						if (iSearchFlag & DOS9_SEARCH_GET_FIRST_MATCH)
							return NULL;



					} else if (Dos9_DirExists(lpFilePath)) {

						_Dos9_SeekFiles(lpFilePath, lpRegExp, iLvl+1, iMaxLvl, iOutDescriptor, iSearchFlag);

					}

				}
			}
		}

		wclosedir(pDir);
	}
	return NULL;
}

int _Dos9_SplitMatchPath(const wchar_t* lpPathMatch, wchar_t* lpStaticPart, size_t iStaticSize,  wchar_t* lpMatchPart, size_t iMatchSize)
{
	const wchar_t* lpLastToken=NULL; /* a pointer to the last static delimiter */
	const wchar_t* lpCh=lpPathMatch; /* a pointer to browse string */
	int iContinue=TRUE;
	size_t iSize;

	while (*lpCh && iContinue) {

		/*
		    detect the part that is static (that does not contains
		    any matching characters
		 */

		switch(*lpCh) {

		case L'/':
		case L'\\':
			lpLastToken=lpCh+1;
			break;

		case L'?':
		case L'*':
			iContinue=FALSE;
			break;

		}

		lpCh++;

	}

	if (lpLastToken && !iContinue) {

		/*
		    if match part and static part are different
		*/

		iSize=lpLastToken-lpPathMatch;


		if ((iSize==1)) {

			/* the token is just '/' which means
			   volume root */
			iSize++;

		} else if ((iSize==3) && (*(lpPathMatch+1)==L':'))  {

			/* the token is a windows drive letter */
			iSize++;

		}

		if (iSize < iStaticSize)
			iStaticSize=iSize;

		wcsncpy(lpStaticPart, lpPathMatch, iStaticSize);
		lpStaticPart[iStaticSize-1]=L'\0';

		wcsncpy(lpMatchPart, lpLastToken, iMatchSize);
		lpStaticPart[iMatchSize-1]=L'\0';

	} else if (!iContinue) {

		/* if there is no static part */

		if (iStaticSize)
			*lpStaticPart=L'\0';

		wcsncpy(lpMatchPart, lpPathMatch, iMatchSize);
		lpStaticPart[iMatchSize-1]=L'\0';

	} else {

		/* if there is no match part */

		if (iMatchSize)
			*lpMatchPart=L'\0';

		wcsncpy(lpStaticPart, lpPathMatch, iStaticSize);
		lpStaticPart[iStaticSize-1]=L'\0';

	}

	return 0;
}

int _Dos9_GetMatchPart(const wchar_t* lpRegExp, wchar_t* lpBuffer, size_t iLength, int iLvl)
{
	const wchar_t *lpCh=lpRegExp,
	              *lpLastToken=lpRegExp;
	int iCurrentLvl=1, /* the first level is the level 1 */
	    iContinue=TRUE;

	size_t iSize;

	while (*lpCh && iContinue) {

		if (*lpCh==L'\\' || *lpCh==L'/') {

			if (iCurrentLvl==iLvl)
				iContinue=FALSE;

			iCurrentLvl++;
			lpLastToken=lpCh+1;

		}

		lpCh++;

	}

	if (!*lpCh) {

		/* if we are at string end */

		if (iLvl!=iCurrentLvl) {

			*lpBuffer=L'\0';

		} else {

			wcsncpy(lpBuffer, lpLastToken, iLength);
			lpBuffer[iLength-1]=L'\0';

		}

		return 0;

	}

	iSize=lpCh-lpLastToken+1;

	if (iSize < iLength)
		iLength=iSize;

	wcsncpy(lpBuffer, lpLastToken, iLength);
	lpBuffer[iSize-1]=L'\0';

	return 0;
}

int _Dos9_GetMatchDepth(wchar_t* lpRegExp)
{
	int iDepth;

	for (iDepth=1; *lpRegExp; lpRegExp++) {

		if (*lpRegExp==L'\\' || *lpRegExp==L'/') iDepth++;

	}

	return iDepth;

}

int _Dos9_MakePath(wchar_t* lpPathBase, wchar_t* lpPathEnd, wchar_t* lpBuffer, int iLength)
{
	int iLen=wcslen(lpPathBase)+wcslen(lpPathEnd);
	int i=0;

	if (lpPathBase[wcslen(lpPathBase)-1] != L'/' && lpPathBase[wcslen(lpPathBase)-1] != L'\\') {
		iLen++;
		i=1;
	}

	if (iLen>iLength) return -1;

	if (!wcscmp(lpPathBase, L".")) {

		wcscpy(lpBuffer, lpPathEnd);
		return 0;
	}

	wcscpy(lpBuffer, lpPathBase);

	if (i)
		wcscat(lpBuffer, L"/");

	wcscat(lpBuffer, lpPathEnd);

	return 0;
}

wchar_t* _Dos9_GetFileName(wchar_t* lpPath)
{
	wchar_t* lpLastPos=NULL;

	for (; *lpPath; lpPath++) {
		if (*lpPath==L'/' || *lpPath==L'\\') lpLastPos=lpPath+1;
	}

	return lpLastPos;
}

LIBDOS9 int Dos9_FreeFileList(LPFILELIST lpflFileList)
{

	_Dos9_FreeFileList(lpflFileList);

	return 0;

}

LPFILELIST _Dos9_WaitForFileList(LPFILEPARAMETER lpParam)
{
	wchar_t lpFileName[FILENAME_MAX];
	char cUseStat=lpParam->bStat;
	int iInDescriptor=lpParam->iInput;
	LPFILELIST lpflLast=NULL;
	LPFILELIST lpflCurrent=NULL;

	while (TRUE) {

		/* get data from the descriptor (take a large amount
		   of FILENAME_MAX bytes because of binary type of
		   the descriptor). */
		if (read(iInDescriptor, lpFileName, FILENAME_MAX*sizeof(wchar_t))) {

			if (*lpFileName==L'\1')
				break;

			if ((lpflCurrent=malloc(sizeof(FILELIST)))) {

				wcscpy(lpflCurrent->lpFileName, lpFileName);
				lpflCurrent->lpflNext=lpflLast;

				if ((cUseStat)) {

					wstat(lpFileName, &(lpflCurrent->stFileStats));

#if defined WIN32
					lpflCurrent->stFileStats.st_mode=GetFileAttributesW(lpFileName);
#endif

				}

				lpflLast=lpflCurrent;

			}
		}
	}

	Dos9_EndThread(lpflCurrent);
	return lpflCurrent;
}

int                 _Dos9_WaitForFileListCallBack(LPFILEPARAMETER lpParam)
{
	wchar_t lpFileName[FILENAME_MAX];
	char cUseStat=lpParam->bStat;
	int iInDescriptor=lpParam->iInput;
	size_t i=0;
	void(*pCallback)(FILELIST*)=lpParam->pCallBack;

	FILELIST flElement;

	while (TRUE) {

		/* get data from the descriptor (take a large amount
		   of FILENAME_MAX bytes because of binary type of
		   the descriptor). */
		if (read(iInDescriptor, lpFileName, FILENAME_MAX*sizeof(wchar_t))) {

			if (*lpFileName==L'\1')
				break;

			wcscpy(flElement.lpFileName, lpFileName);

			if (cUseStat) {

				wstat(lpFileName, &(flElement.stFileStats));

#ifdef WIN32

				/* Using windows, more detailled directory information
				   can be obtained */
				flElement.stFileStats.st_mode=GetFileAttributesW(lpFileName);

#endif // WIN32

			}

			/* use the callback function */
			pCallback(&flElement);

			i++;
		}

	}

	Dos9_EndThread((void*)i);
	return i;
}

int _Dos9_FreeFileList(LPFILELIST lpflFileList)
{
	LPFILELIST lpflNext;

	for (; lpflFileList; lpflFileList=lpflNext) {

		lpflNext=lpflFileList->lpflNext;
		free(lpflFileList);

	}

	return 0;
}

wchar_t* _Dos9_SeekCasePatterns(wchar_t* lpSearch, wchar_t* lpPattern)
{
	const wchar_t* lpPatternBegin=(const wchar_t*)lpPattern;
	wchar_t* lpLastBegin;
	while (TRUE) {

		for (; *lpSearch && (toupper(*lpPattern)!=toupper(*lpSearch)); lpSearch++);

		lpLastBegin=lpSearch;

		for (; *lpSearch && *lpPattern && *lpPattern!=L'*' && *lpPattern!=L'?'
				&& (towupper(*lpPattern)==towupper(*lpSearch)); lpSearch++, lpPattern++);

		if (!*lpPattern || *lpPattern==L'*' || *lpPattern==L'?') {
			//printf("<FOUND> (returns: '%s')\n",lpLastBegin, lpSearch);
			return lpSearch;
		}

		if (!*lpSearch) {
			//printf("[WRONG] ('%s')\n", lpLastBegin);
			return NULL;
		}

		lpSearch=lpLastBegin+1;
		lpPattern=(wchar_t*)lpPatternBegin;

	}

}

wchar_t* _Dos9_SeekPatterns(wchar_t* lpSearch, wchar_t* lpPattern)
{
	const wchar_t* lpPatternBegin=(const wchar_t*)lpPattern;
	wchar_t* lpLastBegin;
	while (TRUE) {

		/* search the next character identical to the given character */
		for (; *lpSearch && (*lpPattern!=*lpSearch); lpSearch++) {
		}

		lpLastBegin=lpSearch;

		/* loop until the loop encounter a character '*' or '?'
		   (the next regexp symbol) */
		for (; *lpSearch && *lpPattern && *lpPattern!=L'*' && *lpPattern!=L'?'
			   && (*lpPattern==*lpSearch); lpSearch++, lpPattern++) {
		}

		/* return if the pattern true is either finished or '*' or '?' */
		if (!*lpPattern || *lpPattern==L'*' || *lpPattern==L'?') {
			//printf("<FOUND> (returns: '%s')\n",lpLastBegin, lpSearch);
			return lpSearch;
		}

		/* return false if the loop browsed all the string without
		   valiable match */
		if (!*lpSearch) {
			//printf("[WRONG] ('%s')\n", lpLastBegin);
			return NULL;
		}

		lpSearch=lpLastBegin+1;
		lpPattern=(wchar_t*)lpPatternBegin;

	}
}
