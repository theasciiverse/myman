/* stdlib.h - Missing headers for glibc on Linux for use with TCC
 * Copyright 2013, Benjamin C. Wiley Sittler <bsittler@gmail.com>
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

#ifndef GLIBC_MISSING_HEADERS_STDLIB_H_INC
#define GLIBC_MISSING_HEADERS_STDLIB_H_INC 1

#include <tcclib.h>

#include <sys/types.h>

#define EXIT_SUCCESS 0

extern char *getenv(const char *name);
/* extern void *malloc(size_t size); */
/* extern void free(void *ptr); */
/* extern void *realloc(void *ptr, size_t size); */
/* extern long int strtol(const char *nptr, char **endptr, int base); */
extern double strtod(const char *nptr, char **endptr);
extern int setenv(const char *name, const char *value, int overwrite);
/* extern int atoi(const char *nptr); */
extern void exit(int status);

/* HACK: This is only actually needed in older versions of tcc, but I
 * have found no way to determine the TinyC version at compile-time. */
#ifndef atexit
extern int __cxa_atexit(void (*func) (void *), void *arg, void *dso_handle);
#define atexit(fn) __cxa_atexit((void *)(fn),0,0)
#endif


#endif /* ! defined(GLIBC_MISSING_HEADERS_STDLIB_H_INC) */
