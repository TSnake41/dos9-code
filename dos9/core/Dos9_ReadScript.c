/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2017 Romain GARBI
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
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifndef WIN32
#include <sys/stat.h>
#endif

#include <libDos9.h>

#include "../../config.h"

#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
#include <libcu8.h>
#endif

#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

/* load a batch script */
int Dos9_OpenBatchScript(struct batch_script_t* restrict script,
                                char* restrict name)
{
    strncpy(script->name, name, sizeof(script->name));
    script->name[sizeof(script->name)-1] = '\0';
#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
    script->wname = NULL;
#endif // defined

    return Dos9_LoadBatchScript(script);
}

void Dos9_DumpBatchScript(struct batch_script_t* script)
{
    struct cmdlines_t* cmd = script->cmds;
    struct labels_t* lbl = script->lbls;

    fprintf(stderr, "<style>table {border: 1px solid gray;}\n"
                    "td {border: 1px solid black;}</style>"
                    "<p>Dumping batch script <bold>%s</bold> at address <bold>%p</bold></p>\n"
                    "<table><caption>Labels entries parsed</caption>\n"
                    "<tr><th>Address</th><th>Label</th><th>Maps To</th><th>Next</th></tr>\n", script->name, script);

    while (lbl) {

        fprintf(stderr, "<tr><td><div id=\"%p\">%p</div></td><td><pre>%s</pre></td><td><a href=\"#%p\">%p</a></td><td><a href=\"#%p\">%p</a></td></tr>\n",
                        lbl, lbl, lbl->label, lbl->following,lbl->following, lbl->next, lbl->next);

        lbl = lbl->next;

    }

    fprintf(stderr, "</table><table><caption>Command line entries parsed</caption>\n"
                    "<tr><th>Address</th><th>Command line</th><th>Next</th></tr>");

    while (cmd) {

        fprintf(stderr, "<tr><td><a href=\"#%p\">%p</a></td><td><pre>%s</pre></td><td><a href=\"#%p\">%p</a></td></tr>\n",
                        cmd, cmd, cmd->line, cmd->next, cmd->next);
        cmd = cmd->next;

    }

    fprintf(stderr, "</table>\n");
}

#ifdef WIN32
__inline__ size_t Dos9_GetScriptSize(struct batch_script_t* script, int* restrict err)
{
    WIN32_FILE_ATTRIBUTE_DATA attr;

    *err = 0;

#ifdef DOS9_USE_LIBCU8
    size_t cvt;

    if (script->wname == NULL
        && !(script->wname = (wchar_t*) libcu8_xconvert(LIBCU8_TO_U16, script->name,
                                    strlen(script->name)+1, &cvt))) {
        *err = 1;
        return 0;
    }

    if (!GetFileAttributesExW(script->wname, GetFileExInfoStandard, &attr)){
        *err = 1;
        return 0;
    }


#else

    if (!GetFileAttributesExA(script->name, GetFileExInfoStandard, &attr)) {
        *err = 1;
        return 0;
    }
#endif /* DOS9_USE_LIBCU8 */

    return attr.nFileSizeLow;
}
#else
__inline__ size_t Dos9_GetScriptSize(struct batch_script_t* script, int* restrict err)
{
    struct stat st;

    if (stat(script->name, &st) == 0)
        return st.st_size;

    *err = 1;
    return 0;
}
#endif

__inline__ char* xstrdup(const char* str)
{
    char* new;

    if ((new = strdup(str)) == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/xstrdup",
                                        -1);

    return new;
}

