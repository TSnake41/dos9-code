#include "../../LibDos9.h"

#ifndef WIN32

void Dos9_ClearConsoleScreen(void)
{
    wprintf(L"\033[H\033[2J");
}

void Dos9_SetConsoleColor(COLOR cColor)
{
    Dos9_SetConsoleTextColor(cColor);
    wprintf(L"\033[H\033[2J");
}

void Dos9_SetConsoleTextColor(COLOR cColor)
{
    if (cColor & DOS9_FOREGROUND_INT) {

        /* set foreground intensity */
        wprintf(L"\033[1m");
        cColor^=DOS9_FOREGROUND_INT;

    } else {

        wprintf(L"\033[0m");

    }

    if (cColor & DOS9_BACKGROUND_DEFAULT)
        wprintf(L"\033[49m");

    if (cColor & DOS9_FOREGROUND_DEFAULT)
        wprintf(L"\033[39m");

    if ((cColor & DOS9_FOREGROUND_DEFAULT)
        && (cColor & DOS9_BACKGROUND_DEFAULT))
            return;

    if (!(cColor & DOS9_BACKGROUND_DEFAULT))
        wprintf(L"\033[%dm",DOS9_GET_BACKGROUND_(cColor)+40);

    if (!(cColor & DOS9_FOREGROUND_DEFAULT))
        wprintf(L"\033[%dm", DOS9_GET_FOREGROUND_(cColor)+30);

}

LIBDOS9 void Dos9_SetConsoleTitle(wchar_t* lpTitle)
{
    wprintf(L"\033]0;%s\007", lpTitle);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord)
{
    wprintf(L"\033[%d;%dH", iCoord.Y+1, iCoord.X+1);
}


LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void)
{
    return (CONSOLECOORD){0,0};
}

LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize )
{
    if (bVisible) {
        wprintf(L"\033[25h");
    } else {
        wprintf(L"\033[25l");
    }
}

#endif
