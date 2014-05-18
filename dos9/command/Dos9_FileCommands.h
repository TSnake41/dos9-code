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

#ifndef DOS9_FILECOMMANDS_H_INCLUDED
#define DOS9_FILECOMMANDS_H_INCLUDED

#define DOS9_ASK_CONFIRMATION 0x01
#define DOS9_DELETE_READONLY 0x02
#define DOS9_DONT_ASK_GENERIC 0x04

int Dos9_Dir(char* lpLine);
int Dos9_CmdDel(char* lpLine);
int Dos9_CmdMove(char* lpLine);
int Dos9_CmdCopy(char* lpLine);
int Dos9_CmdRen(char* lpLine);
int Dos9_CmdRmdir(char* lpLine);
int Dos9_CmdMkdir(char* lpLine);

#endif // DOS9_FILECOMMANDS_H_INCLUDED
