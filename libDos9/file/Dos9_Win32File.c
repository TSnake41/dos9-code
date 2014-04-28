#include "Dos9_File.h"

#if defined WIN32

static wchar_t lpCurrentDir[FILENAME_MAX+3]=L"CD=";

int Dos9_GetExePath(wchar_t* lpBuf, size_t iBufSize)
{
    wchar_t* lpToken;

    GetModuleFileNameW(NULL, lpBuf, iBufSize);

    if ((lpToken=wcsrchr(lpBuf, L'\\'))) {

        *lpToken=L'\0';

    } else {

        lpBuf[0]=L'\0';

    }

    return 0;
}


int Dos9_FileExists(wchar_t* ptrName)
{
    int iAttrib = GetFileAttributesW(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            !(iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int Dos9_DirExists(wchar_t* ptrName)
{
    int iAttrib = GetFileAttributesW(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            (iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int Dos9_UpdateCurrentDir(void)
{
    _wgetcwd(lpCurrentDir+3, MAX_PATH);

    return _wputenv(lpCurrentDir);
}

int Dos9_SetCurrentDir(wchar_t* lpPath)
{
	int iRes;

    if (_wchdir(lpPath))
    	return -1;

    _wgetcwd(lpCurrentDir+3, MAX_PATH);

    return _wputenv(lpCurrentDir);
}

wchar_t* Dos9_GetCurrentDir(void)
{
    return lpCurrentDir+3;
}

#endif
