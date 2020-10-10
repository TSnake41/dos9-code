# Programming Guidelines to Dos9 #

This documents sums up some quite important guidelines to programmers that are 
interested in extending Dos9. It discusses overall topic about internal Dos9 
features.

Please read this document carefully before attempting any modifications to the 
original code base.

## Command semantics ##

Dos9 follows some rules for the design of command-lines for the command it 
includes. As far as possible, the following rules must be met:

* No switches should be mandatory. Switch are designed to toogle in optional 
  features, not to provide you with easy-to-use delimiters. It is possible to 
  pass parameters following switches using either **/switch parameter** or 
  **/switch\[:\]parameter** at your option. However, the second syntax should 
  not be used if **/switch** requires several parameters.

* No choices should be mandatory. We consider that there is very few occasions 
  where a choice cannot be ignored in a well designed command. Please try to 
  avoid syntaxes like **{param1 | param2}**.

* Commands dealing with files should definitely handle regular expressions and 
  be able to process as many file name as can be provided. This is sadly not 
  the case in every Dos9 command but providing robust commands is highly 
  considered. 

## Creating new commands ##

Commands are the easiest extension that can be added to Dos9. To begin with, a 
typical function implementing a command should be of the following form:

    int Dos9_CmdMyCommand(char* line)

Where **\*line** is a pointer to the command line and the return value is the 
**ERRORLEVEL** status of the command. Note that you should not assume that 
**\*line** is writeable. To Get Command line arguments, One of the following 
command should be used :

    /* Get the next command line parameter expanded in a buffer */
    char* Dos9_GetNextParameter(char* line, char* recv, int size);
    /* Similar, but returns a ESTR */
    char* Dos9_GetNextParameterEs(char* line, ESTR* recv);
    /* Get the end of line expanded in an ESTR */
    char* Dos9_GetEndOfLine(char* line, ESTR* recv);
    /* Get a parameter list from the line */
    PARAMLIST* Dos9_GetParamList(char* lpLine);
    void Dos9_FreeParamList(PARAMLIST* list);

Where **\*line** refers to the current position in the command line, and the 
**return value** refers to the next position in the command line \(or 
**NULL** if the end of line has been reached\). The **\*recv** parameter 
should refer to a buffer of at least size chars, or to an **ESTR** structure 
\(a dynamic sized string structure provided by Dos9\).

To add a new command to the intrepretor, just add an element describing your 
command inside the array exposed in **core/Dos9\_CommandInfo.h**.

If your command is designed to interract with the user through text messages, 
please provide appropriate gettext interface to translate them, as described 
below. Be sure to free all the objects you may allocate.

If your command uses standard input and output, please use carefully the 
thread-local files provided by the interpretor, as described below.

## Includes ##

Almost everything you may ever need is in:

    #include "Dos9_Core.h" /* file in dos9/core/ */
     #include <libDos9.h>

## Multi threading ##

It is important to notice that this branch of Dos9 actively uses 
multi-threading capabilities to run various interpretors from a single Dos9 
process, allowing various features run faster. Such features include pipes, or 
even for loops.

For this reason, use of global or static variables \(unless marked as thread 
local storage, eg. declared with **\_\_thread attribute**\) is strongly 
discouraged.

Creating a new interpretor inside the current process can easily be achieved 
by a call to **Dos9\_CloneInstance\(\)**:

    THREAD Dos9_CloneInstance(void(*proc)(void*), void* data);

Where **\*proc** refers to a function to be called upon successful launch of 
the new interpretor and **\*data** refers to a pointer to be passed as a 
parameter to **\*proc**. This function always succeed. Upon interpretors exit, 
the thread returns a value corresponding to the interpretors last 
**ERRORLEVEL** status. 

As a process may host various interpretors, it is advised to pay a particular 
attention to memory leaks as they can affect really badly other interpretors.

## ESTR structures ##

