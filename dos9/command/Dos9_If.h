/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2013 DarkBatcher
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

#ifndef DOS9_CMD_IF_H
#define DOS9_CMD_IF_H

#define DOS9_IF_CASE_UNSENSITIVE 1
#define DOS9_IF_NEGATION 2
#define DOS9_IF_EXIST 4
#define DOS9_IF_ERRORLEVEL 8
#define DOS9_IF_DEFINED 16

#define DOS9_FLOAT_EQUAL_PRECISION 0.0000000000001

#define CMPTYPE int
#define CMP_EQUAL 1
#define CMP_DIFFERENT 2
#define CMP_GREATER 3
#define CMP_GREATER_OR_EQUAL 4
#define CMP_LESSER 5
#define CMP_LESSER_OR_EQUAL 6
#define CMP_FLOAT_COMP 0x10

int Dos9_PerformExtendedTest(const char* lpCmp, const char* lpParam1,
                                    const char* lpParam2, int iFlag);

int Dos9_CmdIf(DOS9CONTEXT* pContext, char* lpParam); // function to support if

#endif // DOS9_CMD_CONDITION_H
