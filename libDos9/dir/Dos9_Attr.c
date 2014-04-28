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

#include "../LibDos9.h"

short Dos9_MakeFileAttributes(const wchar_t* lpToken)
{
    short wAttr=0;
    short wFlag=0;
    char bNeg=FALSE;

    for (;*lpToken;lpToken++) {

        switch(toupper(*lpToken)){
            case L'I':
                wFlag=DOS9_CMD_ATTR_VOID;
                break;

            case L'A':
                wFlag=DOS9_CMD_ATTR_ARCHIVE;
                break;

            case L'R':
                wFlag=DOS9_CMD_ATTR_READONLY;
                break;

            case L'H':
                wFlag=DOS9_CMD_ATTR_HIDEN;
                break;

            case L'S':
                wFlag=DOS9_CMD_ATTR_SYSTEM;
                break;

            case L'D':
                wFlag=DOS9_CMD_ATTR_DIR;
                break;

            case L'-':
                bNeg=TRUE;

            default:
                wFlag=0;
        }
        if (bNeg && wFlag) {

            wFlag|=wFlag<<1;
            bNeg=FALSE;

        }

        wAttr|=wFlag;

    }

    return wAttr;
}

int Dos9_CheckFileAttributes(short wAttr, const FILELIST* lpflList)
{
    int iResult=0;
    int iReturn=1;
    int iAttributes;

    if (!wAttr) return TRUE;

    iAttributes=Dos9_GetFileMode(lpflList);

    if (wAttr & DOS9_CMD_ATTR_ARCHIVE) {
        iResult=iAttributes & DOS9_FILE_ARCHIVE;
        if (wAttr & DOS9_CMD_ATTR_ARCHIVE_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & DOS9_CMD_ATTR_HIDEN) {
        iResult=iAttributes & DOS9_FILE_HIDDEN;
        if (wAttr & DOS9_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & DOS9_CMD_ATTR_SYSTEM) {
        iResult=iAttributes & DOS9_FILE_SYSTEM;
        if (wAttr & DOS9_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    if (wAttr & DOS9_CMD_ATTR_VOID) {
        iResult=(Dos9_GetFileSize(lpflList)==0);
        if (wAttr & DOS9_CMD_ATTR_HIDEN_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }


    if (wAttr & DOS9_CMD_ATTR_DIR) {
        iResult=iAttributes & DOS9_FILE_DIR;
        if (wAttr & DOS9_CMD_ATTR_DIR_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    #if defined WIN32

    if (wAttr & DOS9_CMD_ATTR_READONLY) {
        iResult=iAttributes & DOS9_FILE_READONLY;
        if (wAttr & DOS9_CMD_ATTR_READONLY_N) iResult=!iResult;
        iReturn=iReturn && iResult;
    }

    #elif defined _POSIX_C_SOURCE

    if (wAttr & DOS9_CMD_ATTR_READONLY) {

        iResult=(Dos9_GetFileMode(lpflList) & (DOS9_FILE_READ | DOS9_FILE_WRITE)) == DOS9_FILE_READ;
        if (wAttr & DOS9_CMD_ATTR_READONLY_N) iResult=!iResult;
        iReturn=iReturn &&iResult;

    }

    #endif

    return iReturn;
}