Dos9 also provides some functions to manage dynamic sized strings. These allow 
dealing with string without caring about the size of the underlying buffer. 
These strings are designed by ESTR structures.

    /* Allocate new ESTR structure */
    ESTR* Dos9_EsInit(void); 
    /* Free ESTR structure */
    void Dos9_EsFree(ESTR* str); 
    /* Copy *orig into *dest */
    int Dos9_EsCpy(ESTR* dest, char* orig);
    int Dos9_EsCpyE(ESTR* dest, ESTR* orig); 
    /* Concatenate *orig to *dest */
    int Dos9_EsCat(ESTR* dest, char* orig); 
    int Dos9_EsCatE(ESTR* dest, ESTR* orig);
    /* Return internal char buffer version of estr */
    char* Dos9_EsToChar(ESTR* estr); 
    /* Return internal char buffer version of estr */
    char* estr->str;

**ESTR** is a opaque structure providing extendable strings. Theses structure 
needs to be allocated through **Dos9\_EsInit\(\)** and freed through 
**Dos9\_EsFree\(\)**. The c-string associated with a **ESTR** structure can be 
obtaine using the **str** member as described above.

## Output streams ##

As said earlier, Dos9 is multi-threaded, various thread may run 
simultaneously, and have different output files. For these reasons, standard 
output, error and input must be accessed using Dos9 internal variables listed 
below, which are **thread-local**:

    __thread FILE* fInput; /* stdin */
    __thread FILE* fOutput; /* stdout */
    __thread FILE* fError; /* stderr */

Thus, code must use the **f** prefixed versions of C standard library 
functions \(eg. **fprintf** and so on\).

In addition, both **fInput**, **fOutput** and **fError** are binary files, 
thus, If you want a message containing newlines to be printed portably across 
platforms, use the **DOS9\_NL** macro instead of **\n**.

## Paths ##

As Dos9 is multi threaded, a command shall not refer to OS current directory, 
instead, it shall use one of the following function to create Absolute paths. 
Note that it also implies that it is not safe to use relative paths in a 
command without using these functions to convert them to absolute path.

    /* Convert *full to absolute path and return full->str */
    char* Dos9_EsToFullPath(ESTR* full);
    /* Same as Dos9_MakeFullPath() but check the path exists */ 
    int  Dos9_GetFileFullPath(char* full, const char* partial, size_t size);
    /* Set *full (at least size bytes long) to absolute version of *partial path */
    void Dos9_MakeFullPath(char* full, const char* partial, size_t size);
    /* Set *full to the absolute version of *partial path */
    void Dos9_MakeFullPathEs(ESTR* full, const char* partial);
    /* Duplicate a path converting it to absolute. Behaviour identical to strdup(). */
    char* Dos9_FullPathDup(const char* path);

**Dos9\_FullPathDup\(\)** allocates a new string to store the absolute path, 
be sure to free it with **free\(\)** when no longer needed.

## Environment ##

Concerning environment variable, Dos9 uses its own internal functions that 
deal with internally stored environment. Dos9 does not rely on functions 
provided by the OS that runs it, as there is too many differences in the 
implementations.

     /* A pointer to the interpretor environment */
    ENVBUF* lpeEnv;
    /* Duplicate environment */
    ENVBUF* Dos9_EnvDup(ENVBUF* pEnv);
    /* Initialize environment */
    ENVBUF* Dos9_InitEnv(char** env)
    /* Retrieve a var */
    char* Dos9_GetEnv(ENVBUF* pEnv, const char* name);
    /* Set a var */
    void  Dos9_SetEnv(ENVBUF* pEnv, const char* name, const char* content);
    /* Unset a var */
    void  Dos9_UnSetEnv(ENVBUF* pEnv, const char* name);

**ENVBUF** is a structure containing the current environment, it can be 
allocated from a previously allocated **ENVBUF** via **Dos9\_EnvDup\(\)**, or 
via an array of c-strings \(just like **environ**\) using 
**Dos9\_InitEnv\(\)**.

To access the current intrepretor environment, please use the thread-local 
variable **lpeEnv**.

## File search functions ##

