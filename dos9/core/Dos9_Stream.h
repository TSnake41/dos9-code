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

#ifndef DOS9_STREAM_H
#define DOS9_STREAM_H

#include <libDos9.h>
#include "Dos9_Core.h"

/* The aim of this new implementation is to get to be able to run several Dos9
   threads at the same time without conflicts of stream redirection.

    * To handle this, descriptors are only redirected when external executables are
    run, otherwise, output and input are dealt with internally.

   This implementation must support the following redirection types :

        command [n]> file

        which redirect the n file descriptor in the file.

        command [n]>&[m]

        which redirects the n file descriptor into the m descriptor

    Assuming a STREAMSTACK struct named stack, stdin, stdout and stderr can
    be retrieved through stack.in, stack.out, stack.err.

    Note that, however, the current implement can not handle more than 10
    non-trivial redirection simultaneously.

 */

#define STREAMSTACK_REDIRECT_FILE (0)
#define STREAMSTACK_REDIRECT_FD   (1)

/* pushes or pop the stream stack */
STREAMSTACK* Dos9_PushStreamStack(STREAMSTACK* lpssStreamStack);
STREAMSTACK* Dos9_PopStreamStack(STREAMSTACK* lpssStreamStack);

/* open output redirection */
int     Dos9_OpenOutput(STREAMSTACK* lpssStreamStack,
                            PARSED_STREAM_START* lpssStart);

#define Dos9_SetStreamStackLockState(stack, state) (stack->lock = state)
#define Dos9_GetStreamStackLockState(stack) (stack->lock)

/* Setup aditionnal redirections */
int     Dos9_RedirectStreams(STREAMSTACK* lpssStack);

/* Setup a pipe. That is, pushes two redirection level :

   * The first one redirect stdin into the input of the pipe
   * The second on redirects  the output of pipe into stdin

 Use Dos9_PopStreamStack to pop the levels of redirections and
 so execute the pipe

 */
int     Dos9_OpenPipe(STREAMSTACK* lpssStreamStack);

#endif // DOS9_STREAM_H
