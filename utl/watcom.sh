#!/bin/bash --
# watcom.sh - compile MyMan for 16-bit DOS using Open Watcom C/C++
#
# This calls watcom.bat inside DOSBox to do the dirty work.

# override DOSBOX to point to your DOSBox binary and possibly some arguments
DOSBOX="${DOSBOX:-dosbox}"

# override $WATCOM to point to the root of your Open Watcom installation
# i.e. the directory containing a "binw" subdirectory
WATCOM="${WATCOM:-~/WATCOM}"

prefix="WAT$$"
srcdir="$(dirname "$0")"/..
TMPDIR="${TMPDIR:-/tmp}"
errfile="${prefix}.ERR"
retfile="${prefix}.RET"

# allow settings to be overridden on the command line
while [ $# -gt 0 ]
do
    if [ :"${1/=/}" != :"$1" ]
    then
        export -- "$1"
    else
        echo "$0: unrecognized option: '$1'" >&2
        echo "usage: $0 [ VAR1=VALUE1 [ VAR2=VALUE2 [ ... ] ]" >&2
        exit 2
    fi
    shift
done

cd "$srcdir" &&
rm -f "${TMPDIR}/${errfile}" &&
touch "${TMPDIR}/${errfile}" &&
rm -f "${TMPDIR}/${retfile}" &&
echo 0 > "${TMPDIR}/${retfile}" &&
(
    tail -f "${TMPDIR}/${errfile}" | sed 's/$//;s/^\([^:]*\)\\/\1\//;s/: \([0-9]\+\): /:\1: /;s/\(.*\) (warning)$/warning: \1/;s/ (error)$//'
) & tail=${!}
sleep 1
(echo '[dosbox]'; echo 'memsize=64'; echo '[cpu]'; echo 'cycles=max') | $DOSBOX -conf /dev/fd/0 -noautoexec -c cls -c 'mount c '"${WATCOM}" -c 'mount t '"${TMPDIR:-/tmp}" -c 'mount d .' -c 'd:' -c 'set WATCOM=c:\' -c 'set INCLUDE=c:\h' -c 'path c:\binw;%path%' -c '%COMSPEC% /c call utl\watcom '"$*"' -fr=t:'"${errfile}"' > t:'"${errfile}"'' -c 'if errorlevel 1 echo 1 >> t:'"${retfile}"'' -c 'exit' >/dev/null || echo $? > "${TMPDIR}/${retfile}"
sleep 1
disown "$tail"
kill "$tail"
ret="$(cat "${TMPDIR}/${retfile}" | tr -d '' | tail -1)"
rm -f "${TMPDIR}/${retfile}" "${TMPDIR}/${errfile}"
exit "$ret"
