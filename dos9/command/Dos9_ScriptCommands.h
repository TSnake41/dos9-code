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

#ifndef DOS9_SCRIPTCOMMAND_H
#define DOS9_SCRIPTCOMMAND_H

#include <wchar.h>

int Dos9_CmdEcho(wchar_t* lpLine);
int Dos9_CmdPause(wchar_t* lpLine);

int Dos9_CmdSet(wchar_t* lpLine);
int Dos9_CmdSetS(wchar_t* lpLine);
int Dos9_CmdSetP(wchar_t* lpLine);
int Dos9_CmdSetA(wchar_t* lpLine, int bFloats);
int Dos9_CmdSetEvalFloat(ESTR* lpExpression);
int Dos9_CmdSetEvalInt(ESTR* lpExpression);

int Dos9_CmdBlock(wchar_t* lpLine);

int Dos9_CmdExit(wchar_t* lpLine);
int Dos9_CmdSetLocal(wchar_t* lpLine);
int Dos9_CmdHelp(wchar_t* lpLine);

int Dos9_CmdRem(wchar_t* lpLine);
int Dos9_CmdTitle(wchar_t* lpLine);
int Dos9_CmdCls(wchar_t* lpLine);

int Dos9_CmdColor(wchar_t* lpLine);
int Dos9_CmdGoto(wchar_t* lpLine);

int Dos9_CmdType(wchar_t* lpLine);

int Dos9_CmdCd(wchar_t* lpLine);

int Dos9_CmdDir(wchar_t* lpLine);

int Dos9_CmdShift(wchar_t* lpLine);

#endif
