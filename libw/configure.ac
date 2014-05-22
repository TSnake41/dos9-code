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

dnl Adding configuration header
AC_CONFIG_HEADERS(lib/libw_conf.h)

dnl Checking for programs
AC_PROG_CC
AC_LANG_C
AC_PROG_MAKE_SET
AC_HEADER_STDC

dnl Looking for functions that are availiable in the current environement
AC_CHECK_FUNCS(wcserror _wcserror wstat _wstat wgetenv _wgetenv wcscasecmp _wcsicmp wfopen _wfopen wtol _wtol wtoi _wtoi swprintf _swprintf snwprintf _snwprintf)
AC_CHECK_FUNCS(wcsncasecmp _wcsnicmp putws _putws wprintf wcsdup _wcsdup wputenv _wputenv wrmdir _wrmdir wmkdir _wmkdir wtof _wtof wfdopen _wfdopen)
AC_CHECK_FUNCS(wopen _wopen wchdir _wchdir)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
