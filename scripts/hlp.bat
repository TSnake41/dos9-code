@ECHO Off
SETLOCAL EnableDelayedExpansion

::   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
::   Copyright (C) 2010-2017 Romain GARBI
:: 	 Copyright (C) 2016      Teddy Astie
::
::   This program is free software: you can redistribute it and/or modify
::   it under the terms of the GNU General Public License as published by
::   the Free Software Foundation, either version 3 of the License, or
::   (at your option) any later version.
::
::   This program is distributed in the hope that it will be useful,
::   but WITHOUT ANY WARRANTY; without even the implied warranty of
::   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
::   GNU General Public License for more details.
::
::   You should have received a copy of the GNU General Public License
::   along with this program.  If not, see <http://www.gnu.org/licenses/>.

set hlp_data_dir=%DOS9_SHARE%/dos9/hlp
set hlp_base_dir=%USERPROFILE%/.dos9/hlp

:: Cd to the appropriate directory
if not exist !hlp_base_dir! MKDIR !hlp_base_dir!
cd /d !hlp_base_dir!

if not exist hlp.conf.bat (
	echo No hlp.conf.bat
	call :config
)

call hlp.conf.bat

if /i "%~1" equ "/l" goto :list
if /i "%~1" equ "/b" goto :build
if /i "%~1" equ "/c" goto :config
if /i "%~1" neq "/?" goto :show
:: if nothing has been selected, goto show


ECHO Hlp 1.0 - Manual page viewier
ECHO Copyright ^(C^) 2013-2018 Romain Garbi ^(Darkbatcher^)
ECHO Copyright ^(C^) 2016-2018 Teddy Astie ^(TSnake41^)
ECHO This program comes with ABSOLUTELY NO WARRANTY; This is free software,
ECHO and you are welcome to redistribute it under the terms of the GNU CPL.
ECHO.
ECHO Hlp [page-name ^| /b [language [charset]] ^| /c ^| /d]
ECHO.
ECHO     - page-name : The page to be opened
ECHO.
ECHO     - /b : build the manual pages using given charset and language: 
ECHO.
ECHO        - charset : The charset to be used to build the manual pages. If no
ECHO          charset is given, then it will use the default charset ^(basically,
ECHO          the utf-8 charset^). This require the ICONV command to support. Use
ECHO          this parameter if your terminal does not support basic utf-8 encoding
ECHO          ^(like windows^).
ECHO.
ECHO        - language : The language to be used to build the manual page.
ECHO.
ECHO     - /c : configures option for the manual page maker.
ECHO.
ECHO     - /l : Show the list of available manual pages.
ECHO.
ECHO     - /d : download the manual package from the repository at
ECHO       ^<http://www.dos9.org^>. [not functional yet]    

GOTO:EOF

:build
ECHO Hlp 1.0 - Manual page viewier Copyright ^(C^) 2013-2016 Romain Garbi ^(Darkbatcher^)
ECHO This program comes with ABSOLUTELY NO WARRANTY; This is free software,
ECHO and you are welcome to redistribute it under the terms of the GNU CPL.
ECHO.

ECHO first param "%~1"
shift /1
IF "%~1" equ "" (

   SET lang_build=%locale%

) else (

   SET lang_build=%~1
   shift /1
)

IF "%~1" equ "" (

	SET charset_build=%charset%

) else (

	SET charset_build=%~1

)

ECHO *** Building manual pages for %lang_build% with charset %charset_build% ***
ECHO.
ECHO  1] Remove content of the documentation folder.
ECHO.
for %%A in (ansi html txt) do (
	if exist %%A RMDIR /Q /S %%A
)
ECHO  2] Creating subdirectories.
ECHO.

:: create static directory
for %%A in (
ansi
html
txt
) do (

	IF not exist %%A (
		echo 	* Creating %%A
		mkdir %%A
	)
)

:: browse the directory and make subirectories that
:: are not existing.
FOR /F "tokens=*" %%A in ('cd /d !hlp_data_dir!/!lang_build!
dir /b /s /a:d') do (
	
	ECHO 	* Creating %%A
	
	SET path_ansi=ansi/%%A
	SET path_html=html/%%A
	SET path_txt=txt/%%A
	
	IF not exist !path_ansi! mkdir !path_ansi!
	IF not exist !path_html! mkdir !path_html!
	IF not exist !path_txt! mkdir !path_txt!

	
)

