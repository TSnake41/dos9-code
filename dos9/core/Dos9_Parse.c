/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 Romain GARBI (DarkBatcher)
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libDos9.h>

#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

PARSED_STREAM_START* Dos9_ParseLine(DOS9CONTEXT* pContext, ESTR* lpesLine)
{
	PARSED_STREAM_START* lppssReturn; /* a pointer to the structure to be
                                         returned */

	PARSED_STREAM* lppsStream;

	if (!(lppssReturn=Dos9_ParseOutput(pContext, lpesLine)))
		return NULL;

	if (!(lppsStream=Dos9_ParseOperators(lpesLine))) {

		Dos9_FreeLine(lppssReturn);
		return NULL;

	}

	lppssReturn->lppsStream=lppsStream;

	return lppssReturn;
}

/* Get the '>' and '<' style redirections */
PARSED_STREAM_START* Dos9_ParseOutput(DOS9CONTEXT* pContext, ESTR* lpesLine)
{

	char *lpCh=Dos9_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char lpCorrect[]="1";

	char cChar;

	int   fd, fd2;

	ESTR *lpesFinal=Dos9_EsInit(),
	      *lpesParam=Dos9_EsInit();

	PARSED_STREAM_START* lppssStart;

	if (!(lppssStart=Dos9_AllocParsedStreamStart())) {

		Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
		                      __FILE__ "/ParseOutput()",
		                      -1);
		goto error;

	}

	lpSearchBegin=lpCh;
	lpNextBlock=Dos9_GetNextBlockBegin(lpCh);

    /* seek any '>' and '<' operators out of quotes and blocks */
	while ((lpNextToken=Dos9_SearchToken_OutQuotes(lpSearchBegin, "<>"))) {


        /* Jump over the blocks to ignore them */
		if ((lpNextToken >= lpNextBlock)
		    && (lpNextBlock!=NULL)) {

			lpSearchBegin=Dos9_GetBlockLineEnd(lpNextBlock);

			if (lpSearchBegin==NULL) {

				Dos9_FreeLine(lppssStart);

				Dos9_ShowErrorMessage(DOS9_MALFORMED_BLOCKS, lpCh, FALSE);


				goto error;

			}

			lpNextBlock=Dos9_GetNextBlockBegin(lpSearchBegin);

			continue;

		}

		cChar = *lpNextToken;
		*lpNextToken = '\0';

		Dos9_EsCat(lpesFinal, lpCh);

		switch (cChar) {

            case '>':
                /* handling output is more difficult, because it may be :

                        n>&m
                        n> file

                 */
                fd = Dos9_GetDescriptor(lpNextToken, Dos9_EsToChar(lpesLine));

                lpNextToken++;

                if (*lpNextToken == '&') {
                    /* we are redirecting a file descriptor into another file
                       descriptor */

                    fd2 = strtol(++ lpNextToken, &lpCh, 10); /* accept only decimal
                        numbers */

                    if ((fd2 == STDOUT_FILENO) && (fd == STDERR_FILENO)) {

                        lppssStart->cOutputMode |= PSTREAMSTART_REDIR_OUT;

                    } else if ((fd2 == STDERR_FILENO)
                                && (fd == STDOUT_FILENO)) {

                        lppssStart->cOutputMode |= PSTREAMSTART_REDIR_ERR;

                    }

                    if ((lppssStart->cOutputMode & PSTREAMSTART_REDIR_ERR)
                        && (lppssStart->cOutputMode & PSTREAMSTART_REDIR_OUT)) {

                        /* Prevent user from creating a circular file redirection
                           with stdout refferring to stderr, and stderr refferring
                           to stdout */

                        Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION,
                                                NULL,
                                                FALSE
                                                );

                        goto error;

                    }

                } else {

                    if (!(lpCh =
                        Dos9_GetNextParameterEs(pContext, lpNextToken+1,
                                                                lpesParam))) {

                        Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION,
                                                NULL,
                                                FALSE);

                        goto error;

                    }

                    if (((fd == STDOUT_FILENO) && !(lppssStart->lpOutputFile))
                        || ((fd == STDERR_FILENO)
                                && !(lppssStart->lpErrorFile))) {

                        Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED,
                                               (fd == STDOUT_FILENO) ?
                                                    lppssStart->lpOutputFile : lppssStart->lpErrorFile,
                                                FALSE);

                        goto error;


                    }

                    switch (fd) {

                    case STDOUT_FILENO:

                        if (!(lppssStart->lpOutputFile =
                                strdup(Dos9_EsToChar(lpesParam)))) {

                            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                                | DOS9_PRINT_C_ERROR,
                                            __FILE__ "/Dos9_ParseOutput()",
                                            FALSE);

                            goto error;

                        }

                        break;

                    case STDERR_FILENO:

                        if (!(lppssStart->lpErrorFile =
                                strdup(Dos9_EsToChar(lpesParam)))) {

                            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                                | DOS9_PRINT_C_ERROR,
                                            __FILE__ "/Dos9_ParseOutput()",
                                            FALSE);

                            goto error;

                        }


                    }

                }

                break;

            case '<':
                /* handling input is quite simple, because it only needs to
                   get the next parameter */

                if (!(lpCh = Dos9_GetNextParameterEs(pContext, lpNextToken+1,
                                                                lpesParam))) {

                    Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION,
                                            NULL,
                                            FALSE);

                    goto error;

                }

                if (lppssStart->lpInputFile) {

                    Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED,
                                            lppssStart->lpInputFile,
                                            FALSE);

                }

                /* get the string back */
                if (!(lppssStart->lpInputFile =
                        strdup(Dos9_EsToChar(lpesParam)))) {

                    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                                | DOS9_PRINT_C_ERROR,
                                            __FILE__ "/Dos9_ParseOutput()",
                                            TRUE
                                            );


                    goto error;

                }

		}

		lpSearchBegin = lpCh;

	}

	Dos9_EsCat(lpesFinal, lpCh);

	Dos9_EsCpyE(lpesLine, lpesFinal);
	Dos9_EsFree(lpesFinal);
	Dos9_EsFree(lpesParam);

	return lppssStart;

