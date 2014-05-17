/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef DOS9_SPLITPATH_H

#ifdef _POSIX_C_SOURCE

#define _MAX_DRIVE FILENAME_MAX
#define _MAX_DIR FILENAME_MAX
#define _MAX_FNAME FILENAME_MAX
#define _MAX_EXT FILENAME_MAX

#endif

#include <wchar.h>

void Dos9_SplitPath(wchar_t* lpPath, wchar_t* lpDisk, wchar_t* lpDir, wchar_t* lpName, wchar_t* lpExt);

#endif // DOS9_SPLITPATH_H
