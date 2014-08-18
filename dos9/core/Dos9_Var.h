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
#ifndef DOS9_VAR_H
#define DOS9_VAR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <libDos9.h>

#include "Dos9_Core.h"

#define LOCAL_VAR_BLOCK_SIZE 128

int Dos9_setenv(const char* name, const char* content);

#define DOS9_ALL_PATH    0
#define DOS9_DISK_LETTER 1
#define DOS9_PATH_PART   2
#define DOS9_FILENAME    3
#define DOS9_EXTENSION   4
#define DOS9_FILESIZE    5
#define DOS9_TIME        6
#define DOS9_ATTR        7

#define DOS9_VAR_MAX_OPTION 16

#define Dos9_GetLocalBlock() (LOCAL_VAR_BLOCK*)calloc(LOCAL_VAR_BLOCK_SIZE, sizeof(LOCAL_VAR_BLOCK))
#define Dos9_FreeLocalBlock(lpBlock) free(lpBlock)


typedef char* LOCAL_VAR_BLOCK;

int              Dos9_InitVar(DOS9CONTEXT* pContext, char* lpArray[]);

int              Dos9_SetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char cVarName, char* cVarContent);

char*            Dos9_GetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char* lpName, ESTR* lpRecieve);
int              Dos9_GetVar(DOS9CONTEXT* pContext, char* lpName, ESTR* lpRecieve);
char*            Dos9_GetLocalVarPointer(LOCAL_VAR_BLOCK* lpvBlock, char cVarName);

#endif
