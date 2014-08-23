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

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "../libDos9.h"

#include "Dos9_File.h"

#ifndef WIN32

#include <sys/stat.h>

int Dos9_GetExePath(char* lpBuf, size_t iBufSize)
{
    char* lpDelim;

    #ifdef DOS9_FILE_SYM_LINK

        if (readlink(DOS9_FILE_SYM_LINK, lpBuf, iBufSize)==-1) return -1;

        if ((lpDelim=strrchr(lpBuf, '/'))) {

            *lpDelim='\0';

        }

    #else

        /* we shoud add custom codes for various operating systems.
           Operating systems not currently supported by are FreeBSD,
           MacOS X, and solaris */


    #endif

    return 0;
}

int Dos9_DirExists(const char *ptrName)
{
    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISDIR(sStat.st_mode);

}

int Dos9_FileExists(const char* ptrName)
{
    struct stat sStat;

    if (stat(ptrName, &sStat)==-1)
        return FALSE;

    return S_ISREG(sStat.st_mode);

}

#endif

LIBDOS9 int Dos9_AbsolutePath(char* buf, size_t bufsiz,
                                    const char* currdir, const char* reldir)
{

        if (*reldir == '/'
        #if defined(WIN32)
            || (*reldir /* this string is not null terminated */
                && *(reldir+1) == ':'
                && (*(reldir+2) == '/'
                    || *(reldir+2) == '\\'))
        #endif
            ) {

            /* This path is absolute */

            snprintf(buf, bufsiz, "%s", reldir);

        } else if (!*reldir) {

            snprintf(buf, bufsiz, "%s", currdir);

        } else {

            snprintf(buf, bufsiz, "%s/%s", currdir, reldir);

        }

}

LIBDOS9 int Dos9_IsAbsolute(const char* path)
{

    return (*path == '/'
    #if defined(WIN32)
        || (*path
            && *(path+1) == ':'
            && (*(path+2) == '\\'
                || *(path+2) == '/'))
    #endif
    );
}