/* reload a batch script */
int Dos9_LoadBatchScript(struct batch_script_t* script)
{
    FILE* file;
    ESTR *line = Dos9_EsInit(),
         *cmd = Dos9_EsInit();
    struct cmdlines_t* newcmd;
    struct labels_t* newlbl, *last=NULL;
    int eof=0, nb=0, checkres=0;
    char *pch;

    #ifdef WIN32
    HANDLE h;
    #endif

    /* Initialize all fields that will be used for parsing */
    script->curr = NULL;
    script->lbls = NULL;
    script->cmds = NULL;
#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
    script->wname = NULL;
#endif /* WIN32 && DOS9_USE_LIBCU8 */
    script->cpy = 0;

    if ((file = fopen(script->name, "r")) == NULL)
        goto error;

    script->size = Dos9_GetScriptSize(script, &checkres);

    if (checkres)
        goto error;

    while (!eof && !(eof=Dos9_EsGet(line, file))) {

        pch = Dos9_SkipBlanks(line->str);

        nb ++; /* increment line number */

        if (*pch == ':') {

            if (*(pch+1) == ':')
                continue; /* This line is a comment get rid of it*/

            /* This line is a label */

            if ((newlbl = malloc(sizeof(struct labels_t))) == NULL)
                Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                      | DOS9_PRINT_C_ERROR,
                                      __FILE__ "/Dos9_LoadBatchScript", -1);

            if (last == NULL) {

                script->lbls = newlbl;

            } else {

                last->next = newlbl;

            }

            last = newlbl;
            newlbl->next = NULL;
            newlbl->following = script->curr;

            Dos9_RmTrailingNl(line->str);

            /* Remove the useless delims set at the end of the label */
            Dos9_StripEndDelims(pch);
            newlbl->label = xstrdup(pch);

            continue;

        } else if (*pch == '\0') {

            /* This is a blank line, or a comment, skip it */
            continue;

        }

        /* the read line should *definitely* end with a '\n' (in
           in fact it always do, except when reaching end of file) */
        if (strchr(pch, '\n') == NULL)
            Dos9_EsCat(line, "\n");

        /* This is a command */
        Dos9_EsCat(cmd, pch);

        /* We reached a line end since blocks appear to be complete.
           Allocate a new cmdlines_t instance to store the line */

        *(line->str) = '\0'; /* clear line */

        if ((newcmd = malloc(sizeof(struct cmdlines_t))) == NULL)
            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION
                                  | DOS9_PRINT_C_ERROR,
                                        __FILE__ "/Dos9_LoadBatchScript", -1);

        newcmd->line = xstrdup(cmd->str);
        newcmd->next = NULL;

        if (script->curr) /* add the new line to the linked list */
            script->curr->next = newcmd;

        script->curr = newcmd; /* change the current command */

        if (script->cmds == NULL) /* set the first command */
            script->cmds = newcmd;

        *(cmd->str) = '\0';


    }

    script->curr = script->cmds;

    Dos9_EsFree(cmd);
    Dos9_EsFree(line);
    fclose(file);

    /* Dos9_DumpBatchScript(script); */
    return 0;

error:
    if (file)
        fclose(file);

    Dos9_EsFree(line);
    Dos9_EsFree(cmd);
    return -1;
}

int Dos9_ReloadBatchScript(struct batch_script_t* script)
{
    struct cmdlines_t* cmd = script->cmds;
    int i = 0;

    /* try to get the line number of the current line */
    while (cmd && cmd != script->curr) {

        cmd = cmd->next;
        i++;

    }

    if (! script->cpy) {

        /* if this is not a copy, free data structs */
        Dos9_FreeCmdlines(script->cmds);
        Dos9_FreeLabels(script->lbls);

    }

    if (Dos9_LoadBatchScript(script) == -1)
        return -1;

    cmd = script->cmds;

    /* place the current line where it should be */
    while (cmd && (i --))
        cmd = cmd->next;

    /* if there is not enough lines in the new script,
       restart from the beginning */
    script->curr = cmd ? (cmd) : (script->cmds);

    return 0;
}

/* Check batch script changes */
int Dos9_CheckBatchScriptChanges(struct batch_script_t* script)
{
    int ret, err;

    ret = (script->size != Dos9_GetScriptSize(script, &err));

    /* if error, say it doesn't changed */
    if (err)
        return 0;

    return ret;
}

void Dos9_FreeBatchScript(struct batch_script_t* script)
{
    /* Do not free if it a copy */
    if (script->cpy)
        return;

#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
    if (script->wname)
        free(script->wname);
#endif
    *(script->name) = '\0';
    Dos9_FreeLabels(script->lbls);
    Dos9_FreeCmdlines(script->cmds);
}

void Dos9_CopyBatchScript(struct batch_script_t* restrict recv,
                                    struct batch_script_t* restrict orig)
{
    memcpy(recv, orig, sizeof(struct batch_script_t));

    recv->cpy = 1; /* mark recv as a copy */
}

void Dos9_FreeLabels(struct labels_t* labels)
{
    struct labels_t* tmp;

    while (labels) {

        tmp = labels->next;
        free(labels->label);
        free(labels);
        labels = tmp;

    }
}

void Dos9_FreeCmdlines(struct cmdlines_t* cmdlines)
{
    struct cmdlines_t* tmp;

    while (cmdlines) {

        tmp = cmdlines->next;
        free(cmdlines->line);
        free(cmdlines);
        cmdlines = tmp;

    }
}
