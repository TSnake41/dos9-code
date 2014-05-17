#ifndef DOS9_MODULE_READ_H
#define DOS9_MODULE_READ_H

#include <stdio.h>
#include <wchar.h>
#include <libDos9.h>

typedef struct INPUT_FILE {
	wchar_t  lpFileName[FILENAME_MAX];
	int   iPos;
	int   bEof;
} INPUT_FILE;

int Dos9_GetLine(ESTR* lpesLine, INPUT_FILE* pIn);
int Dos9_CheckBlocks(ESTR* lpesLine);
void Dos9_RmTrailingNl(wchar_t* lpLine);

#endif
