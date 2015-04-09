/* stdio.h - Missing headers for glibc on Linux for use with TCC
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

#ifndef GLIBC_MISSING_HEADERS_STDIO_H_INC
#define GLIBC_MISSING_HEADERS_STDIO_H_INC 1

#include <tcclib.h>

#include <linux/fs.h>

#define L_tmpnam 20
/* #define EOF (-1) */

/* typedef void FILE; */

/* extern FILE *stdout, *stderr, *stdin; */
/* extern FILE *fopen(const char *path, const char *mode); */
/* extern FILE *freopen(const char *path, const char *mode, FILE *stream); */
/* extern int fgetc(FILE *stream); */
/* extern int ungetc(int c, FILE *stream); */
extern int fputc(int c, FILE *stream);
extern int putchar(int c);
/* extern int printf(const char *format, ...); */
/* extern int fprintf(FILE *stream, const char *format, ...); */
/* extern int sprintf(char *str, const char *format, ...); */
extern int fscanf(FILE *stream, const char *format, ...);
extern int feof(FILE *stream);
extern long ftell(FILE *stream);
/* extern int fclose(FILE *fp); */
extern int fflush(FILE *stream);
/* extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream); */
/* extern void perror(const char *s); */
extern int fputs(const char *s, FILE *stream);
extern int fileno(FILE *stream);
extern void setbuf(FILE *stream, char *buf);
extern int sscanf(const char *str, const char *format, ...);
extern int puts(const char *s);

#endif /* ! defined(GLIBC_MISSING_HEADERS_STDIO_H_INC) */
