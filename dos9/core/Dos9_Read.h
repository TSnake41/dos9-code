#ifndef DOS9_MODULE_READ_H
#define DOS9_MODULE_READ_H

#include <stdio.h>

#include "Dos9_Types.h"

int Dos9_GetLine(ESTR* lpesLine, INPUT_FILE* pIn);
int Dos9_CheckBlocks(ESTR* lpesLine);
void Dos9_RmTrailingNl(char* lpLine);

#endif
