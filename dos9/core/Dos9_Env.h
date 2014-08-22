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
#ifndef DOS9_ENV_H
#define DOS9_ENV_H

#include "Dos9_Types.h"

/*  Dos9 also provide functions for environment variables, to be able to
    run various threads */

ENVVAR**  Dos9_ReAllocEnvBuf(int* nb, ENVVAR* envbuf);
ENVVAR*   Dos9_AllocEnvVar(const char* name, const char* content);

void     Dos9_AdjustVarName(char* name);

ENVBUF* Dos9_InitEnv(char** env);
ENVBUF* Dos9_EnvDup(ENVBUF* pBuf);

char* Dos9_GetEnv(ENVBUF* pEnv, const char* name);
void  Dos9_SetEnv(ENVBUF* pEnv, const char* name, const char* content);
void  Dos9_UnSetEnv(ENVBUF* pEnv, const char* name);

void Dos9_ExportEnv(ENVBUF* pEnv, const char* name, const char* content);

void Dos9_EnvFree(ENVBUF* pEnv);
#endif
