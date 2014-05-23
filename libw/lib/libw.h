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

/* This lib provides a compatibility layer for *Nixes system calls. Indeed
   Some *Nixes do not ship wide characters systems call, nor wide characters
   equivalent of libc function. Thus this library provides these wrappers
   for systems that do not support these functions.

   Usually, equivalent searched on *Nixes systems match the name that are not
   prefixed with an underscore ('_'). The functions prefixed by an underscore
   are usually functions provided by Microsoft Windows' CRT.

 */

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

#ifndef HAVE_WPUTENV
#ifdef  HAVE__WPUTENV
#define wputenv _wputenv
#else
int wputenv(const char* lpEnv);
#define BUILD_WPUTENV 1
#endif // _wputenv
#endif // wputenv

#ifndef HAVE_WCSCASECMP
#ifdef  HAVE__WCSICMP
#define wcscasecmp _wcsicmp
#else
int wcscasecmp(const wchar_t* lpw1, const wchar_t* lpw2);
#define BUILD_WCSCASECMP 1
#endif //_wcsicmp
#endif // wcscasecmp

#ifndef HAVE_WCSNCASECMP
#ifdef HAVE__WCSNICMP
#define wcsncasecmp _wcsnicmp
#else
int wcsncasecmp(const wchar_t* lpw1, const wchar_t* lpw2, size_t n);
#define BUILD_WCSNCASECMP 1
#endif // _wcsnicmp
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
#ifdef  HAVE__WTOI
#define wtoi _wtoi
#else
#define BUILD_WTOI 1
int wtoi(const wchar_t* lpNum);
#endif // _wtoi
#endif // wtol

#ifndef HAVE_WTOF
#ifdef  HAVE__WTOF
#define wtof _wtof
#else
#define BUILD_WTOF 1
int wtoi(const wchar_t* lpNum);
#endif // _wtof
#endif // _wtof

#if !defined(HAVE_SWPRINTF) || !defined(HAVE_ISO_SWPRINTF)
#if defined(HAVE__VSNWPRINTF)
int libw_swprintf(wchar_t* dest, size_t count, const wchar_t* format, ...);
#define BUILD_SWPRINTF 1
#define swprintf libw_swprintf
#endif // _swprintf
#endif // swprintf

#ifndef HAVE_PUTWS
#ifdef  HAVE__PUTWS
#define putws _putws
#elif defined(HAVE_WPRINTF)
#define putws(str) wprintf("%s\n", str)
#else
int putws(wchar_t* str);
#define BUILD_PUTWS 1
#endif // _putws
#endif // putws

#ifndef HAVE_WCSDUP
#ifdef HAVE__WCSDUP
#define wcsdup _wcsdup
#else
wchar_t* wcsdup(const wchar_t* lpwD);
#define BUILD_WCSDUP
#endif // _wcsdup
#endif // wcsdup

#ifndef HAVE_WRMDIR
#ifdef  HAVE__WRMDIR
#define wrmdir _wrmdir
#else
int wrmdir(const wchar_t* lpwDir);
#define BUILD_WRMDIR 1
#endif // _wrmdir
#endif // wrmdir

#ifndef HAVE_WMKDIR
#ifdef  HAVE__WMKDIR
#define wmkdir _wmkdir
#else
int wmkdir(const wchar_t* lpwDir);
#define BUILD_WMKDIR 1
#endif // _wrmdir
#endif // wrmdir

#ifndef HAVE_WOPEN
#ifdef  HAVE__WOPEN
#define wopen _wopen
#else
int wopen(const wchar_t *pathname, int flags, int mode);
#define BUILD_WOPEN 1
#endif // _wopen
#endif // wopen

#ifndef HAVE_WFDOPEN
#ifdef  HAVE__WFDOPEN
#define wfdopen _wfdopen
#else
int wfdopen(int d, const wchar_t* mode);
#define BUILD_WOPEN 1
#endif // _wfdopen
#endif // wfdopen

#ifndef HAVE_WCHDIR
#ifdef  HAVE__WCHDIR
#define wchdir _wchdir
#else
int wchdir(const wchar_t* dir);
#define BUILD_WCHDIR 1
#endif // _wchdir
#endif // wchdir

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
