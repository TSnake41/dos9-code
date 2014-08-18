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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libDos9.h>

#include "Dos9_Alias.h"
#include "../core/Dos9_Core.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_ShowHelp.h"

int Dos9_CmdAlias(DOS9CONTEXT* pContext, char* lpLine)
{

    ESTR* lpEsParam=Dos9_EsInit();
    COMMANDLIST* lpclNewCommands;
    COMMANDINFO  ciCommand;

    int iReplace=FALSE;

    COMMANDFLAG iRet;

    void* pVoid;

    char* lpCh;

    lpLine+=5;

    while ((lpCh=Dos9_GetNextParameterEs(pContext, lpLine, lpEsParam))) {

        if (!strcmp("/?", Dos9_EsToChar(lpEsParam))) {

            Dos9_ShowInternalHelp(pContext, DOS9_HELP_ALIAS);

            goto error;

        } else if (!stricmp("/f", Dos9_EsToChar(lpEsParam))) {

            iReplace=TRUE;

        } else {

            Dos9_GetEndOfLine(pContext, lpLine, lpEsParam);
            lpLine=Dos9_EsToChar(lpEsParam);

            lpLine=Dos9_SkipAllBlanks(lpLine);

            break;

        }

        lpLine=lpCh;

    }

    if (!(lpCh=Dos9_SearchChar(lpLine, '='))) {

        Dos9_ShowErrorMessageX(pContext,
                               DOS9_UNEXPECTED_ELEMENT,
                               lpLine);

        goto error;

    }

    Dos9_AdjustVarName(lpLine); /* remove the extra tabs and lines
                                   at the end of line */

    *lpCh='\0';
    lpCh++;

    ciCommand.cfFlag=strlen(lpLine) | DOS9_ALIAS_FLAG;
    ciCommand.ptrCommandName=lpLine;

    ciCommand.lpCommandProc=lpCh;

    iRet=Dos9_GetCommandProc(lpLine, pContext->pCommands, &pVoid);

    if (iReplace && (iRet!=-1)) {

        /* it is possible to reassign Dos9 internal commands. I decided
           to allow this because it may be a funny trick to hack arround. for
           example, if some batch requires some uncompatible features it
           allows to redifine these commands in order to get compatibility */

        if ((Dos9_ReplaceCommand(&ciCommand, pContext->pCommands))) {

            /* if we fail to reasign command, print an error message */

            Dos9_ShowErrorMessageX(pContext,
                                   DOS9_UNABLE_REPLACE_COMMAND,
                                   lpLine
                                  );

            goto error;


        }

        Dos9_EsFree(lpEsParam);

        return 0;

    }

    if (iRet!=-1) {

        /* the command name is not allowed since it is already used */

        Dos9_ShowErrorMessageX(pContext,
                               DOS9_TRY_REDEFINE_COMMAND,
                               lpLine
                              );

        goto error;

    }

    if ((Dos9_AddCommandDynamic(&ciCommand, &(pContext->pCommands)))) {

        Dos9_ShowErrorMessage(pContext,
                              DOS9_UNABLE_ADD_COMMAND,
                              lpLine
                              );

        goto error;

    }

    if (!(lpclNewCommands=Dos9_ReMapCommandInfo(pContext->pCommands))) {

        Dos9_ShowErrorMessageX(pContext,
                               DOS9_UNABLE_REMAP_COMMANDS,
                               __FILE__ "/Dos9_CmdAlias()",
                              );

        goto error;

    }

    Dos9_FreeCommandList(pContext->pCommands);

    pContext->pCommands=lpclNewCommands;

    Dos9_EsFree(lpEsParam);

    return 0;

error:
    Dos9_EsFree(lpEsParam);
    return 0;

}
