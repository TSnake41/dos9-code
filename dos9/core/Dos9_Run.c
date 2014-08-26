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

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef _POSIX_C_SOURCE
#include <sys/wait.h>
#endif

#include "Dos9_Core.h"

#define DOS9_DBG_MODE

#include "Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"


int Dos9_RunBatch(DOS9CONTEXT* pContext)
{
	ESTR* lpLine=Dos9_EsInit();

	char *lpCh,
	     *lpTmp;

	int res;

	while (!(pContext->pIn->bEof)
            && !(pContext->iMode & DOS9_CONTEXT_ABORT_FILE)) {

		DOS9_DBG("[*] %d : Parsing new line\n", __LINE__);

        pContext->iMode &= ~ DOS9_CONTEXT_ABORT;

		if (*(pContext->pIn->lpFileName)=='\0' /* if taking cmd from prompt */
		    && (pContext->iMode &  DOS9_CONTEXT_ECHO_ON)
		    ) {

			/* this is a direct input */

			Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IGREEN | DOS9_GET_BACKGROUND(colColor));
			printf("\nDOS9 ");

			Dos9_SetConsoleTextColor(colColor);

			printf("%s>", pContext->lpCurrentDir);

		}

		if (Dos9_GetLine(lpLine, pContext->pIn))
			continue; /* skip if eof was encoutered */

		lpCh=Dos9_SkipBlanks(Dos9_EsToChar(lpLine));


        if (*(pContext->pIn->lpFileName)!='\0' /* taking commands from file */
		    && (pContext->iMode & DOS9_CONTEXT_ECHO_ON) /* echo is enabled */
		    && *lpCh!='@' /* there is no @ sign */
		    ) {

			Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IGREEN
                                    | DOS9_GET_BACKGROUND(colColor));
			printf("\nDOS9 ");
			Dos9_SetConsoleTextColor(colColor);

			printf("%s>%s", pContext->lpCurrentDir, Dos9_EsToChar(lpLine));

		}

        /* replace variables in the line. Only '%' signs will be expanded
           even if delayed expansion is enabled. Delayed expansion is
           executed when taking arguments from command line. */
		Dos9_ReplaceVars(pContext, lpLine);

        /* exec the line */
		Dos9_RunLine(pContext, lpLine);

		DOS9_DBG("\t[*] Line run.\n");

	}

	DOS9_DBG("*** Input ends here  ***\n");
    /* returns */
	return 0;

}

int Dos9_RunLine(DOS9CONTEXT* pContext, ESTR* lpLine)
{
	PARSED_STREAM_START* lppssStreamStart;
	PARSED_STREAM* lppsStream;

    int loop=TRUE,
        ret;

	Dos9_RmTrailingNl(Dos9_EsToChar(lpLine));

	lppssStreamStart=Dos9_ParseLine(pContext, lpLine);

	if (!lppssStreamStart) {
		DOS9_DBG("!!! Can't parse line : \"%s\".\n", strerror(errno));
		return -1;
	}

	if (Dos9_OpenOutput(pContext->pStack, lppssStreamStart))
        return -1;

	DOS9_DBG("\t[*] Global streams set.\n");

	lppsStream=lppssStreamStart->lppsStream;

	do {

        if (lppsStream->cNodeType != PARSED_STREAM_NODE_PIPE) {

            /* If we do not encountered a pipe, this is quite simple
                to handle, just run commands sequentially */
            ret = Dos9_RunCommand(pContext, lppsStream->lpCmdLine);

            switch(lppsStream->cNodeType) {

                case PARSED_STREAM_NODE_YES:
                    loop = !ret;
                    break;

                case PARSED_STREAM_NODE_NOT:
                    loop = ret;
                    break;

                /* case PARSED_STREAM_NODE_NONE:
                     if we arrived here, loop is obviously TRUE */

            }

        } else {

            /* not implemented : run piped commands simultaneously */

        }


        if (pContext->iMode & DOS9_CONTEXT_ABORT)
                loop = 0;

	} while ((lppsStream=lppsStream->lppsNode) && loop);

    pContext->pStack=Dos9_PopStreamStack(pContext->pStack);

	Dos9_FreeLine(lppssStreamStart);

	DOS9_DBG("\t[*] Line run.\n");

	DOS9_DBG("*** Input ends here  ***\n");

	return 0;
}

