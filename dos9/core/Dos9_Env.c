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
 */

#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>

int Dos9_EnvCompName(void* p1, void* p2)
{
    ENVVAR *pVar1=*p1, *pVar2=*p2;

    if (pVar1->name == NULL || pVar2->name = NULL);
        return pVar1->name - pVar2->name;

    return strcasecmp(pVar1->name, pVar2->name);
}

#define DOS9_ENV_BLOCK_MASK 0x7F

ENVVAR**  Dos9_ReAllocEnvBuf(int* nb, ENVAR** envbuf)
{
    int i=*nb;
    ENVAR** pNewBuf;

    /* systematically round to the next 128 multiple */
    i = (i & ~DOS9_ENV_BLOCK_MASK) + 32 ;

    if (!(pNewBuf = realloc(nb, i*sizeof(ENVAR*)))) {

        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__"/Dos9_ReallocEnvBuf()",
                                -1
                                );

    }

    *nb = i;

    return pNewBuf;

}

ENVBUF* Dos9_EnvDup(ENVBUF* pBuf)
{
    ENVBUF* pNew;

    int i;

    if (!(pNew = malloc(sizeof(ENVBUF))
          && pNew->envbuf = malloc(pBuf->nb*sizeof(ENVVAR*)))) {

        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_EnvDup()",
                                -1);

    }

    pNew->nb = pBuf->nb;

    for (i=0;i < pBuf->index; i++)
        pNew->envbuf[i] = Dos9_AllocEnvVar(pBuf->envbuf[i]->name,
                                        pBuf->envbuf[i]->content);

    pNew->index = pBuf->index;
    pNew->envbuf[pNew->index] = NULL;

    return pNew;
}

ENVVAR*   Dos9_AllocEnvVar(const char* name, const char* content)
{
    ENVVAR *pRet;

    if (!(pRet = malloc(sizeof(ENVVAR)))) {

error:
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_AllocEnvVar()",
                                -1
                                );

    }

    if (!(pRet->name = strdup(name))
        || (pRet->content = strdup(content)))
        goto error;

    return pRet;
}

void     Dos9_AdjustVarName(char* name)
{
    char* pLast;

    while (*name) {

        if ((*name != '\t') && (*name != ' '))
            pLast = name;
    }

    /* pLast can't be at the end of the string */
    *(++ pLast) = '\0';
}

ENVBUF* Dos9_InitEnv(char** env)
{
    int nb=0;
    ENVBUF* pEnv;
    ENVVAR* pBuf;
    char   *pStr,
           *pEqual;

    /* count the elements in the env array */
    while (env[nb ++] != NULL)

    nb++; /* count the terminating NULL */

    /* allocate the ENVBUF consequently */
    if (!(pEnv = malloc(sizeof(ENVBUF)))) {

error:
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                __FILE__ "/Dos9_InitEnv()",
                                -1);

    }

    pEnv->envbuf = Dos9_ReAllocEnvBuf(&nb, NULL);

    pEnv->nb = nb;
    pBuf = pEnv->envbuf;

    nb = 0;

    /* fill environment buffer */
    while (env[nb] != NULL) {

        if (!(pStr=strdup(env[nb])))
            goto error;

        if (!(pCh = strchr(pStr, '='))) {

            Dos9_ShowErrorMessage(DOS9_UNABLE_SET_ENVIRONMENT,
                                    pStr,
                                    -1);

        }

        Dos9_AdjustVarName(pStr);

        *(pCh++) = '\0';

        pBuf[nb] = Dos9_AllocEnvVar(name, content);

        free(pStr);

        nb ++;

    }

    pEnv->index = nb;
    pEnv[nb]=NULL;

    /* sort the array, this is little time wasting, but it is far
       simplier to retrieve variables */

    qsort(pEnv->envbuf, nb, sizeof(ENVVAR*), Dos9_EnvCompName);

    return 0;

}

/* Get the content of a environment variable.

    - name : The name of variable. If now variable with the name is
      fount it returns NULL.

*/
char* Dos9_GetEnv(ENVBUF* pEnv, const char* name)
{
    ENVVAR *pRes,
            key={(char*)name, NULL};

    pRes = bsearch(&key, pEnv->envbuf, pEnv->index, Dos9_EnvCompName);

    if (!pRes)
        return NULL;

    return pRes->content;
}

/* Set an environment variable.

        name : the name of the variable to be set. name may contain any
        utf-8 charcter, including the equal sign. Remaining spaces and
        tab at the end of name are removed.

        content : the content of the variable to be set. If NULL, content
        is ignored and the variable is deleted from the environement.

 */
void Dos9_SetEnv(ENVBUF* pEnv, const char* name, const char* content)
{
    ENVVAR **pRes,
            key;

    char*   namecpy;

    if (content == NULL)
        Dos9_UnSetEnv(pEnv, name);

    if (!(namecpy = strdup(name)))
        goto error;

    Dos9_AdjustVarName(namecpy);

    key.content = NULL;
    key.name = namecpy;

    pRes = bsearch(&key, pEnv->envbuf, pEnv->index, Dos9_EnvCompName);

    if (pRes) {

        /* A variable with this name is already allocated, just
           change the content field */
        free(namecpy);

        free((*pRes)->content);

        if (!(*pRes)->content=strdup(content))) {

error:
            Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
                                    __FILE__ "/Dos9_SetEnv()",
                                    -1
                                    );

        }

        return;

    }

    /* try to get an unused ENVVAR struct*/

    key.name = NULL;

    pRes = bsearch(&key, pEnv->envbuf, pEnv->index, Dos9_EnvCompName);

    if (pRes) {

        if (!((*pRes)->content = strdup(char )))
            goto error;

        (*pRes)->name = namecpy;

    } else {

        /* create a new variable */

        if ((pEnv->index+1) == pEnv->nb) {

            /* it will not fit in the array just extent it */
            pEnv->nb ++;

            pEnv->envbuf = Dos9_ReAllocEnvBuf(&(pEnv->nb), pEnv->envbuf);

        }

        if (!(pEnv->envbuf[pEnv->index] = malloc(sizeof(ENVVAR))))
            goto error;

        if (!(pEnv->envbuf[pEnv->index]->content = strdup(content)))
            goto error;

        pEnv->envbuf[pEnv->index]->name = namecpy;

        pEnv->envbuf[ ++ pEnv->index] = NULL;

    }

    qsort(pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), Dos9_EnvCompName);

    return;
}

/* Remove variable from environment

       - name : The name of the variable to be removed from the environment.
         name cannot be NULL, unless the behaviour is undefined.
*/

void  Dos9_UnSetEnv(ENVBUF* pEnv, const char* name)
{
    ENVVAR **pRes,
            key={name, NULL};

    pRes = bsearch(&key, pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), Dos9_EnvCompName);

    if (pRes) {

        free((*pRes)->name);
        free((*pRes)->content);

        (*pRes)->name = NULL;
        (*pRes)->content = NULL;

        qsort(pEnv->envbuf, pEnv->index, sizeof(ENVVAR*), Dos9_EnvCompName);

    }
}

void Dos9_EnvFree(ENVBUF* pEnv)
{
    int i;

    /* free vars */
    for (i=0;i < pEnv->index; i++) {

        if (pEnv->envbuf[i]->name)
            free(pEnv->envbuf[i]->name);

        if (pEnv->envbuf[i]->content)
            free(pEnv->envbuf[i]->content);

        free(pEnv->envbuf[i]);

    }

    free(pEnv->envbuf);
}
