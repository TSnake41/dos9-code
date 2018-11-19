/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 Romain GARBI
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

enum {
  DOS9_HELP_ECHO = 0,
  DOS9_HELP_SET,
  DOS9_HELP_GOTO,
  DOS9_HELP_IF,
  DOS9_HELP_CD,
  DOS9_HELP_RD,
  DOS9_HELP_MD,
  DOS9_HELP_DEL,
  DOS9_HELP_COPY,
  DOS9_HELP_MOVE,
  DOS9_HELP_CALL,
  DOS9_HELP_FOR,
  DOS9_HELP_SETLOCAL,
  DOS9_HELP_REM,
  DOS9_HELP_DIR,
  DOS9_HELP_CLS,
  DOS9_HELP_TITLE,
  DOS9_HELP_REN,
  DOS9_HELP_TYPE,
  DOS9_HELP_PAUSE,
  DOS9_HELP_COLOR,
  DOS9_HELP_EXIT,
  DOS9_HELP_ALIAS,
  DOS9_HELP_SHIFT,
  DOS9_HELP_FIND,
  DOS9_HELP_MORE,
  DOS9_HELP_BREAK,
  DOS9_HELP_CHCP,
  DOS9_HELP_PUSHD,
  DOS9_HELP_POPD,
  DOS9_HELP_WC,
  DOS9_HELP_XARGS,
  DOS9_HELP_ENDLOCAL,
  DOS9_HELP_PROMPT,
  DOS9_HELP_PECHO,
  DOS9_HELP_TIMEOUT,
  DOS9_HELP_MOD,
  DOS9_HELP_VER,
  DOS9_HELP_ARRAY_SIZE
};


/* this is to instanciate a little embedded command
   help, to provide simple syntax help if no other
   help is availiable */
void Dos9_LoadInternalHelp(void);
void Dos9_ShowInternalHelp(int cmdId);

#endif // DOS9_HELP_H
