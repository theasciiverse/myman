@echo off
@rem # turboc.bat - compile MyMan using Turbo C
@rem #
@rem # NOTE: this does not work with both color and optcurses yet --
@rem # the compiler runs out of memory and croaks.

set CC=tcc -ml -O -d -Z

if not exist obj mkdir obj

@rem #
@rem # create directory hierarchy for converted files
@rem #

if not exist obj\crlf mkdir obj\crlf
if not exist obj\crlf\mygetopt mkdir obj\crlf\mygetopt
if not exist obj\crlf\inc mkdir obj\crlf\inc
if not exist obj\crlf\src mkdir obj\crlf\src


@rem #
@rem # cmdx - bootstrap executable for running DOS command lines
@rem #
@rem # NOTE: this is only created if it does not already exist

if exist obj\cmdx.exe goto cmdx

cd obj
%comspec% /c "echo main(i,s)char**s;{exit(system(s[1]));} > cmdx.c"
if errorlevel 1 goto cmdx1
goto cmdx2

:cmdx1
@rem # NOTE: some versions of command.com do not understand quotes
%comspec% /c echo main(i,s)char**s;{exit(system(s[1]));} > cmdx.c

:cmdx2
%CC% -ecmdx.exe cmdx.c
cd ..

:cmdx

@rem #
@rem # echox - bootstrap executable for creating multi-line c source with CR/LF pairs
@rem #
@rem # NOTE: this is only created if it does not already exist

if exist obj\echox.exe goto echox

cd obj
cmdx "echo main(i,s)char**s;{while(*++s)puts(*s);} > echox.c"
%CC% -eechox.exe echox.c
cd ..

:echox

@rem #
@rem # u2d - lightweight line-ending converter
@rem #
@rem # NOTE: this is only created if it does not already exist

if exist obj\u2d.exe goto u2d

obj\echox "compiling u2d"

cd obj
echox "#include <stdio.h>" "main(i){while((i=getchar())!=-1)putchar(i);}" > u2d.c
%CC% -eu2d.exe u2d.c
cd ..

:u2d

obj\echox "deleting old build products"

if exist obj\crlf\mygetopt\*.* del obj\crlf\mygetopt\*.*
if exist obj\crlf\inc\*.* del obj\crlf\inc\*.*
if exist obj\crlf\src\*.* del obj\crlf\src\*.*
if exist obj\*.obj del obj\*.obj
if exist myman-t.exe del myman-t.exe
if exist myman-m.exe del myman-m.exe
if exist myman-c.exe del myman-c.exe
if exist myman.exe del myman.exe
if exist myman-r.exe del myman-r.exe
if exist myman-rc.exe del myman-rc.exe

obj\echox "converting line endings"

obj\cmdx "obj\u2d < mygetopt\getopt.h > obj\crlf\mygetopt\getopt.h"
obj\cmdx "obj\u2d < mygetopt\mygetopt.h > obj\crlf\mygetopt\mygetopt.h"
obj\cmdx "obj\u2d < inc\guess.h > obj\crlf\inc\guess.h"
obj\cmdx "obj\u2d < inc\utils.h > obj\crlf\inc\utils.h"
obj\cmdx "obj\u2d < inc\coniocur.h > obj\crlf\inc\coniocur.h"
obj\cmdx "obj\u2d < inc\rawcurs.h > obj\crlf\inc\rawcurs.h"
obj\cmdx "obj\u2d < inc\optcurs.h > obj\crlf\inc\optcurs.h"

obj\cmdx "obj\u2d < mygetopt\mygetopt.c > obj\crlf\mygetopt\mygetopt.c"
obj\cmdx "obj\u2d < src\utils.c > obj\crlf\src\utils.c"
obj\cmdx "obj\u2d < src\logic.c > obj\crlf\src\logic.c"
obj\cmdx "obj\u2d < src\myman.c > obj\crlf\src\myman.c"
obj\cmdx "obj\u2d < src\main.c > obj\crlf\src\main.c"

cd obj\crlf

set ret=0

..\..\obj\echox "compiling myman-t"

%CC% -DMY_CURSES_H="coniocur.h";USE_COLOR=0 -Imygetopt;inc -n..\..\obj -o..\..\obj\myman-t -c src\myman
if errorlevel 1 set ret=1

..\..\obj\echox "compiling myman-m"

%CC% -DCONIOCURSES;USE_COLOR=0 -Imygetopt;inc -n..\..\obj -o..\..\obj\myman-m -c src\myman
if errorlevel 1 set ret=1

..\..\obj\echox "compiling myman-c"

%CC% -DMY_CURSES_H="coniocur.h" -Imygetopt;inc -n..\..\obj -o..\..\obj\myman-c -c src\myman
if errorlevel 1 set ret=1

..\..\obj\echox "compiling myman"

%CC% -DCONIOCURSES -Imygetopt;inc -n..\..\obj -c src\myman
if errorlevel 1 set ret=1

..\..\obj\echox "compiling myman-r"

%CC% -DMY_CURSES_H="rawcurs.h";USE_COLOR=0 -Imygetopt;inc -n..\..\obj -o..\..\obj\myman-r -c src\myman
if errorlevel 1 set ret=1

..\..\obj\echox "compiling myman-rc"

%CC% -DMY_CURSES_H="rawcurs.h" -Imygetopt;inc -n..\..\obj -o..\..\obj\myman-rc -c src\myman
if errorlevel 1 set ret=1

..\..\obj\echox "compiling utils"

%CC% -Imygetopt;inc -n..\..\obj -c src\utils
if errorlevel 1 goto croaked

..\..\obj\echox "compiling logic"

%CC% -Imygetopt;inc -n..\..\obj -c src\logic
if errorlevel 1 goto croaked

..\..\obj\echox "compiling mygetopt"

%CC% -Imygetopt;inc -n..\..\obj -c mygetopt\mygetopt
if errorlevel 1 goto croaked

..\..\obj\echox "linking"

cd ..\..\obj
%CC% -e..\myman-t utils.obj logic.obj myman-t.obj mygetopt.obj
%CC% -e..\myman-m utils.obj logic.obj myman-m.obj mygetopt.obj
%CC% -e..\myman-c utils.obj logic.obj myman-c.obj mygetopt.obj
%CC% -e..\myman utils.obj logic.obj myman.obj mygetopt.obj
%CC% -e..\myman-r utils.obj logic.obj myman-r.obj mygetopt.obj
%CC% -e..\myman-rc utils.obj logic.obj myman-rc.obj mygetopt.obj

if errorlevel 1 goto croaked
goto ok
:croaked
set ret=1
:ok

cd ..\..

@rem #
@rem # remove converted files
@rem #

obj\echox "removing converted files"

del obj\crlf\mygetopt\*.h
del obj\crlf\inc\*.h
del obj\crlf\mygetopt\*.c
del obj\crlf\src\*.c

@rem #
@rem # remove directory hierarchy for converted files
@rem #

obj\echox "removing directory hierarchy for converted files"

rmdir obj\crlf\mygetopt
rmdir obj\crlf\inc
rmdir obj\crlf\src
rmdir obj\crlf

obj\echox "done [EXIT STATUS %ret%]"

%comspec% /c exit %ret%
