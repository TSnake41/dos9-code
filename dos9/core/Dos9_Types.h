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

#ifndef DOS9_TYPES_H
#define DOS9_TYPES_H

#include <libDos9.h>

/* Definition for local var structures :

    An pointer to the command. A pointer to a LOCAL_VAR_BLOCK is in
    fact a pointer to a array that stores strings at a special index,
    according to a simple computation */
typedef char* LOCAL_VAR_BLOCK;

/* Definition of struct that represent an environment variable */
typedef struct ENVVAR {
    char* name; /* a pointer to the name of the variable, this storage
                   way allow having equal signs within the name, though
                   it is not portable on most platforms through standard
                   libC calls */
    char* content; /* a pointer to the content of the variable */
} ENVVAR;

/* definition of stucture that represent an environment buffer */
typedef struct ENVBUF {
    int nb; /* number of elements of the environment buffer */
    int index; /* the index of the last element actually used in the
                  the environment buffer */
    ENVVAR** envbuf; /* environment buffer */
} ENVBUF;

/* definition of a structure that represents a line parsed for
   conditional operators ie ('||' or '&') */
typedef struct PARSED_STREAM {
	ESTR* lpCmdLine; /* The command line */
	char cNodeType; /* conditional operator type */
	struct PARSED_STREAM* lppsNode; /* pointer to next node */
} PARSED_STREAM;

/* definition of a structure that reprensents redirections specified
   through the command line */
typedef struct PARSED_STREAM_START {
	struct PARSED_STREAM* lppsStream; /* a pointer to a parsed line */
	char cOutputMode;  /* the output mode (overwrite, or no) */
	char* lpOutputFile; /* the output file */
	char* lpInputFile; /* the input file */
	char* lpErrorFile; /* the error file */
} PARSED_STREAM_START;


/* Definitions for non redirections systems */

#define STREAMSTACK_BUFSIZ         10
#define STREAMSTACK_FREE_BUFSIZ   (STREAMSTACK_BUFSIZ+3)

union _STREAMSTACK_REDIRECT_DEST {
    int fd;
    FILE* file;
};

struct _STREAMSTACK_REDIRECT {
    char type; /* The redirection type (to file or to fd) */
    int fromFd; /* The original fd */
    union _STREAMSTACK_REDIRECT_DEST to; /* the destination of the
                    redirection */
};

typedef struct STREAMSTACK {
    FILE* in; /* the current stdin file */
    FILE* out; /* the current stdout file */
    FILE* err; /* the current error file */
    FILE* tofree[STREAMSTACK_FREE_BUFSIZ]; /* file pointers to be free */
    int   freeindex; /* numbers of elements to be free'd */
    int   lock; /* a lock that prevent current redirection level to be poped */
    int   index; /* number of redirection used */
    struct _STREAMSTACK_REDIRECT redir[STREAMSTACK_BUFSIZ]; /* not implemented yet */
    struct STREAMSTACK *next; /* a pointer to the previous element */
} STREAMSTACK;

/* Definiton of the structure that reprensents the file that is being
   read */
typedef struct INPUT_FILE {
	char  lpFileName[FILENAME_MAX]; /* The file name */
	int   iPos; /* The position of where we read next line */
	int   bEof; /* An indicator to check wether we reached end of file */
} INPUT_FILE;


/* This structure is designed in order to make Dos9 more
   flexible and to make it possible to have something
   *actually* thread safe.

   The context also stores a pointer the current directory,
   aiming to be able to have multiple threads running
   independently in different current working directories.
   This is included in an effort to make parallelization
   achievable. */

typedef struct DOS9CONTEXT {
    STREAMSTACK* pStack; /* redirection stack */
    LOCAL_VAR_BLOCK* pLocalVars; /* array for local variables */
    INPUT_FILE* pIn; /* information on the loaded file */
    COMMANDLIST* pCommands; /* information about the command available*/
    ENVBUF* pEnv; /* information about environment variable */
    char lpCurrentDir[FILENAME_MAX]; /* current context directory */
    int iMode; /* current interpretor mode */
    int iErrorLevel; /* the ErrorLevel code */
    int iLastErrorLevel; /* the last ErrorLevel code */
} DOS9CONTEXT;


#endif /* DOS9_TYPES_H */