Dos9 provides a mechanism to get matching files from regular expression in 
libDos9. 

    /* Structure to list matching files */
    typedef struct FILELIST {
        char  lpFileName[FILENAME_MAX]; /* filename */
        struct stat stFileStats; /* file stats */
        struct FILELIST* lpflNext; /* pointer to next match */
        struct FILELIST* lpflPrevious; /* Use only internally, do not use this */
    } FILELIST,*LPFILELIST;
    
    /* Return a list of all matching files to *lpPathMatch */
    FILELIST*  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag);
    /* Free a list of matching files */
    void Dos9_FreeFileList(FILELIST* list);
    

Where **\*lpPathMatch** is a pointer to a nul-terminated regular expression to 
match. The **FILELIST** is allocated by the function, do not forget to free it 
with the appropriate function. **Dos9\_GetMatchFileList\(\)** can accept the 
following flags:

     /* Match everything in the directory */
     #define DOS9_SEARCH_DEFAULT
     /* Browse sub-dir for matches */
     #define DOS9_SEARCH_RECURSIVE
     /* Stop after the first match is found */
     #define DOS9_SEARCH_GET_FIRST_MATCH
     /* Do not grab informations about files */
     #define DOS9_SEARCH_NO_STAT
     /* Do not list pseudo dirs '.' and '..' */
     #define DOS9_SEARCH_NO_PSEUDO_DIR
     /* Search mode equivalent DIR (ie "dir" is equivalent to "dir/*" */
     #define DOS9_SEARCH_DIR_MODE
    

**Dos9\_GetMatchFileList\(\)** does not search for files with some particular 
attribute. However, Dos9 provides a way to discriminate between files with 
some attributes and others via the following functions:

    /* Returns a short reprensenting attributes 
       from a standard string based attributes representation */   
    short Dos9_MakeFileAttributes(const char* lpToken);
    /* Check that a file has specified attribute */
    int Dos9_CheckFileAttributes(short wAttr, const FILELIST* lpflList);
    /* Split a FILELIST in two list based on attributes */
    int Dos9_AttributesSplitFileList(short wAttr, FILELIST* pIn, FILELIST** pSelected, FILELIST** pRemaining);

The **Dos9\_MakeFileAttributes\(\)** and **Dos9\_CheckFileAttributes\(\)** 
functions are quite straightforward to understand. 
**Dos9\_AttributesSplitFileList\(\)** is a bit less easy to understand. Given 
a FILELIST **\*pIn** and an attribute **wAttr**, the function returns two 
lists of files. Uppon successful return \(0\), **\*pSelected** points to a 
file list that match **wAttr**. While, **\*pRemaining** points to a file list 
that do not match **wAttr**.

Note that **\*\*pSelected** and **\*\*pRemaining** are made from **\*pIn** 
elements, that is **\*pIn** is somehow destroyed in the process. Both 
**\*\*pSelected** and **\*\*pRemaining** must be freed when they are no longer 
useful using **Dos9\_FreeFileList\(\)**.

## Errors handling ##

**Dos9** provides a set of standardized error messages declared in 
**dos9/errors/Dos9\_Errors.h**. Upon failure, the return value of a command 
function \(which is stored in **%ERRORLEVEL%**\) should be one of these 
standard error codes, unless in some specific cases. A locale-dependent 
message can be printed using the **Dos9\_ShowErrorMessage\(\)** function:

    void Dos9_ShowErrorMessage(unsigned int iErrorNumber, const char* lpComplement, int iExitCode);

Where **iErrorNumber** is the error code, **\*lpComplement** a string to be 
displayed as a complement to the error message. If **iExitCode** is different 
from 0, then the function terminates the Dos9 process with **iErrorNumber** as 
exit code.

## Creating Modules ##

The version **218.3** introducted the possibility to load modules in order to 
load additionnal commands at run-time, providing the possibility to 
dynamically extend Dos9. 

The module feature requires a platform providing a way to link dynamically. 
Some platforms such as Android do not provide such interfaces, thus the module 
feature can be disabled when building using:

    make no-modules

