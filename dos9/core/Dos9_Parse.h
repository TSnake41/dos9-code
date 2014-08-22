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

#ifndef DOS9_PARSE_H
#define DOS9_PARSE_H

#include <libDos9.h>
#include "Dos9_Core.h"

/* A structure to store command line
 * These information are stored in two different ways:
 * - Normal line : part of a normal line
 * - Blocks of commands : blocks of commands defined by using braces
 */

#define PARSED_STREAM_NODE_NONE 0x00
#define PARSED_STREAM_NODE_YES 0x01
#define PARSED_STREAM_NODE_NOT 0x02
#define PARSED_STREAM_NODE_PIPE 0x03
#define PARSED_STREAM_NODE_RESET -1

#define PSTREAMSTART_TRUNC_IN   (0x01) /* truncate in (unused) */
#define PSTREAMSTART_TRUNC_OUT  (0x02) /* truncate out */
#define PSTREAMSTART_TRUNC_ERR  (0x04) /* truncate err */
#define PSTREAMSTART_REDIR_ERR  (0x08) /* redirect err in out */
#define PSTREAMSTART_REDIR_OUT  (0x10) /* redirect out in err */


PARSED_STREAM_START* Dos9_ParseLine(DOS9CONTEXT* pContext, ESTR* lpLine);

PARSED_STREAM_START* Dos9_ParseOutput(ESTR* lpesLine);
PARSED_STREAM*       Dos9_ParseOperators(DOS9CONTEXT* pContext, ESTR* lpesLine);

PARSED_STREAM_START* Dos9_AllocParsedStreamStart(void);
PARSED_STREAM*       Dos9_AllocParsedStream(PARSED_STREAM* lppsStream);

void    Dos9_FreeParsedStream(PARSED_STREAM* lppsStream);
void    Dos9_FreeParsedStreamStart(PARSED_STREAM_START* lppssStart);

#define Dos9_FreeLine(lpssStreamStart) \
            Dos9_FreeParsedStreamStart(lpssStreamStart)

int     Dos9_GetDescriptor(const char* current, const char* begin);

#endif // DOS9_PARSE_H
