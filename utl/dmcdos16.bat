@echo off
rem dmcdos16.bat - cross-compile MyMan for 16-bit Dos using Digital Mars C/C++

set CC=dmc

%CC% -0 -GT128 -o+space -NS -mld -DRAWCURSES -Iinc -Imygetopt src\myman.c mygetopt\mygetopt.c src\utils.c src\logic.c %1 %2 %3 %4 %5 %6 %7 %8 %9