ECHO.
ECHO. 3] Produce the output :
:: browse the directory and use tea to produce the output.
FOR /F "tokens=*" %%A in ('cd /d !hlp_data_dir!/!lang_build!
dir /b /s /a:-d') do (

	:: unfortunately, we used %~dpnA before to get all the stuff
	:: this is no longer possible since %~d gets absolute path ...
	:: we need a little hack then
	
	SET name=%%~A
	
	SET ansi_build=ansi/!name:.tea=!.ansi
	SET html_build=html/!name:.tea=!.html
	SET txt_build=txt/!name:.tea=!.txt
	
	ECHO    [*] Building "!name:.tea=!" :
	ECHO      - file : !txt_build!
	tea "%hlp_data_dir%/%lang_build%/%%~A" !txt_build!.tmp /E:UTF-8 /O:TEXT-PLAIN

	ECHO      - file : !ansi_build!
	tea "%hlp_data_dir%/%lang_build%/%%~A" !ansi_build!.tmp /E:UTF-8 /O:TEXT-ANSI

	ECHO      - file : !html_build!
    tea "%hlp_data_dir%/%lang_build%/%%~A" !html_build! /E:UTF-8 /O:html /M "%hlp_data_dir%/meta" /H "%hlp_data_dir%/header" /F "%hlp_data_dir%/footer" /T "The Dos9 Project :: "

	ECHO      - converting output from UTF-8 to %charset_build%
	:: just convert .ansi file, it is not needed for html document
	:: which were generated using tea, that means to change encoding
	:: and tag <meta charset="utf-8" />
	if /i !charset_build! NEQ UTF-8 (
		iconv -f UTF-8 -t %charset_build% !ansi_build!.tmp > !ansi_build!
		iconv -f UTF-8 -t %charset_build% !txt_build!.tmp > !txt_build!
	) else (
		move !ansi_build!.tmp !ansi_build! > NUL
		move !txt_build!.tmp !txt_build! > NUL
	)
)

:: Now, create the hlpdb file
ECHO.
ECHO.4] Create title search system
(FOR /F "tokens=*" %%A in ('dir /b /s /a:-D txt/*.txt') do (
	SET /p title= < "%%A" > NUL
	ECHO !title!%%A
)) > hlpdb

