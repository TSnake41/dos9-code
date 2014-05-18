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

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "libw.h"

LIBW char* libw_wcstombs(const wchar_t* lpIn)
{
	char* lpBuf;
	size_t size;

	size=wcstombs(NULL, lpIn, 0);
		/* get the requiered length for translation */

	if (!(lpBuf=malloc(size)))
		return NULL;

	if (wcstombs(lpBuf, lpIn, size)== (size_t)-1) {

		/* error occured while translation */

		free(lpBuf);
		return 0;

	}

	return lpBuf;

}

LIBW wchar_t* libw_mbstowcs(const char* lpIn)
{
	wchar_t* lpBuf;
	size_t size;

	size=mbstowcs(NULL, lpIn, 0);
		/* get the requiered length for translation */

	if (!(lpBuf=malloc(size*sizeof(wchar_t))))
		return NULL;

	if (mbstowcs(lpBuf, lpIn, size)== (size_t)-1) {

		/* error occured while translation */

		free(lpBuf);
		return 0;

	}

	return lpBuf;
}
