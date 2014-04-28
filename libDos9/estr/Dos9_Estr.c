#include "../LibDos9.h"

#ifndef DEFAULT_ESTR
#define DEFAULT_ESTR 32
#endif

#define _Dos9_EsTotalLen(ptrChaine) ((wcslen(ptrChaine)/DEFAULT_ESTR+1)*DEFAULT_ESTR)
#define _Dos9_EsTotalLen2(ptrChaine, ptrString) (((wcslen(ptrChaine)+wcslen(ptrString))/DEFAULT_ESTR+1)*DEFAULT_ESTR)
#define _Dos9_EsTotalLen3(ptrChaine,iSize) (((wcslen(ptrChaine)+iSize)/DEFAULT_ESTR+1)*DEFAULT_ESTR)
#define _Dos9_EsTotalLen4(iSize) ((iSize/DEFAULT_ESTR+1)*DEFAULT_ESTR)


int _Dos9_NewLine=DOS9_NEWLINE_LINUX;

int _Dos9_Estr_Init(void)
{
    return 0;
}


void _Dos9_Estr_Close(void)
{

}

ESTR* Dos9_EsInit(void)
{
    ESTR* ptrESTR=NULL;

    ptrESTR=malloc(sizeof(ESTR));

    if (ptrESTR)
    {

        ptrESTR->ptrString=malloc(DEFAULT_ESTR*sizeof(wchar_t));

        if (!(ptrESTR->ptrString))
            goto Dos9_EsInit_Error;

        ptrESTR->iLenght=DEFAULT_ESTR;

    } else {

        Dos9_EsInit_Error:

        puts("Error : Not Enough memory to run Dos9. Exiting...");
        exit(-1);

        return NULL;
        /* if exist fails */

    }

    *(ptrESTR->ptrString)='\0';

    /* Release the lock */

    return ptrESTR;
}

LIBDOS9 int Dos9_EsFree(ESTR* ptrESTR)
{
    /* free the string buffer */

    free(ptrESTR->ptrString);

    /* free the structure itself */
    free(ptrESTR);

    return 0;
}

LIBDOS9 int Dos9_EsGet(ESTR* ptrESTR, FILE* ptrFile)
{
    int iCurrentL=0;

    size_t iTotalBytesRead=0;

    wchar_t *crLf=NULL,
         	*ptrCursor=NULL,
         	*lpResult;

    ptrCursor=ptrESTR->ptrString;
    iCurrentL=ptrESTR->iLenght-1;

    while (1)
    {

        lpResult=fgetws(ptrCursor, iCurrentL+1, ptrFile);


        if(lpResult==NULL) {

            /* this means we will continue because there is no more
               stuff to get from the file */

            break;

        } else {

            /* this could appear to be the number of byte read
               however, it is just used to know wether the
               function have read some text there */

            iTotalBytesRead+=(size_t)lpResult;

        }

        crLf=wcschr(ptrESTR->ptrString, L'\n');

        if (crLf!=NULL)
            break;

        iCurrentL=ptrESTR->iLenght;

        ptrESTR->iLenght=ptrESTR->iLenght*2;

        crLf=realloc(ptrESTR->ptrString, ptrESTR->iLenght*sizeof(wchar_t));

        if (crLf==NULL) {
                /* make if more fault tolerant, abort the loop,
                   and just read a part of the real line (not
                   so bad however). */

                ptrESTR->iLenght=iCurrentL;

                return -1;
        }

        ptrESTR->ptrString=crLf;

        ptrCursor=ptrESTR->ptrString+iCurrentL-1;

    }

    switch (_Dos9_NewLine) {

        case DOS9_NEWLINE_WINDOWS :

            Dos9_EsReplace(ptrESTR, L"\r\n", L"\x1\x2\x3\x4");
            Dos9_EsReplace(ptrESTR, L"\n", L"\r\n");
            Dos9_EsReplace(ptrESTR, L"\r", L"\r\n");
            Dos9_EsReplace(ptrESTR, L"\x1\x2\x3\x4", L"\r\n");
			/* not really stable */

            break;

        case DOS9_NEWLINE_LINUX :

            Dos9_EsReplace(ptrESTR, L"\r\n", L"\n");
            Dos9_EsReplace(ptrESTR, L"\r", L"\n");

            break;

        case DOS9_NEWLINE_MAC :

            Dos9_EsReplace(ptrESTR, L"\r\n", L"\r");
            Dos9_EsReplace(ptrESTR, L"\n", L"\r");


    }

    //printf(" Returning\n");

    return !iTotalBytesRead;
}


