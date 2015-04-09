@echo off
rem watcom32.bat - compile MyMan for 32-bit DOS/4GW using Open Watcom C/C++
rem
rem NOTE: This requires you to create a copy of owcc.exe called
rem owcc32.exe which invokes wcc386.exe rather than wcc.exe. I did this
rem by copying the executable and patching the binary in a hex editor to
rem call "wzz" instead of "wcc", and by making a copy of wcc386.exe
rem called wzz.exe so that the character count in owcc32.exe would not
rem change.
rem
rem This attempts to build the following executables:
rem MYMAN32.EXE - 32-bit (DOS/4GW) using raw stdio
rem MYMAN32G.EXE - 32-bit (DOS/4GW) using <graph.h>

set CC=owcc32 -v -bDOS4G -mtune=3 -O3 -Os -mcmodel=f

if exist myman32.exe del myman32.exe
if exist myman32g.exe del myman32g.exe

if exist __owcc__.lnk del __owcc__.lnk
if exist getopt32.o del getopt32.o
if exist utils32.o del utils32.o
if exist logic32.o del logic32.o
if exist myman32g.o del myman32g.o
if exist myman32.o del myman32.o
if exist getopt32.err del getopt32.err
if exist utils32.err del utils32.err
if exist logic32.err del logic32.err
if exist myman32.err del myman32.err

%CC% -c -Iinc -Imygetopt -ogetopt32.o mygetopt\mygetopt.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -c -Iinc -Imygetopt -outils32.o src\utils.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -c -Iinc -Imygetopt -ologic32.o src\logic.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -DGRAPHCURSES -c -Iinc -Imygetopt -omyman32g.o src\myman.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -DRAWCURSES -c -Iinc -Imygetopt -omyman32.o src\myman.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -omyman32g.exe utils32.o logic32.o getopt32.o myman32g.o %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -omyman32.exe utils32.o logic32.o getopt32.o myman32.o %1 %2 %3 %4 %5 %6 %7 %8 %9
