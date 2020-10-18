#include <string.h>

#include "core/Dos9_Run.h"

#if __EMSCRIPTEN__

int Dos9_Emscripten_RunLine(char *line)
{
  ESTR estr = {
    .str = line,
    .len = strlen(line)
  };

  return Dos9_RunLine(&estr);
}

#endif
