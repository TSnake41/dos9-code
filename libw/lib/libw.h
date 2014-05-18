/*
 *
 *   libw - a wide character compatibility layer for *Nixes.
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

#ifndef LIBW_H
#define LIBW_H

#include <wchar.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>

#include "libw_conf.h"

#ifndef HAVE_WCSERROR
#ifdef  HAVE__WCSERROR
#define wcserror _wcserror
#else
wchar_t* wcserror(int errnum);
#define BUILD_WCSERROR 1
#endif // defined _wcserror
#endif // wcserror

#ifndef HAVE_WSTAT
#ifdef  HAVE__WSTAT
#define wstat _wstat
#else
int wstat(wchar_t* lpwFile, struct stat* pStat);
#define BUILD_WSTAT 1
#endif // _wstat
#endif // wstat

#ifndef HAVE_WGETENV
#ifdef  HAVE__WGETENV
#define wgetenv _wgetenv
#else
wchar_t* wgetenv(const wchar_t* lpEnv);
#define BUILD_WGETENV 1
#endif //_wgetenv
#endif //wgetenv

#ifndef HAVE_WCSCASECMP
#ifdef  HAVE__WCSICMP
#define wcscasecmp _wcsicmp
#else
int wcscasecmp(const wchar_t* lpw1, const wchar_t* lpw2);
#define BUILD_WCSCASECMP 1
#endif //_wcsicmp
#endif // wcscasecmp

#ifndef HAVE_WFOPEN
#ifdef  HAVE__WFOPEN
#define wfopen _wfopen
#else
FILE* wfopen(const wchar_t* lpwFile, const wchar_t* lpvMode);
#define BUILD_WFOPEN 1
#endif // _wfopen
#endif // wfopen

#ifndef HAVE_WTOL
#ifdef  HAVE__WTOL
#define wtol _wtol
#else
long wtol(const wchar_t* lpwNum);
#define BUILD_WTOL 1
#endif // _wtol
#endif //wtof

#ifndef HAVE_WTOI
#ifdef  HAVE_WTOI
#define wtoi _wtoi
#else
#define BUILD_WTOI 1
int wtoi(const wchar_t* lpNum);
#endif // _wtoi
#endif // wtol

#ifndef HAVE_SWPRINTF
#ifdef  HAVE__SWPRINTF
#define swprintf _swprintf
#else
int swprintf(wchar_t* dest, const wchar_t* format, ...);
#define BUILD_SWPRINTF 1
#endif // _swprintf
#endif // swprintf

#ifndef HAVE_SNWPRINTF
#ifdef  HAVE__SNWPRINTF
#error we have snwprintf
#define snwprintf _snwprintf
#else
int snwprintf(wchar_t* dest, size_t n, const wchar_t* format, ...);
#endif // _snwprintf
#endif // snwprintf

#define LIBW

/*
	Convert a multibyte string to a wide string and return the result.
	If fails, it returns NULL.
	You must use free to free the converted string
*/
LIBW char* libw_wcstombs(const wchar_t* lpIn);

/*
	Exact opposite of the previous function.
 */
LIBW wchar_t* libw_mbstowcs(const char* lpIn);


#endif // LIBW_H