ECHO.
ECHO.5] Cleanning temporary files
:: del /s /q ./*.tmp > NUL

GOTO:EOF

:show

if not exist hlpdb (
	echo HLP : Unable to find any compiled manual page. Running "hlp /b"...
	call :build
	ECHO HLP : Built with success
)

if "%~1" equ "" (
	
	if exist txt/commands.txt (
		
		%view_cmd% %mode%/commands.%mode%
		exit /b 0
		
	) else (
	
		echo Unable to find default manual page. Is the man page database really built ?
		echo If not, try to compile it using the /b switch.
		exit /b 1
	)
)

SET i=0

FOR /F "tokens=1,* delims=" %%A IN ('type hlpdb | find /i /e "*%~1**" ') DO (
	SET /a:I i+=1
	SET tmp=%%B
	SET HlpPath[!i!]=!tmp:txt=%mode%!
 	SET HlpPath[!i!].name=%%A
)

if %i%==1 (

	%view_cmd% %HlpPath[1]%
	EXIT

) else if %i%==0 (
	
	ECHO Error: no manual page available for "%~1"
	ECHO.
	exit /b 1

) else (

	ECHO Looking for manual page "%~1"
	ECHO.
	ECHO There are many pages that match the name you gave.
	ECHO Choose the one you want to browse on the following menu:
	ECHO.
	
	FOR /L %%A in (1,1,%i%) do (
		ECHO 	%%A.	!HlpPath[%%A].name!
	)
	
	ECHO.
)

:retry
SET /P choice=Your choice (either 'q' to quit or a number to select the page^) : 

if /i !choice! equ q GOTO:EOF
if not defined HlpPath[!choice!] goto :retry

%view_cmd% !HlpPath[%choice%]!

GOTO:EOF

:list
for /f "tokens=1,2 delims=" %%A in (!hlp_base_dir!/hlpdb) do echo %%~A
goto :eof

:config
if exist hlp.conf.bat call hlp.conf.bat

ECHO Hlp - Manual page viewier
ECHO Copyright ^(C^) 2013-2016 Romain Garbi ^(Darkbatcher^)
ECHO Copyright ^(C^) 2016-2017 Teddy Astie ^(TSnake41^)
ECHO This program comes with ABSOLUTELY NO WARRANTY; This is free software,
ECHO and you are welcome to redistribute it under the terms of the GNU CPL.
ECHO.
ECHO Setting Hlp Options :
ECHO.
ECHO. 1] Setting mode :
ECHO.
ECHO    The mode option determines the file type that will be displayed by Hlp.
ECHO There is three different values possible : ansi, txt and html.
ECHO.
ECHO  * html mode permits to browse allmost all document through the use of html
ECHO files and a browser. This mode is pretty convenient, altough it requieres a
ECHO functional web browser.
ECHO.
ECHO  * ansi mode allows little less interaction. It consists of text files that are
ECHO formatted to be coloured when using a ANSI terminal. In this mode, you can't
ECHO easily surf through links
ECHO.
ECHO  * txt mode consists of pure text files. It consists of text files, with little
ECHO or no formatting. The text files generated are designed to be read on a 
ECHO terminal. You may choose this options if you have neither any functional web 
ECHO browser nor any ANSI-compatible terminal.
ECHO.
:mode_retry
set /p conf_mode=Please choose a mode (html/ansi/txt) :
if not defined conf_mode (
	set conf_mode=html
) else if [ !conf_mode! neq html and !conf_mode! neq ansi and !conf_mode! neq txt ] goto :mode_retry
ECHO.
ECHO 2] Choosing locale :
ECHO.
ECHO    The locale option determines the language you want to use for manual pages.
ECHO The following list of locales is available : 
ECHO.
cd /d %hlp_data_dir%
dir /b /A:D
cd /d %hlp_base_dir%
ECHO.

:locale_retry
set /p conf_locale=Please choose a locale : 
if not defined conf_locale (
   if defined LANGUAGE (
	set conf_locale=!LANGUAGE!
   ) else (
	set conf_locale=en_US
   )
)
if not exist !hlp_data_dir!/!conf_locale! goto :locale_retry

ECHO. 3] Chose charset :
ECHO.
ECHO    The charset is used to determine how to print language-specific characters
ECHO on your terminal device.
ECHO.
ECHO    By default, we recommend to use the UTF-8 encoding. However, if your 
ECHO terminal does not support UTF-8 natively, you can set the charset to the 
ECHO encoding your terminal supports.
ECHO.
ECHO    This option does not affect manual pages viewed in HTML format, since any 
ECHO decent browser supports UTF-8 nowadays
ECHO.
set /p conf_charset=Please choose a charset : [default UTF-8] : 
if not defined conf_charset (
   set conf_charset=UTF-8
)
ECHO.
ECHO 4] Config file generation.
ECHO.
ECHO    You entered the following configuration :
ECHO.
ECHO        * mode : %conf_mode%
ECHO        * locale : %conf_locale%
ECHO        * charset : %conf_charset%
ECHO.
ECHO 	If you are using a UNIX-based system that does not include the xdg-open
ECHO program, You will probably need to edit the configuration file hlp.conf.bat
ECHO to set the correct path to program to be used when opening html files.
:Ok_retry
set /p Ok=Do you really want to continue and save configuration (y/N) : 
if /i [ not defined Ok or !Ok! equ N ] (
	GOTO :show_quit
) else if /i !Ok! neq Y (
	GOTO :Ok_retry
)

(ECHO :: Hlp Configuration file
ECHO set mode=%conf_mode%
ECHO :: The mode to use 
ECHO set view_cmd=%conf_prg%
ECHO :: The command to view the stream
ECHO set locale=%conf_locale%
ECHO :: The locale to use when compiling
ECHO set charset=%conf_charset%
ECHO :: The charset to use when compiling
ECHO set html_prg=start
ECHO set txt_prg=more
ECHO set ansi_prg=more
ECHO set view_cmd=!!%%mode%%_prg!!) > hlp.conf.bat

ECHO 5] Deleting outdated files :
:: If we either change locale or encoding, we must regen files
IF [ !conf_locale! neq !locale! or !conf_charset! neq !charset! ] (
	if exist hlpdb del hlpdb
)
ECHO.
ECHO  *** Configuration file successfully generated. ***


:show_quit
