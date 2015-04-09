#!/bin/bash
# nacl.sh - compile MyMan for NaCl using self-hosted gcc
#
# This is intended for self-hosted compilation inside NaCl Development Environment
# https://chrome.google.com/webstore/detail/nacl-development-environm/aljpgkjeipgnmdpikaajmnepbcfkglfa
#
# Run this script from the parent (myman) directory:
# $ utl/nacl.sh
#
# To use the resulting executable, add the following to ~/.bashrc:
#
# alias myman='~/path/to/myman-sources/smallman -DRAWCURSES_{UTF8,WCWIDTH=-1}'

rm -f smallman
CC=gcc

# This works around some oddness on i686.
if [[ :"$NACL_ARCH" = :i686 && ${#LIBRARY_PATH32} != 0 ]]; then
    CC="$CC -m32 -L '${LIBRARY_PATH32//:/\' -L \'}'"
elif [[ :"$NACL_ARCH" = :x86_64 && ${#LIBRARY_PATH64} != 0 ]]; then
    CC="$CC -m64 -L '${LIBRARY_PATH64//:/\' -L \'}'"
fi

make -f simple.mk smallman CC="$CC" CURSESLIBS=-lppapi_simple\ -lnacl_spawn\ -lnacl_io\ -lppapi\ -lppapi_cpp\ -lcli_main\ -lstdc++\ -lm CURSESINCLUDE=-DUNICODE\ -DRAWCURSES\ -Dmain=nacl_main
