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

#ifndef DOS9_COMMANDINFO_H
#define DOS9_COMMANDINFO_H

#include <wchar.h>

#include "Dos9_ScriptCommands.h"
#include "Dos9_FileCommands.h"
#include "Dos9_Conditions.h"
#include "Dos9_For.h"
#include "Dos9_Call.h"
#include "Dos9_Alias.h"

#include "../core/Dos9_Core.h"

#define STRLEN(str) (sizeof(str)/sizeof(wchar_t)-1)


/* the flag paramater detemines wether a space should be
   searched */

COMMANDINFO lpCmdInfo[]= {
	{L"ECHO", Dos9_CmdEcho, 0},
	{L"EXIT", Dos9_CmdExit, STRLEN(L"EXIT")},
	{L"PAUSE", Dos9_CmdPause, STRLEN(L"PAUSE")},
	{L"SET", Dos9_CmdSet, STRLEN(L"SET")},
	{L"SETLOCAL", Dos9_CmdSetLocal, STRLEN(L"SETLOCAL")},
	{L"HELP", "hlp.bat", STRLEN(L"HELP") | DOS9_ALIAS_FLAG},
	{L"REM", Dos9_CmdRem, STRLEN(L"REM")},
	{L"CLS", Dos9_CmdCls, STRLEN(L"CLS")},
	{L"COLOR", Dos9_CmdColor, STRLEN(L"COLOR")},
	{L"TITLE", Dos9_CmdTitle, STRLEN(L"TITLE")},
	{L"TYPE", Dos9_CmdType, STRLEN(L"TYPE")},
	{L"MORE", Dos9_CmdType, STRLEN(L"MORE")},
	{L"GOTO", Dos9_CmdGoto, STRLEN(L"GOTO")},
	{L"GOTO:", Dos9_CmdGoto, 0},
	{L"CD", Dos9_CmdCd, STRLEN(L"CD")},
	{L"CHDIR", Dos9_CmdCd, STRLEN(L"CHDIR")},
	{L"DIR", Dos9_CmdDir, STRLEN(L"DIR")},
	{L"IF", Dos9_CmdIf, STRLEN(L"IF")},
	{L"DEL", Dos9_CmdDel, STRLEN(L"DEL")},
	{L"ERASE", Dos9_CmdDel, STRLEN(L"ERASE")},
	{L"REN", Dos9_CmdRen, STRLEN(L"REN")},
	{L"RENAME", Dos9_CmdRen, STRLEN(L"RENAME")},
	{L"MOVE", Dos9_CmdMove, STRLEN(L"MOVE")},
	{L"MD", Dos9_CmdMkdir, STRLEN(L"MD")},
	{L"MKDIR", Dos9_CmdMkdir, STRLEN(L"MKDIR")},
	{L"RD", Dos9_CmdRmdir, STRLEN(L"RD")},
	{L"RMDIR", Dos9_CmdRmdir, STRLEN(L"RMDIR")},
	{L"COPY", Dos9_CmdCopy, STRLEN(L"COPY")},
	{L"FOR", Dos9_CmdFor, STRLEN(L"FOR")},
	{L"(", Dos9_CmdBlock, 0},
	{L"CALL", Dos9_CmdCall, STRLEN(L"CALL")},
	{L"CALL:", Dos9_CmdCall, 0},
	{L"ALIAS", Dos9_CmdAlias, STRLEN(L"ALIAS")},
	{L"SHIFT", Dos9_CmdShift, STRLEN(L"SHIFT")}
};

#endif