int Dos9_RunCommand(DOS9CONTEXT* pContext, ESTR* lpCommand)
{

	int (*lpProc)(DOS9CONTEXT*,char*),
        iFlag,
        iErrorLevel;

	char lpErrorlevel[]="-3000000000",
         *lpCmdLine;

	static int lastErrorLevel=0;

RestartSearch:

	lpCmdLine=Dos9_EsToChar(lpCommand);

	lpCmdLine=Dos9_SkipAllBlanks(lpCmdLine);

	switch((iFlag=Dos9_GetCommandProc(lpCmdLine, pContext->pCommands, (void**)&lpProc))) {

	case -1:
BackTrackExternalCommand:
        /* we shall run an external command */
		iErrorLevel=Dos9_RunExternalCommand(pContext, lpCmdLine);
		break;

	default:
		if (iFlag
		    && lpCmdLine[iFlag & ~DOS9_ALIAS_FLAG]!=' '
		    && lpCmdLine[iFlag & ~DOS9_ALIAS_FLAG]!='\t'
		    && lpCmdLine[iFlag & ~DOS9_ALIAS_FLAG]!='\0')
			goto BackTrackExternalCommand;

		if (iFlag & DOS9_ALIAS_FLAG) {

			/* We encountered an alias, expand it and loop again */
			Dos9_ExpandAlias(lpCommand,
			                 lpCmdLine + (iFlag & ~DOS9_ALIAS_FLAG),
			                 (char*)lpProc
			                );

			goto RestartSearch;

		}

		iErrorLevel=lpProc(pContext, lpCmdLine);

	}

	if (iErrorLevel!=pContext->iLastErrorLevel) {

		snprintf(lpErrorlevel, sizeof(lpErrorlevel), "%d", iErrorLevel);
		Dos9_SetEnv(pContext->pEnv, "ERRORLEVEL",lpErrorlevel);
		pContext->iLastErrorLevel=iErrorLevel;

	}

	pContext->iErrorLevel = iErrorLevel;

	return iErrorLevel;
}


int Dos9_RunBlock(DOS9CONTEXT* pContext, BLOCKINFO* lpbkInfo)
{

	ESTR *lpEsLine=Dos9_EsInit();

	char *lpToken = lpbkInfo->lpBegin,
	      *lpEnd = lpbkInfo->lpEnd,
	       *lpBlockBegin,
	       *lpBlockEnd,
	       *lpNl;

	size_t iSize;

	int iOldState;

	/* Save old lock state and lock the
	   level, definitely */
	iOldState=Dos9_GetStreamStackLockState(pContext->pStack);
	Dos9_SetStreamStackLockState(pContext->pStack, TRUE);

	DOS9_DBG("Block_b=\"%s\"\n"
	         "Block_e=\"%s\"\n",
	         lpToken,
	         lpEnd
	        );

	while (*lpToken && (lpToken < lpEnd)) {

		lpBlockBegin=Dos9_GetNextBlockBeginEx(lpToken, TRUE);

		/* get the block that are contained in the line */

		if (lpBlockBegin) {

			lpBlockEnd=Dos9_GetBlockLineEnd(lpBlockBegin);

			assert(lpBlockEnd != NULL);

			lpBlockBegin=lpBlockEnd;

		} else {

			lpBlockBegin=lpToken;

		}

		/* search the end of the line */
		if (!(lpBlockEnd=Dos9_SearchChar(lpBlockBegin, '\n'))) {

			lpBlockEnd=lpEnd;

		}

		lpBlockEnd++;

		if (lpBlockEnd > lpEnd)
			lpBlockEnd=lpEnd;

		iSize=lpBlockEnd-lpToken;

		Dos9_EsCpyN(lpEsLine, lpToken, iSize);

		//printf("Running=\"%s\"\n", Dos9_EsToChar(lpEsLine));

		//getch();

		lpToken=Dos9_SkipAllBlanks(lpToken);

		if (*lpToken=='\0'
		    || *lpToken=='\n') {

			/* don't run void lines, it is time wasting */
			lpToken=lpBlockEnd;

			continue;

		}

		lpToken=lpBlockEnd;

		Dos9_RunLine(pContext, lpEsLine);

		/* if we are asked to abort the command */
		if (pContext->iMode & DOS9_CONTEXT_ABORT)
			break;


	}

	/* releases the lock */
	Dos9_SetStreamStackLockState(pContext->pStack, iOldState);

	Dos9_EsFree(lpEsLine);

	return 0;
}

