dnl  libw - a wide character compatibility layer for *Nixes.
dnl  Copyright (C) 2010-2014 DarkBatcher
dnl
dnl  This program is free software: you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation, either version 3 of the License, or
dnl  (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
AC_INIT([libw],
	[2014.0.1],
	[darkbatcher@dos9.org],
	[libw],
	[http://www.dos9.org/])

AC_PREREQ([2.59])

AC_LANG_SOURCE(C)

dnl Adding configuration header
AC_CONFIG_HEADERS(lib/libw_conf.h)

dnl Checking for programs
AC_PROG_CC
AC_LANG_C
AC_PROG_MAKE_SET
AC_HEADER_STDC

dnl Looking for functions that are availiable in the current environement
AC_CHECK_FUNCS(wcserror _wcserror wstat _wstat wgetenv _wgetenv wcscasecmp)
AC_CHECK_FUNCS(_wcsicmp wfopen _wfopen wtol _wtol wtoi _wtoi swprintf)
AC_CHECK_FUNCS(_swprintf wcsncasecmp _wcsnicmp putws)
AC_CHECK_FUNCS(_putws wprintf wcsdup _wcsdup wputenv _wputenv wrmdir _wrmdir)
AC_CHECK_FUNCS(wmkdir _wmkdir wtof _wtof wfdopen _wfdopen wopen _wopen)
AC_CHECK_FUNCS(wchdir _wchdir _vsnwprintf)

dnl Check if the system is supporting C99 version of swprintf (this is an
dnl issue on windows, where the function may not be compliant)

CFLAGS_tmp=$CFLAGS
CFLAGS='$CFLAGS -Werror -Wfatal-errors'
conform=no

AC_MSG_CHECKING([wether swprintf is ISO-C 99 conformant])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>]
[#include <stdlib.h>]
[#include <wchar.h>]],[[char buf[FILENAME_MAX];]
[swprintf(buf, FILENAME_MAX, "test %s", "test2");]])], [AC_DEFINE([HAVE_ISO_SWPRINTF], [1], [Define if C99 swprintf available])
conform=yes])

AC_MSG_RESULT([$conform])

CFLAGS=$CFLAGS_tmp

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
