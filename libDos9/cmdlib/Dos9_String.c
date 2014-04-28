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
#include <wchar.h>

#include "../LibDos9.h"

LIBDOS9 wchar_t* Dos9_SkipBlanks(wchar_t* lpCh)
{
    while (*lpCh==L' ' || *lpCh==L'\t')
        lpCh++;

    if (*lpCh==L'^')
        lpCh++;

    return lpCh;
}

LIBDOS9 wchar_t* Dos9_SkipAllBlanks(wchar_t* lpCh)
{
    while (*lpCh==L' '
           || *lpCh==L'\t'
           || *lpCh==L'@' /* this is the silent character */
           || *lpCh==L';' /* and this the default eol-char */
           )
        lpCh++;

    return lpCh;
}

LIBDOS9 wchar_t* Dos9_SearchChar(wchar_t* lpCh, wchar_t cChar)
{

    wchar_t* lpNxt;

    while ((lpNxt=wcschr(lpCh, cChar))) {

        /* if the we are at the beginnig, of the
           string, don't search anymore and return
           lpNext */

        if (lpNxt==lpCh)
            break;

        /* else, we are further in the line, so that
           the fact that *(lpNext-1) is not a buffer
           overrun can be assumed */

        /* if the character is not escaped, use it
           as the needed character */

        if (*(lpNxt-1)!=L'^')
            break;

        /* the character is escaped, just loop
           back till we found the right */

        lpCh=lpNxt+1;

    }

    return lpNxt;

}

LIBDOS9 wchar_t* Dos9_SearchLastChar(wchar_t* lpCh, wchar_t cChar)
{
    wchar_t *lpLastMatch=NULL,
            *lpNxt;

    while ((lpNxt=wcschr(lpCh, cChar))) {

        if (lpCh==lpNxt) {

            lpLastMatch=lpNxt;

        } else {

            if (*(lpNxt-1)!=L'^')
                lpLastMatch=lpNxt;

        }

        lpCh=lpNxt+1;

    }

    return lpLastMatch;
}

LIBDOS9 void Dos9_UnEscape(wchar_t* lpCh)
{
    wchar_t *lpUnEscapeCh=lpCh;

    while (TRUE) {

        if (*lpCh==L'^') {

            /* this should be un escaped */

            lpCh++;


            /* if the caracter escaped is a line-feed,
               don't copy the line feed */

            if (*lpCh==L'\n')
                lpCh++;

        }

        if (lpUnEscapeCh!=lpCh)
            *lpUnEscapeCh=*lpCh;

        if (*lpCh==L'\0')
            return;

        lpUnEscapeCh++;
        lpCh++;

    }

}

LIBDOS9 wchar_t* Dos9_GetNextNonEscaped(wchar_t* lpCh)
{

    if (*lpCh==L'^')
        lpCh++;

    if (*lpCh)
        lpCh++;

    return lpCh;
}

LIBDOS9 wchar_t* Dos9_SearchToken(wchar_t* lpCh, wchar_t* lpDelims)
{

    wchar_t* lpNxt;

    while ((lpNxt=wcspbrk(lpCh, lpDelims))) {

        if (lpNxt==lpCh)
            break;

        if (*(lpNxt-1)!=L'^')
            break;

        lpCh=lpNxt+1;

    }

    return lpNxt;

}

LIBDOS9 wchar_t* Dos9_SearchLastToken(wchar_t* lpCh, wchar_t* lpDelims)
{
    wchar_t *lpNxt,
			*lpLastOccurence=NULL;


    while ((lpNxt=wcspbrk(lpCh, lpDelims))) {

        if (lpCh==lpNxt) {

            lpLastOccurence=lpCh;

        } else {

            if (*(lpNxt-1)!=L'^')
                lpLastOccurence=lpNxt;

        }

        lpCh=lpNxt+1;

    }

    return lpLastOccurence;
}
