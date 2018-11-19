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

#ifndef DOS9_MESSAGES_H
#define DOS9_MESSAGES_H



#ifdef DOS9_DEBUG_MODE

#define DEBUG(Msg,...) fprintf(stderr, __FILE__ "\t" __func__ ":%d\n\t" Msg "\n", __LINE__, ##__VA_ARGS__)
#define DEBUG_(a) fprintf( stderr, "$ %s (line %d) * %d\n", __func__, __LINE__, a)
#define DOS9_DEBUG(a) fprintf( stderr, "$ %s (line %d) * %s\n", __func__, __LINE__, a)

#else

#define DEBUG(Msg, ...)
#define DEBUG_(a)
#define DOS9_DEBUG(a)

#endif

void Dos9_ShowErrorMessage(unsigned int iErrorNumber, const char* lpComplement, int iExitCode);
void Dos9_LoadErrors(void);

enum {
  DOS9_NO_ERROR = 0,
  DOS9_FILE_ERROR,
  DOS9_DIRECTORY_ERROR,
  DOS9_COMMAND_ERROR,
  DOS9_UNEXPECTED_ELEMENT,
  DOS9_BAD_COMMAND_LINE,
  DOS9_LABEL_ERROR,
  DOS9_EXTENSION_DISABLED_ERROR,
  DOS9_EXPECTED_MORE,
  DOS9_INCOMPATIBLE_ARGS,
  DOS9_UNABLE_RENAME,
  DOS9_MATH_OUT_OF_RANGE,
  DOS9_MATH_DIVIDE_BY_0,
  DOS9_MKDIR_ERROR,
  DOS9_RMDIR_ERROR,
  DOS9_STREAM_MODULE_ERROR,
  DOS9_SPECIAL_VAR_NON_ASCII,
  DOS9_ARGUMENT_NOT_BLOCK,
  DOS9_FOR_BAD_TOKEN_SPECIFIER,
  DOS9_FOR_TOKEN_OVERFLOW,
  DOS9_FOR_USEBACKQ_VIOLATION,
  DOS9_FAILED_ALLOCATION,
  DOS9_CREATE_PIPE,
  DOS9_FOR_LAUNCH_ERROR,
  DOS9_FOR_BAD_INPUT_SPECIFIER,
  DOS9_FOR_TRY_REASSIGN_VAR,
  DOS9_INVALID_EXPRESSION,
  DOS9_INVALID_TOP_BLOCK,
  DOS9_UNABLE_DUPLICATE_FD,
  DOS9_UNABLE_CREATE_PIPE,
  DOS9_UNABLE_SET_ENVIRONMENT,
  DOS9_INVALID_REDIRECTION,
  DOS9_ALREADY_REDIRECTED,
  DOS9_MALFORMED_BLOCKS,
  DOS9_NONCLOSED_BLOCKS,
  DOS9_UNABLE_ADD_COMMAND,
  DOS9_UNABLE_REMAP_COMMANDS,
  DOS9_TRY_REDEFINE_COMMAND,
  DOS9_UNABLE_REPLACE_COMMAND,
  DOS9_UNABLE_SET_OPTION		,
  DOS9_COMPARISON_FORBIDS_STRING,
  DOS9_FAILED_FORK,
  DOS9_NO_MATCH,
  DOS9_NO_VALID_FILE,
  DOS9_INVALID_NUMBER,
  DOS9_TOO_MANY_ARGS,
  DOS9_INVALID_IF_EXPRESSION,
  DOS9_UNABLE_MOVE,
  DOS9_UNABLE_COPY,
  DOS9_UNABLE_DELETE,
  DOS9_UNABLE_MKDIR,
  DOS9_UNABLE_RMDIR,
  DOS9_MOVE_NOT_RENAME,
  DOS9_INVALID_CODEPAGE,
  DOS9_BREAK_ERROR,
  DOS9_INVALID_LABEL,
  DOS9_LOCK_MUTEX_ERROR,
  DOS9_RELEASE_MUTEX_ERROR,
  DOS9_UNABLE_CAT,
  DOS9_UNABLE_LOAD_MODULE,
  DOS9_INCOMPATIBLE_MODULE,
  DOS9_MODULE_LOADED,
  DOS9_ERROR_MESSAGE_NUMBER
};

#define DOS9_PRINT_C_ERROR 0x80

extern const char* lpErrorMsg[DOS9_ERROR_MESSAGE_NUMBER];
extern const char* lpQuitMessage;

#endif
