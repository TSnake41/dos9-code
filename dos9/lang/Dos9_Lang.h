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

#ifndef DOS9_LANG_INCLUDED_H
#define DOS9_LANG_INCLUDED_H

#include <wchar.h>

extern const wchar_t* lpMsgEchoOn;
extern const wchar_t* lpMsgEchoOff;
extern const wchar_t* lpMsgPause;

extern const wchar_t* lpDirNoFileFound;
extern const wchar_t* lpDirListTitle;
extern const wchar_t* lpDirFileDirNb;


extern const wchar_t* lpHlpMain;
extern const wchar_t* lpHlpDeprecated;

extern const wchar_t* lpDelConfirm;
extern const wchar_t* lpDelChoices;

extern const wchar_t* lpAskYn;
extern const wchar_t* lpAskyN;
extern const wchar_t* lpAskyn;

extern const wchar_t* lpAskYna;
extern const wchar_t* lpAskyNa;
extern const wchar_t* lpAskynA;
extern const wchar_t* lpAskyna;

extern const wchar_t* lpAskYes;
extern const wchar_t* lpAskYesA;

extern const wchar_t* lpAskNo;
extern const wchar_t* lpAskNoA;

extern const wchar_t* lpAskAll;
extern const wchar_t* lpAskAllA;

extern const wchar_t* lpAskInvalid;


void Dos9_LoadStrings(void); /* a function that loads differents
                               languages traductions */

#endif
