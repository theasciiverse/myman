/* string.h - Missing headers for glibc on Linux for use with TCC
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

#ifndef GLIBC_MISSING_HEADERS_STRING_H_INC
#define GLIBC_MISSING_HEADERS_STRING_H_INC 1

#include <tcclib.h>

#include <sys/types.h>

/* extern char *strchr(const char *s, int c); */
/* extern char *strrchr(const char *s, int c); */
extern char *strstr(const char *haystack, const char *needle);
extern size_t strlen(const char *s);
/* extern char *strcpy(char *dest, const char *src); */
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
/* extern void *memcpy(void *dest, const void *src, size_t n); */
/* extern void *memset(void *s, int c, size_t n); */
extern int memcmp(const void *s1, const void *s2, size_t n);

#endif /* ! defined(GLIBC_MISSING_HEADERS_STRING_H_INC) */
