@echo off
rem tccwin32.bat - compile (and maybe run) MyMan using TCC-Win32
rem
rem NOTE: try this batch file with -run [ NUL: args... ]

set CC=tcc

%CC% -version > nul:

if errorlevel 1 set CC=c:\tcc\tcc

%CC% -DRAWCURSES -Iinc -Imygetopt -Lobj src\myman.c mygetopt\mygetopt.c src\utils.c src\logic.c -luser32 %1 %2 %3 %4 %5 %6 %7 %8 %9
