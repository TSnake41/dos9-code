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

#ifndef DOS9_CMD_SET_H
#define DOS9_CMD_SET_H

#include <libDos9.h>

int Dos9_CmdSet(DOS9CONTEXT* pContext, char *lpLine);
int Dos9_CmdSetS(DOS9CONTEXT* pContext, char* lpLine);
int Dos9_CmdSetP(DOS9CONTEXT* pContext, char* lpLine);
int Dos9_CmdSetA(DOS9CONTEXT* pContext, char* lpLine, int bFloats);
int Dos9_CmdSetEvalFloat(DOS9CONTEXT* pContext, ESTR* lpExpression);
int Dos9_CmdSetEvalInt(DOS9CONTEXT* pContext, ESTR* lpExpression);

#endif // DOS9_CMD_ECHO_H
