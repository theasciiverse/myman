@echo off
rem tccwin32.bat - compile (and maybe run) MyMan using TCC-Win32
rem
rem NOTE: try this batch file with -run [ NUL: args... ]

set CC=tcc

if not exist obj mkdir obj
if exist obj\advapi32.def goto ready
tiny_impdef advapi32.dll -o obj\advapi32.def 2>nul:
if errorlevel 1 goto genadvapi32
goto ready

:genadvapi32
echo LIBRARY advapi32.dll > obj\advapi32.def
echo EXPORTS >> obj\advapi32.def
echo RegOpenKeyExA >> obj\advapi32.def
echo RegQueryValueExW >> obj\advapi32.def
echo RegCloseKey >> obj\advapi32.def

:ready

%CC% -DRAWCURSES -Iinc -Imygetopt -Lobj src\myman.c mygetopt\mygetopt.c src\utils.c src\logic.c -luser32 -ladvapi32 %1 %2 %3 %4 %5 %6 %7 %8 %9