int Dos9_RunExternalCommand(DOS9CONTEXT* pContext, char* lpCommandLine)
{

	char *lpArguments[FILENAME_MAX],
	     lpFileName[FILENAME_MAX],
	     lpExt[_MAX_EXT],
	     lpTmp[FILENAME_MAX],
	     lpExePath[FILENAME_MAX];

	ESTR* lpEstr[FILENAME_MAX];

	int i=0;


	Dos9_GetParamArrayEs(pContext, lpCommandLine, lpEstr, FILENAME_MAX);

	if (!lpEstr[0])
		return 0;

	Dos9_EsReplace(lpEstr[0], "\"", "");

	for (; lpEstr[i] && (i < FILENAME_MAX); i++) {
		lpArguments[i]=Dos9_EsToChar(lpEstr[i]);
	}

	lpArguments[i]=NULL;

	/* check if the program exist */

	if (Dos9_GetFilePath(pContext, lpFileName, lpArguments[0], sizeof(lpFileName))==-1) {

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
		                      lpArguments[0],
		                      FALSE);
		goto error;

	}

	/* check if "command" is a batch file */
	Dos9_SplitPath(lpFileName, NULL, NULL, NULL, lpExt);

	if (!stricmp(".bat", lpExt)
	    || !stricmp(".cmd", lpExt)) {

		/* these are batch */

		strncpy(lpTmp, lpFileName, sizeof(lpFileName));

		if (!(i < FILENAME_MAX-2)) {

			i=FILENAME_MAX-3;
			lpArguments[i]=NULL;

		}

		for (; (i > 0); i--)
			lpArguments[i+2]=lpArguments[i];

		lpArguments[i+2]=NULL;

		lpArguments[2]=lpTmp;
		lpArguments[1]="//"; /* use this switch to prevent
                                other switches from being executed */

		Dos9_GetExePath(lpExePath, sizeof(lpExePath));

#ifdef WIN32

		snprintf(lpFileName, sizeof(lpFileName) ,"%s/dos9.exe", lpExePath);

#else

		snprintf(lpFileName, sizeof(lpFileName) ,"%s/dos9", lpExePath);

#endif // WIN32

	}

	if (Dos9_RunExternalFile(lpFileName, lpArguments)==-1)
		goto error;

	for (i=0; lpEstr[i]; i++)
		Dos9_EsFree(lpEstr[i]);

	return 0;


error:
	for (i=0; lpEstr[i] && (i < FILENAME_MAX); i++)
		Dos9_EsFree(lpEstr[i]);

	return -1;

}


#ifdef WIN32

int Dos9_RunExternalFile(char* lpFileName, char** lpArguments)
{
	int res;

	errno=0;

	/* in windows the result is directly returned */
	res=spawnv(_P_WAIT, lpFileName, (char * const*)lpArguments);

	if (errno==ENOENT) {

		res=-1;

		Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
		                      lpArguments[0],
		                      FALSE
		                     );

	}


	return res;

}

#elif defined _POSIX_C_SOURCE

int Dos9_RunExternalFile(char* lpFileName, char** lpArguments)
{
	pid_t iPid;

	int iResult;

	iPid=fork();

	if (iPid == 0 ) {
		/* if we are in the son */

		if ( execv(lpFileName, lpArguments) == -1) {

			/* if we got here, we can't set ERRORLEVEL
			   variable anymore, but print an error message anyway.

			   This is problematic because if fork do not fail (that
			   is the usual behaviour) command line such as

			        batbox || goto error

			   will not work as expected. However, during search in the
			   path, command found exist, so the risk of such a
			   dysfunction is limited.

			   For more safety, we return -1, so that the given value will be
			   reported anyway*/

			Dos9_ShowErrorMessage(DOS9_COMMAND_ERROR,
			                      lpArguments[0],
			                      FALSE
			                     );

			exit(-1);


		}

	} else {
		/* if we are in the father */

		if (iPid == (pid_t)-1) {
			/* the execution failed */
			return -1;

		} else {

			wait(&iResult);

		}

	}

	return 0;

}


#endif // WIN32 || _POSIX_C_SOURCE
