@echo off
rem watcom.bat - compile MyMan for 16-bit DOS using Open Watcom C/C++
rem
rem This attempts to build the following executables:
rem MYMAN.EXE - 16-bit huge-model using raw stdio
rem MYMAN-G.EXE - 16-bit huge-model using <graph.h>

set CC=owcc -v -bDOS -Wc,-zm -mtune=i86 -O3 -Os -mcmodel=h -Wc,-zc

if exist myman.exe del myman.exe
if exist myman-g.exe del myman-g.exe

if exist __owcc__.lnk del __owcc__.lnk
if exist mygetopt.o del mygetopt.o
if exist utils.o del utils.o
if exist logic.o del logic.o
if exist myman-g.o del myman-g.o
if exist myman.o del myman.o
if exist mygetopt.err del mygetopt.err
if exist utils.err del utils.err
if exist logic.err del logic.err
if exist myman.err del myman.err

%CC% -c -Iinc -Imygetopt -omygetopt.o mygetopt\mygetopt.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -c -Iinc -Imygetopt -outils.o src\utils.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -c -Iinc -Imygetopt -ologic.o src\logic.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -DGRAPHCURSES -c -Iinc -Imygetopt -omyman-g.o src\myman.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -DRAWCURSES -c -Iinc -Imygetopt -omyman.o src\myman.c %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -omyman-g.exe utils.o logic.o mygetopt.o myman-g.o %1 %2 %3 %4 %5 %6 %7 %8 %9
%CC% -omyman.exe utils.o logic.o mygetopt.o myman.o %1 %2 %3 %4 %5 %6 %7 %8 %9
