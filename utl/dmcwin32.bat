@echo off
rem dmcwin32.bat - compile MyMan on Win32 using Digital Mars C/C++

set CC=dmc

%CC% -DRAWCURSES -Iinc -Imygetopt src\myman.c mygetopt\mygetopt.c src\utils.c src\logic.c advapi32.lib %1 %2 %3 %4 %5 %6 %7 %8 %9
