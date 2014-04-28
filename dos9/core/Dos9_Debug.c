#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>

#include "Dos9_Debug.h"

void Dos9_ShowDebug(const wchar_t* format,...)
{
	va_list vaArgs;

	va_start(vaArgs, format);
	vfwprintf(stderr, format, vaArgs);

	va_end(vaArgs);
}
