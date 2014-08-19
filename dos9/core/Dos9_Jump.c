#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

#include <errno.h>
#include <string.h>

int Dos9_JumpToLabel(DOS9CONTEXT* pContext,
                        char* lpLabelName, char* lpFileName)
{
	size_t iSize=strlen(lpLabelName);
	char* lpName=lpFileName;
	FILE* pFile;
	ESTR* lpLine=Dos9_EsInit();


	if ((lpFileName==NULL)) {
		lpName=pContext->pIn->lpFileName;
	}


	if (!(pFile=fopen(lpName, "r"))) {
		Dos9_EsFree(lpLine);

		DEBUG("unable to open file : %s");
		DEBUG(strerror(errno));

		return -1;
	}

	while (!Dos9_EsGet(lpLine, pFile)) {

		if (!strnicmp(Dos9_EsToChar(lpLine), lpLabelName, iSize)) {

			if (lpFileName) {

				/* at that time, we can assume that lpFileName is not
				   the void string, because the void string is not usually
				   a valid file name */
				if (*lpFileName=='/'
				    || !strncmp(":/", lpFileName+1, 2)
				    || !strncmp(":\\", lpFileName+1, 2)) {

					/* the path is absolute */
					strncpy(pContext->pIn->lpFileName, lpFileName,
                                            sizeof(pContext->pIn->lpFileName));

					pContext->pIn->lpFileName[FILENAME_MAX-1]='\0';

				} else {

					/* the path is relative */
					snprintf(pContext->pIn->lpFileName,
					         sizeof(pContext->pIn->lpFileName),
					         "%s/%s",
					         pContext->lpCurrentDir,
					         lpFileName
					        );

				}

			}

			pContext->pIn->iPos=ftell(pFile);
			pContext->pIn->bEof=feof(pFile);


			DEBUG("Freeing data");

			fclose(pFile);
			Dos9_EsFree(lpLine);

			DEBUG("Jump created with success");

			return 0;
		}
	}

	fclose(pFile);
	Dos9_EsFree(lpLine);

	DEBUG("Unable to find label");

	return -1;
}

