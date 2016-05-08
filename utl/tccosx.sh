#!/bin/bash --

# Runs MyMan using tcc -run and works around broken header files

main_noreturn() {
    local progname
    progname="$0"
    local link_count
    link_count=0
    while [[ -L "$progname" && $link_count -lt 16 ]]; do
        link_count=$(( $link_count + 1 ))
        local real_progname
        real_progname="$(ls -l "$progname")"
        if [[ :"$real_progname" == :"${real_progname#* -> }" ]]; then
            break
        fi
        real_progname="${real_progname#* -> }"
        if [[ :"${real_progname#/}" == :"$real_progname" ]]; then
            real_progname="$(dirname "$progname")/$real_progname"
        fi
        progname="$real_progname"
    done
    local progdir
    if [[ -r "$progname" || :"${progname/\//}" != :"$progname" ]]; then 
        progdir="$(dirname "$progname")"
    else
        progdir=''
        local path
        path="$PATH"
        while [[ ${#path} != 0 ]]; do
            local path_entry
            path_entry="${path%%:*}"
            path="${path#*:}"
            if [[ -r "$path_entry/$progname" ]]; then
                progdir="$path_entry"
                break
            fi
        done
    fi
    if [[ -L "$progdir" ]]; then
        progdir="$(cd "$progdir" 2>&- && pwd -P || printf '%s' "$progdir")"
    fi
    local srcdir
    srcdir="$progdir/.."
    if [[ :"$srcdir" = :'/..' ]]; then
        srcdir='./..'
    fi
    srcdir="${srcdir##./}"
    if [[ :"${srcdir%/../..}" = :"$srcdir" && :"${srcdir/\/.\//}" = :"$srcdir" ]]; then
        srcdir="${srcdir%/*/..}"
    fi
    if [[ :"$srcdir" = :'' ]]; then
        srcdir='.'
    fi
    if [[ -d "$srcdir" ]]; then
        local srcdir_real
        srcdir_real="$(cd "$srcdir" 2>&- && pwd -P 2>&- || printf '%s' "$srcdir")"
        if [[ ${#srcdir_real} -lt ${#srcdir} ]]; then
            srcdir="$srcdir_real"
        fi
    fi
    local pwd
    pwd="$(pwd -P 2>&- || echo .)"
    srcdir="${srcdir#"$pwd"/}"
    exec tcc -g -Wall -D__DARWIN_ONLY_UNIX_CONFORMANCE -D_OS__OSBYTEORDER_H=1 -D__MATH_H__=1 -DUNICODE -DPRIVATEDATADIR=\""$srcdir"\" -DRAWCURSES -I"$srcdir/"inc -I"$srcdir/"mygetopt -I/usr/include "$srcdir/"src/utils.c "$srcdir/"src/logic.c "$srcdir/"mygetopt/mygetopt.c "$srcdir/"src/tccosx.c -run "$srcdir/"src/myman.c -s spr/spr2h -t chr/khr2h  "$@"
    exit $?
}

main_noreturn "$@"
