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
 */

#ifndef DOS9_EXPAND_H
#define DOS9_EXPAND_H

#include <libDos9.h>
#include "Dos9_Core.h"

#define Dos9_ReplaceVars(lpEsStr) Dos9_ExpandVar(lpEsStr, '%')

void Dos9_ExpandSpecialVar(DOS9CONTEXT* pContext, ESTR* ptrCommandLine);
void Dos9_ExpandVar(DOS9CONTEXT* pContext, ESTR* ptrCommandLine, char cDelimiter);
void Dos9_DelayedExpand(DOS9CONTEXT* pContext, ESTR* ptrCommandLine, char cEnableDelayedExpansion);

void  Dos9_RemoveEscapeChar(char* lpLine);
char* Dos9_StrToken(char* lpString, char cToken);

#endif
