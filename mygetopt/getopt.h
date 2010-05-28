/*  getopt.h - cpp wrapper for mygetopt to make it look like getopt.
 *  Copyright 1997, 2000, 2001, 2002, 2008, 2009, Benjamin C. Wiley Sittler
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

#ifndef MY_WRAPPER_GETOPT_H_INCLUDED
#define MY_WRAPPER_GETOPT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MYGETOPT_MYGETOPT_H
#include MYGETOPT_MYGETOPT_H
#else
#include "mygetopt.h"
#endif

#ifdef getopt
#undef getopt
#endif
#define getopt mygetopt
#ifdef getopt_long
#undef getopt_long
#endif
#define getopt_long mygetopt_long
#ifdef getopt_long_only
#undef getopt_long_only
#endif
#define getopt_long_only mygetopt_long_only
#ifdef _getopt_internal
#undef _getopt_internal
#endif
#define _getopt_internal _mygetopt_internal
#ifdef opterr
#undef opterr
#endif
#define opterr my_opterr
#ifdef optind
#undef optind
#endif
#define optind my_optind
#ifdef optopt
#undef optopt
#endif
#define optopt my_optopt
#ifdef optarg
#undef optarg
#endif
#define optarg my_optarg
#ifdef optreset
#undef optreset
#endif
#define optreset my_optreset

#ifdef __cplusplus
}
#endif

#endif /* MY_WRAPPER_GETOPT_H_INCLUDED */
