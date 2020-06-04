/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2018 Astie Teddy
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

#ifndef DOS9_DIRSTACK_H
#define DOS9_DIRSTACK_H

#include <stdbool.h>
#include <stddef.h>

struct dirstack_t {
  char **paths;
  size_t count;
};

bool Dos9_DirStackInit(void);
void Dos9_DirStackFree(void);

bool Dos9_PushDir(const char *dir);
char *Dos9_PopDir(void); /* You need to free() the output directory path. */

bool Dos9_DirStackCopy(struct dirstack_t *dest);

#endif /* DOS9_DIRSTACK_H */
