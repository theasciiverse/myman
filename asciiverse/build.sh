#!/bin/sh
# Builds myman into out/ with upstream's own configure and Makefile. Every
# maze, tile set and sprite set is compiled in, so the one binary is the
# whole game. In upstream's Makefile CC builds the helper programs that
# turn the mazes and sprites into C and run during the build, HOSTCC
# builds the game itself; LDFLAGS reaches both links. JOBS (default 2)
# sets make's parallel jobs. See .github/workflows/release.yml:
#
#   asciiverse/build.sh                                  # Debian, Ubuntu
#   LDFLAGS="-static -s" asciiverse/build.sh             # Alpine, static
#   NCURSESLIBS=-lncurses NCURSESCFLAGS= \
#     CONFIGURE=--with-ncurses SED=gsed asciiverse/build.sh  # macOS
#   CONFIGURE="--host=x86_64-w64-mingw32 --with-raw --without-win-icon" \
#     HOSTCC=x86_64-w64-mingw32-gcc HOSTLIBS=-lwinpthread \
#     LDFLAGS="-static -s" asciiverse/build.sh           # Windows
#
# The Windows build draws through the Win32 console API (--with-raw), so
# it needs no curses library; winpthread supplies nanosleep.
set -eu
cd "$(dirname "$0")/.."
JOBS="${JOBS:-2}"
set --
for v in CC CFLAGS LDFLAGS LIBS HOSTCC HOSTCFLAGS HOSTLIBS SED; do
  eval "val=\${$v-}"
  [ -n "$val" ] && set -- "$@" "$v=$val"
done
# shellcheck disable=SC2086
./configure ${CONFIGURE:---with-ncursesw} "$@" >/dev/null
case "${CONFIGURE:-}" in *mingw*) exe=.exe ;; *) exe= ;; esac
nice -n 10 make -j"$JOBS" "myman$exe" myman.6
mkdir -p out
cp "myman$exe" "out/myman$exe"
echo "built myman$exe"