Modules examples are provided inside the **modules/** directory. Files located 
in **modules/lib** contain files used to build interfaces between modules and 
Dos9. Thus, when compiling a module, it is mandatory to link against 
**modules/lib/Dos9\_Module.o** if you use C/C++ or to provide equivalent 
interfaces bindings through the programming language you use.

Modules interfaces provide a way for modules to fetch and modify internal 
parameters of the main **Dos9** process. For instance, modules can get 
standard stream of the current Dos9 thread, or get and modify its environment 
variables.

Interfaces provided to modules are the following :

    /* Set environment variable *name to *content */
    void Dos9_SetEnv(const char* name, const char* content);
    /* Get the content of variable *name */
    char* Dos9_GetEnv(const char* name);
    /* Get the current directory */
    const char* Dos9_GetCurrentDir(void);
    /* Set the current directory to *dir */
    int Dos9_SetCurrentDir(const char* dir);
    /* Get fInput */
    FILE* Dos9_GetfInput(void);
    /* Get fOutput */
    FILE* Dos9_GetfOutput(void);
    /* Get fError */
    FILE* Dos9_GetfError(void);
    /* Get bIsScript */
    int Dos9_GetbIsScript(void);
    /* Same as parameters functions */
    char* Dos9_GetNextParameterEs(const char* line, ESTR* recv);
    /* Register a new internal command named after *name, using *fn as a handler*/
    int Dos9_RegisterCommand(const char* name, int(*fn)(char*));
    /* Same as Dos9_ShowErrorMessage() */
    void Dos9_ShowErrorMessage(int err, const char* str, int exitcode);
    /* Same as Dos9_RunLine */
    int Dos9_RunLine(ESTR* line);

When a module gets loaded by **Dos9**, the module function 
**Dos9\_ModuleAttach\(\)** is called just after completing interfaces set-up. 
This allow for some module specific set-up \(such as global variables or 
libraries initialization\) and registration of commands provided by modules 
using **Dos9\_RegisterCommand\(\)**.

## Using gettext ##

**Dos9** uses gettext to manage internationalized messages. Messages are 
defined in **dos9/errors/Dos9\_Errors.c**, **dos9/lang/Dos9\_Lang.c** and 
**dos9/lang/Dos9\_ShowHelp.c**. These 3 files serve different purpose:

* **dos9/errors/Dos9\_Errors.c** provides unified interface to store and 
  display error messages.

* **dos9/lang/Dos9\_ShowHelp.c** provides unified interface to store and 
  display lightweight command help messages.

* **dos9/lang/Dos9\_Lang.c** provides messages and interfaces for the few 
  remaining message that are neither errors nor help messages.

There is no target to merge **po** files, they are automatically merged 
whenever **make all** is invoked. The **po** files generated for **LOCALE** 
are in **po/LOCALE** a simple text editor or poedit can be used to update 
messages. Please try no to leave untranslated or fuzzy messages.

A new locale can be added by adding a new line to the **po/locales.list** and 
building dos9 again. The resulting locale will automatically build \(but not 
automatically translated though\) and integrated to the **bin** target.

The default domain used by **Dos9** is **dos9**. To add a new domain, edit the 
**po/domain.list** and add a new line based on the following syntax:

    domain-name source1 source2 ... sourceN

Where **domain-name** is the name of the text domain to be created and 
**source1 source2 ... sourceN** is a set of source files from where the 
messages will be exctracted. 

Dos9 only extracts messages where **gettext\(\)** is explicitely called, that 
is, no macro expansion. No comment message is extracted neither.

Translatable messages defined in both **dos9/errors/Dos9\_Errors.c** and 
**dos9/lang/Dos9\_Lang.c** should not contain any newline character \(e.g 
'\n'\) in order to prevent incompatibilities between platforms since windows 
uses '\r\n' instead of '\n' \(and similarly, macOS uses '\r'\). However, 
messages defined in **dos9/lang/Dos9\_ShowHelp.c** can contain '\n' characters 
that will be translated at runtime to the appropriate end-of-line characters.

When adding new messages, please avoid to break the standard message format 
used by already defined messages. 