error:
	/* if some fail happened, free memory. However, the
	   origin string will be useless.

	   Note: Therre is memory that can not be free'd, so the
	   only thing to do is to exit in order to clean up memory*/
	Dos9_EsFree(lpesFinal);
	Dos9_EsFree(lpesParam);
	return NULL;

}

PARSED_STREAM*       Dos9_ParseOperators(ESTR* lpesLine)
{
	PARSED_STREAM *lppsStream=NULL,
					*lppsStreamBegin=NULL;

	char *lpCh=Dos9_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char cChar,
	     cNodeType=PARSED_STREAM_NODE_NONE;

	if (!(lppsStreamBegin=Dos9_AllocParsedStream(NULL))) {

		Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
		                      __FILE__ "/Dos9_ParseOperators()",
		                      -1
		                     );

		goto error;

	}

	lppsStream=lppsStreamBegin;

	lpNextBlock=Dos9_GetNextBlockBegin(lpCh);
	lpSearchBegin=lpCh;

	while ((lpNextToken=Dos9_SearchToken_OutQuotes(lpSearchBegin, "|&"))) {

		if ((lpNextToken >= lpNextBlock)
		    && (lpNextBlock != NULL)) {

			lpSearchBegin=Dos9_GetNextBlockEnd(lpNextBlock);

			if (lpSearchBegin == NULL) {

				Dos9_ShowErrorMessage(DOS9_MALFORMED_BLOCKS, lpCh, FALSE);

				goto error;

			}

			lpNextBlock=Dos9_GetNextBlockBegin(lpSearchBegin);

			continue;
		}

		cChar=*lpNextToken;
		*lpNextToken='\0';
		lpNextToken++;

		/* copy the data into the parsed stream structure */
		Dos9_EsCpy(lppsStream->lpCmdLine, lpCh);
		lppsStream->cNodeType=cNodeType;

		if (!(lppsStream=Dos9_AllocParsedStream(lppsStream))) {

			Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
			                      __FILE__ "/Dos9_ParseOperators()",
			                      -1
			                     );

			goto error;

		}

		switch (cChar) {

		case '|':

			if (*lpNextToken=='|') {

				lpNextToken++;
				cNodeType=PARSED_STREAM_NODE_NOT;

			} else {

				cNodeType=PARSED_STREAM_NODE_PIPE;

			}

			break;

		case '&':

			if (*lpNextToken=='&') {

				lpNextToken++;
				cNodeType=PARSED_STREAM_NODE_YES;

			} else {

				cNodeType=PARSED_STREAM_NODE_NONE;

			}

		}

		lpCh=lpNextToken;
		lpSearchBegin=lpCh;
	}

	Dos9_EsCpy(lppsStream->lpCmdLine, lpCh);
	lppsStream->cNodeType=cNodeType;

	return lppsStreamBegin;

error:

	Dos9_FreeParsedStream(lppsStreamBegin);
	return NULL;

}



PARSED_STREAM* Dos9_AllocParsedStream(PARSED_STREAM* lppsStream)
{

	PARSED_STREAM* lppsNewElement;

	if ((lppsNewElement=(PARSED_STREAM*)malloc(sizeof(PARSED_STREAM)))) {
		if (lppsStream)
			lppsStream->lppsNode=lppsNewElement;

		lppsNewElement->lppsNode=NULL;
		lppsNewElement->lpCmdLine=Dos9_EsInit();

		return lppsNewElement;
	}

	return NULL;

}

PARSED_STREAM_START* Dos9_AllocParsedStreamStart(void)
{
	PARSED_STREAM_START* lppssStreamStart;

	if ((lppssStreamStart=malloc(sizeof(PARSED_STREAM_START)))) {

		lppssStreamStart->cOutputMode=0;

		lppssStreamStart->lppsStream=NULL;
		lppssStreamStart->lpInputFile=NULL;
		lppssStreamStart->lpOutputFile=NULL;

		return lppssStreamStart;
	}

	return NULL;
}

void Dos9_FreeParsedStreamStart(PARSED_STREAM_START* lppssStart)
{
	free(lppssStart->lpInputFile);
	free(lppssStart->lpOutputFile);

	Dos9_FreeParsedStream(lppssStart->lppsStream);

	free(lppssStart);
}

void Dos9_FreeParsedStream(PARSED_STREAM* lppsStream)
{
	PARSED_STREAM* lppsLast=NULL;

	for(; lppsStream; lppsStream=lppsStream->lppsNode) {

		if (lppsLast)
			free(lppsLast);

		lppsLast=lppsStream;
		Dos9_EsFree(lppsStream->lpCmdLine);

	}

	if (lppsLast) free(lppsLast);
}

int Dos9_GetDescriptor(const char* current, const char* begin)
{

    int fd=0, multiplier=1;

    while ((current >= begin) && (*current==' ' || *current=='\t'))
        current --;

    if ((--current) <= begin)
        return STDOUT_FILENO;


    while ((current >= begin) && (*current>= '0') && (*current <= '9')) {

        fd=fd+(*(current --) - '0')*multiplier;
        multiplier*=10;

    }

    if (!fd)
        return STDOUT_FILENO;

    return fd;

}

