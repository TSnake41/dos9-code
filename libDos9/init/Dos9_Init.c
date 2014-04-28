#include "../LibDos9.h"

LIBDOS9 int Dos9_LibInit(void)
{
    /* instanciation routine */

    if (_Dos9_Estr_Init() != 0)
        return -1;

    if (_Dos9_Thread_Init() != 0)
        return -1;

    return 0;

}

LIBDOS9 void Dos9_LibClose(void)
{

    _Dos9_Estr_Close();
    _Dos9_Thread_Close();

}
