/* tccosx.c - hacks for use with tcc on Mac OS X/Darwin
 * Copyright 2016, Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

/* FIXME: once tcc is fixed, remove this file */

static void *libSystem;

static void *fromLibSystem(const char*sym) {
    libSystem = libSystem ? libSystem : dlopen("libSystem.dylib", 0);
    return dlsym(libSystem, sym);
}

double
_strtod(const char *restrict nptr, char **restrict endptr) {
    return (*((double(*)(const char*restrict, char**restrict)) fromLibSystem("strtod")))(nptr, endptr);
}

FILE *
_fopen(const char *restrict filename, const char *restrict mode) {
    return (*((FILE *(*)(const char*restrict, const char*restrict)) fromLibSystem("fopen")))(filename, mode);
}

size_t
_fwrite(const void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream) {
    return (*((size_t(*)(const void*restrict, size_t, size_t, FILE*restrict)) fromLibSystem("fwrite")))(ptr, size, nitems, stream);
}

int
_setenv(const char *name, const char *value, int overwrite) {
    return (*((int(*)(const char*, const char*, int)) fromLibSystem("setenv")))(name, value, overwrite);
}

int
_fputs(const char *restrict s, FILE *restrict stream) {
    return (*((int(*)(const char*restrict, FILE*restrict)) fromLibSystem("fputs")))(s, stream);
}

int
_usleep(useconds_t useconds) {
    return (*((int(*)(useconds_t)) fromLibSystem("usleep")))(useconds);
}

int
_select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout) {
    return (*((int(*)(int, fd_set*restrict, fd_set*restrict, fd_set*restrict, struct timeval*restrict)) fromLibSystem("select")))(nfds, readfds, writefds, errorfds, timeout);
}

int
_fcntl(int fildes, int cmd, void*HACK) {
    return (*((int(*)(int, int, void*)) fromLibSystem("fcntl")))(fildes, cmd, HACK);
}

ssize_t
_read(int fildes, void *buf, size_t nbyte) {
    return (*((ssize_t(*)(int, void*, size_t)) fromLibSystem("read")))(fildes, buf, nbyte);
}

FILE *
_freopen(const char *restrict filename, const char *restrict mode, FILE *restrict stream) {
    return (*((FILE*(*)(const char*restrict, const char*restrict, FILE*restrict)) fromLibSystem("freopen")))(filename, mode, stream);
}