LIBDOS9 int Dos9_EsCpy(ESTR* ptrESTR, const wchar_t* ptrChaine)
{
    size_t iLen=_Dos9_EsTotalLen(ptrChaine);
    wchar_t* ptrBuf=ptrESTR->ptrString;

    if (ptrESTR->iLenght < iLen) {

        if (!(ptrBuf=realloc(ptrBuf, iLen*sizeof(wchar_t))))
            return -1;

        ptrESTR->ptrString=ptrBuf;
        ptrESTR->iLenght=iLen;
    }

    wcscpy(ptrBuf, ptrChaine);

    return 0;
}

LIBDOS9 int Dos9_EsCpyN(ESTR* ptrESTR, const wchar_t* ptrChaine, size_t iSize)
{
    size_t iLen=_Dos9_EsTotalLen4(iSize);
    wchar_t* ptrBuf=ptrESTR->ptrString;

    if (ptrESTR->iLenght < iLen)
    {
        if (!(ptrBuf=realloc(ptrBuf, iLen*sizeof(wchar_t))))
            return -1;

        ptrESTR->ptrString=ptrBuf;
        ptrESTR->iLenght=iLen;

    }

        /* this is some test to prevent user from experiencing
           crash in windows whenever iSize is 0. Sadly, windows
           crt's send SIGSEGV to the program, although this
           is correct for C89 standard (afaik) */

    if (iSize)
       wcsncpy(ptrBuf, ptrChaine, iSize);

    ptrBuf[iSize]=L'\0';

    return 0;
}

LIBDOS9 int Dos9_EsCat(ESTR* ptrESTR, const wchar_t* ptrChaine)
{
   int iLen=_Dos9_EsTotalLen2(ptrESTR->ptrString,ptrChaine);
   wchar_t* lpBuf=ptrESTR->ptrString;

   if ((ptrESTR->iLenght<iLen)) {

        if (!(lpBuf=realloc(lpBuf, iLen*sizeof(wchar_t))))
            return -1;

        ptrESTR->ptrString=lpBuf;
        ptrESTR->iLenght=iLen;
   }

   wcscat(lpBuf, ptrChaine);

   return 0;
}

LIBDOS9 int Dos9_EsCatN(ESTR* ptrESTR, const wchar_t* ptrChaine, size_t iSize)
{
   int iLen=_Dos9_EsTotalLen3(ptrESTR->ptrString,iSize+1);
   wchar_t* lpBuf=ptrESTR->ptrString;

   if (ptrESTR->iLenght<iLen) {

        if (!(lpBuf=realloc(lpBuf, iLen*sizeof(wchar_t))))
            return -1;

        ptrESTR->ptrString=lpBuf;
        ptrESTR->iLenght=iLen;
   }

   wcsncat(lpBuf, ptrChaine, iSize);

   return 0;
}

LIBDOS9 int Dos9_EsCpyE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=_Dos9_EsTotalLen(ptrSource->ptrString);
    wchar_t* ptrBuf=ptrDest->ptrString;

    if (iLen > ptrDest->iLenght)
    {
        if (!(ptrBuf=realloc(ptrBuf, iLen*sizeof(wchar_t))))
            return -1;

        ptrDest->ptrString=ptrBuf;
        ptrDest->iLenght=iLen;
    }

    wcscpy(ptrBuf, ptrSource->ptrString);

    return 0;
}

LIBDOS9 int Dos9_EsCatE(ESTR* ptrDest, const ESTR* ptrSource)
{
    int iLen=_Dos9_EsTotalLen2(ptrDest->ptrString, ptrSource->ptrString);
    wchar_t* lpBuf=ptrDest->ptrString;

    if (ptrDest->iLenght<iLen) {

        if (!(lpBuf=realloc(lpBuf,iLen*sizeof(wchar_t))))
            return -1;

        ptrDest->ptrString=lpBuf;
        ptrDest->iLenght=iLen;
    }

    wcscat(lpBuf, ptrSource->ptrString);

    return 0;
}

LIBDOS9 int Dos9_EsReplace(ESTR* ptrESTR, const wchar_t* ptrPattern, const wchar_t* ptrReplace)
{
    wchar_t* lpBuffer=Dos9_EsToChar(ptrESTR), *lpToken;
    int iLength=wcslen(ptrPattern);
    ESTR *lpReturn=Dos9_EsInit();

    while ((lpToken=wcsstr(lpBuffer, ptrPattern))) {

        *lpToken=L'\0';

        Dos9_EsCat(lpReturn, lpBuffer);
        Dos9_EsCat(lpReturn, ptrReplace);

        lpBuffer=lpToken+iLength;

    }

    Dos9_EsCat(lpReturn, lpBuffer);
    Dos9_EsCpyE(ptrESTR, lpReturn);
    Dos9_EsFree(lpReturn);

    return 0;
}
