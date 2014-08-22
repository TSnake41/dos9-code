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

#ifndef DOS9_HELP_H

#include "../core/Dos9_Types.h"

#define DOS9_HELP_ECHO      0
#define DOS9_HELP_SET       1
#define DOS9_HELP_GOTO      2
#define DOS9_HELP_IF        3
#define DOS9_HELP_CD        4
#define DOS9_HELP_RD        5
#define DOS9_HELP_MD        6
#define DOS9_HELP_DEL       7
#define DOS9_HELP_COPY      8
#define DOS9_HELP_MOVE      9
#define DOS9_HELP_CALL      10
#define DOS9_HELP_FOR       11
#define DOS9_HELP_SETLOCAL  12
#define DOS9_HELP_REM       13
#define DOS9_HELP_DIR       14
#define DOS9_HELP_CLS       16
#define DOS9_HELP_TITLE     17
#define DOS9_HELP_REN       18
#define DOS9_HELP_TYPE      19
#define DOS9_HELP_PAUSE     20
#define DOS9_HELP_COLOR     21
#define DOS9_HELP_EXIT      22
#define DOS9_HELP_ALIAS     23
#define DOS9_HELP_SHIFT     24

#define DOS9_HELP_ARRAY_SIZE 25

/* this is to instanciate a little embedded command
   help, to provide simple syntax help if no other
   help is availiable */
void Dos9_LoadInternalHelp(void);
void Dos9_ShowInternalHelp(DOS9CONTEXT* pContext, int cmdId);

#endif // DOS9_HELP_H
