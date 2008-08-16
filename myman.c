/*
 * myman.c - game logic for the MyMan video game
 * Copyright 1997-2006, Benjamin Sittler <bsittler@gmail.com>
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

/* base name for package (without version number or suffix) */
#ifndef MYMAN
#define MYMAN "myman"
#endif

/* package version number */
#ifndef VERSION
#define VERSION "devel"
#endif

/* for setlocale(3) */
#include <locale.h>

#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* for gettimeofday() */
#include <sys/time.h>
#include <time.h>

/* terminal-screen handling library */
#ifdef MY_CURSES_H
#include MY_CURSES_H
#else
#ifdef SLCURSES
#include <slcurses.h>
#else
#ifdef XCURSES
#include <xcurses.h>
#else
#include <curses.h>
#endif
#endif
#endif

/* work-arounds for slcurses */
#ifdef SLCURSES

#ifndef curscr
#define curscr stdscr
#endif

#ifndef MY_INIT_PAIR_RET
#define MY_INIT_PAIR_RET , ~ERR
#endif

#ifndef USE_PALETTE
#define USE_PALETTE 0
#endif

#ifndef intrflush
#define intrflush(win, x)
#endif

#ifndef DISABLE_IDLOK
#ifndef idlok
#define DISABLE_IDLOK
#endif
#endif

#ifndef pair_content
#define pair_content(p,f,b) \
((*(f) = (SLtt_get_color_object ((p))) & 0xff), \
 (*(b) = (SLtt_get_color_object ((p))) >> 7))
#endif

#endif

/* command-line argument parser */
#ifdef MY_GETOPT_H
#include MY_GETOPT_H
#else
#include <getopt.h>
#endif

/* used for screenshots */
#ifndef HTM_SUFFIX
#define HTM_SUFFIX ".html"
#endif

#ifndef CRLF
#define CRLF "\r\n"
#endif

#ifdef __PDCURSES__
#ifndef USE_PALETTE
/* PDCurses doesn't implement init_color as of release 2.2 */
#define USE_PALETTE 0
#endif
#endif

#ifndef USE_WIDEC_SUPPORT
#ifdef WACS_ULCORNER
#define USE_WIDEC_SUPPORT 1
#else
#define USE_WIDEC_SUPPORT 0
#endif
#endif

#ifndef USE_KEYPAD
#ifdef KEY_LEFT
#define USE_KEYPAD 1
#else
#define USE_KEYPAD 0
#endif
#endif

#ifndef SWAPDOTS
#ifdef __PDCURSES__
#ifndef XCURSES
#define SWAPDOTS 1
#endif
#endif
#endif

#ifndef SWAPDOTS
#define SWAPDOTS ((USE_WIDEC_SUPPORT) ? (strstr((setlocale(LC_CTYPE, "") ? setlocale(LC_CTYPE, "") : ""), "UTF-8") ? 1 : 0) : 0)
#endif

#ifdef A_BOLD
#define MY_A_BOLD A_BOLD
#endif

#ifdef A_UNDERLINE
#define MY_A_UNDERLINE A_UNDERLINE
#endif

#ifdef A_STANDOUT
#define MY_A_STANDOUT A_STANDOUT
#endif

#ifdef A_REVERSE
#define MY_A_REVERSE A_REVERSE
#endif

#ifdef A_DIM
#define MY_A_DIM A_DIM
#endif

#ifdef A_ATTRIBUTES
#define MY_A_ATTRIBUTES A_ATTRIBUTES
#endif

#ifdef A_CHARTEXT
#define MY_A_CHARTEXT A_CHARTEXT
#endif

#ifndef MY_A_REVERSE
#ifdef MY_A_STANDOUT
#define MY_A_REVERSE MY_A_STANDOUT
#endif
#endif

#ifndef MY_A_CHARTEXT
#ifdef MY_A_ATTRIBUTES
#define MY_A_CHARTEXT ~MY_A_ATTRIBUTES
#else
#define MY_A_CHARTEXT 0xFF
#endif
#endif

#ifndef USE_ATTR
#ifdef MY_A_BOLD
#define USE_ATTR 1
#else
#define USE_ATTR 0
#endif
#endif

#if ! USE_ATTR
#ifndef USE_COLOR
#define USE_COLOR 0
#endif
#endif

#ifndef SOUND
#define SOUND 0
#endif

#ifndef USE_COLOR
#ifdef COLOR_BLACK
#define USE_COLOR 1
#else
#define USE_COLOR 0
#endif
#endif

#ifndef COLORIZE
#define COLORIZE 1
#endif

#ifndef USE_PALETTE
#define USE_PALETTE USE_COLOR
#endif

#ifndef USE_RAW_UCS
#ifdef SLANG_VERSION
#if SLANG_VERSION >= 20000
#define USE_RAW_UCS 1
#else
#ifdef SLCURSES
#ifdef UTF8
#define USE_RAW_UCS 1
#endif
#endif
#endif
#endif
#endif

#ifndef USE_RAW_UCS
#if USE_WIDEC_SUPPORT
#define USE_RAW_UCS 1
#else
#define USE_RAW_UCS 0
#endif
#endif

#ifndef USE_RAW
#define USE_RAW 0
#endif

#ifndef USE_ACS
#define USE_ACS 1
#endif

#if USE_WIDEC_SUPPORT

#ifndef MY_ACS_BDDB
#ifdef WACS_BDDB
#define MY_ACS_BDDB WACS_BDDB
#endif
#endif
#ifndef MY_ACS_BSSB
#ifdef WACS_BSSB
#define MY_ACS_BSSB WACS_BSSB
#endif
#endif
#ifndef MY_ACS_ULCORNER
#ifdef WACS_ULCORNER
#define MY_ACS_ULCORNER WACS_ULCORNER
#endif
#endif
#ifndef MY_ACS_DDBB
#ifdef WACS_DDBB
#define MY_ACS_DDBB WACS_DDBB
#endif
#endif
#ifndef MY_ACS_SSBB
#ifdef WACS_SSBB
#define MY_ACS_SSBB WACS_SSBB
#endif
#endif
#ifndef MY_ACS_LLCORNER
#ifdef WACS_LLCORNER
#define MY_ACS_LLCORNER WACS_LLCORNER
#endif
#endif
#ifndef MY_ACS_BBDD
#ifdef WACS_BBDD
#define MY_ACS_BBDD WACS_BBDD
#endif
#endif
#ifndef MY_ACS_BBSS
#ifdef WACS_BBSS
#define MY_ACS_BBSS WACS_BBSS
#endif
#endif
#ifndef MY_ACS_URCORNER
#ifdef WACS_URCORNER
#define MY_ACS_URCORNER WACS_URCORNER
#endif
#endif
#ifndef MY_ACS_DBBD
#ifdef WACS_DBBD
#define MY_ACS_DBBD WACS_DBBD
#endif
#endif
#ifndef MY_ACS_SBBS
#ifdef WACS_SBBS
#define MY_ACS_SBBS WACS_SBBS
#endif
#endif
#ifndef MY_ACS_LRCORNER
#ifdef WACS_LRCORNER
#define MY_ACS_LRCORNER WACS_LRCORNER
#endif
#endif
#ifndef MY_ACS_DBDD
#ifdef WACS_DBDD
#define MY_ACS_DBDD WACS_DBDD
#endif
#endif
#ifndef MY_ACS_SBSD
#ifdef WACS_SBSD
#define MY_ACS_SBSD WACS_SBSD
#endif
#endif
#ifndef MY_ACS_DBDS
#ifdef WACS_DBDS
#define MY_ACS_DBDS WACS_DBDS
#endif
#endif
#ifndef MY_ACS_SBSS
#ifdef WACS_SBSS
#define MY_ACS_SBSS WACS_SBSS
#endif
#endif
#ifndef MY_ACS_RTEE
#ifdef WACS_RTEE
#define MY_ACS_RTEE WACS_RTEE
#endif
#endif
#ifndef MY_ACS_DDDB
#ifdef WACS_DDDB
#define MY_ACS_DDDB WACS_DDDB
#endif
#endif
#ifndef MY_ACS_SDSB
#ifdef WACS_SDSB
#define MY_ACS_SDSB WACS_SDSB
#endif
#endif
#ifndef MY_ACS_DSDB
#ifdef WACS_DSDB
#define MY_ACS_DSDB WACS_DSDB
#endif
#endif
#ifndef MY_ACS_SSSB
#ifdef WACS_SSSB
#define MY_ACS_SSSB WACS_SSSB
#endif
#endif
#ifndef MY_ACS_LTEE
#ifdef WACS_LTEE
#define MY_ACS_LTEE WACS_LTEE
#endif
#endif
#ifndef MY_ACS_DDBD
#ifdef WACS_DDBD
#define MY_ACS_DDBD WACS_DDBD
#endif
#endif
#ifndef MY_ACS_SDBD
#ifdef WACS_SDBD
#define MY_ACS_SDBD WACS_SDBD
#endif
#endif
#ifndef MY_ACS_DSBS
#ifdef WACS_DSBS
#define MY_ACS_DSBS WACS_DSBS
#endif
#endif
#ifndef MY_ACS_SSBS
#ifdef WACS_SSBS
#define MY_ACS_SSBS WACS_SSBS
#endif
#endif
#ifndef MY_ACS_BTEE
#ifdef WACS_BTEE
#define MY_ACS_BTEE WACS_BTEE
#endif
#endif
#ifndef MY_ACS_BDDD
#ifdef WACS_BDDD
#define MY_ACS_BDDD WACS_BDDD
#endif
#endif
#ifndef MY_ACS_BDSD
#ifdef WACS_BDSD
#define MY_ACS_BDSD WACS_BDSD
#endif
#endif
#ifndef MY_ACS_BSDS
#ifdef WACS_BSDS
#define MY_ACS_BSDS WACS_BSDS
#endif
#endif
#ifndef MY_ACS_BSSS
#ifdef WACS_BSSS
#define MY_ACS_BSSS WACS_BSSS
#endif
#endif
#ifndef MY_ACS_TTEE
#ifdef WACS_TTEE
#define MY_ACS_TTEE WACS_TTEE
#endif
#endif
#ifndef MY_ACS_BDSS
#ifdef WACS_BDSS
#define MY_ACS_BDSS WACS_BDSS
#endif
#endif
#ifndef MY_ACS_BDSB
#ifdef WACS_BDSB
#define MY_ACS_BDSB WACS_BDSB
#endif
#endif
#ifndef MY_ACS_SSDB
#ifdef WACS_SSDB
#define MY_ACS_SSDB WACS_SSDB
#endif
#endif
#ifndef MY_ACS_BSDB
#ifdef WACS_BSDB
#define MY_ACS_BSDB WACS_BSDB
#endif
#endif
#ifndef MY_ACS_SDBS
#ifdef WACS_SDBS
#define MY_ACS_SDBS WACS_SDBS
#endif
#endif
#ifndef MY_ACS_SDBB
#ifdef WACS_SDBB
#define MY_ACS_SDBB WACS_SDBB
#endif
#endif
#ifndef MY_ACS_DSSB
#ifdef WACS_DSSB
#define MY_ACS_DSSB WACS_DSSB
#endif
#endif
#ifndef MY_ACS_DSBB
#ifdef WACS_DSBB
#define MY_ACS_DSBB WACS_DSBB
#endif
#endif
#ifndef MY_ACS_BSSD
#ifdef WACS_BSSD
#define MY_ACS_BSSD WACS_BSSD
#endif
#endif
#ifndef MY_ACS_BBSD
#ifdef WACS_BBSD
#define MY_ACS_BBSD WACS_BBSD
#endif
#endif
#ifndef MY_ACS_SBDS
#ifdef WACS_SBDS
#define MY_ACS_SBDS WACS_SBDS
#endif
#endif
#ifndef MY_ACS_BBDS
#ifdef WACS_BBDS
#define MY_ACS_BBDS WACS_BBDS
#endif
#endif
#ifndef MY_ACS_SSBD
#ifdef WACS_SSBD
#define MY_ACS_SSBD WACS_SSBD
#endif
#endif
#ifndef MY_ACS_SBBD
#ifdef WACS_SBBD
#define MY_ACS_SBBD WACS_SBBD
#endif
#endif
#ifndef MY_ACS_DBSS
#ifdef WACS_DBSS
#define MY_ACS_DBSS WACS_DBSS
#endif
#endif
#ifndef MY_ACS_DBBS
#ifdef WACS_DBBS
#define MY_ACS_DBBS WACS_DBBS
#endif
#endif
#ifndef MY_ACS_BDBD
#ifdef WACS_BDBD
#define MY_ACS_BDBD WACS_BDBD
#endif
#endif
#ifndef MY_ACS_BSBS
#ifdef WACS_BSBS
#define MY_ACS_BSBS WACS_BSBS
#endif
#endif
#ifndef MY_ACS_HLINE
#ifdef WACS_HLINE
#define MY_ACS_HLINE WACS_HLINE
#endif
#endif
#ifndef MY_ACS_DBDB
#ifdef WACS_DBDB
#define MY_ACS_DBDB WACS_DBDB
#endif
#endif
#ifndef MY_ACS_SBSB
#ifdef WACS_SBSB
#define MY_ACS_SBSB WACS_SBSB
#endif
#endif
#ifndef MY_ACS_VLINE
#ifdef WACS_VLINE
#define MY_ACS_VLINE WACS_VLINE
#endif
#endif
#ifndef MY_ACS_DDDD
#ifdef WACS_DDDD
#define MY_ACS_DDDD WACS_DDDD
#endif
#endif
#ifndef MY_ACS_SDSD
#ifdef WACS_SDSD
#define MY_ACS_SDSD WACS_SDSD
#endif
#endif
#ifndef MY_ACS_DSDS
#ifdef WACS_DSDS
#define MY_ACS_DSDS WACS_DSDS
#endif
#endif
#ifndef MY_ACS_SSSS
#ifdef WACS_SSSS
#define MY_ACS_SSSS WACS_SSSS
#endif
#endif
#ifndef MY_ACS_PLUS
#ifdef WACS_PLUS
#define MY_ACS_PLUS WACS_PLUS
#endif
#endif
#ifndef MY_ACS_DIAMOND
#ifdef WACS_DIAMOND
#define MY_ACS_DIAMOND WACS_DIAMOND
#endif
#endif
#ifndef MY_ACS_DEGREE
#ifdef WACS_DEGREE
#define MY_ACS_DEGREE WACS_DEGREE
#endif
#endif
#ifndef MY_ACS_BBBB
#ifdef WACS_BBBB
#define MY_ACS_BBBB WACS_BBBB
#endif
#endif
#ifndef MY_ACS_BULLET
#ifdef WACS_BULLET
#define MY_ACS_BULLET WACS_BULLET
#endif
#endif
#ifndef MY_ACS_DARROW
#ifdef WACS_DARROW
#define MY_ACS_DARROW WACS_DARROW
#endif
#endif
#ifndef MY_ACS_UARROW
#ifdef WACS_UARROW
#define MY_ACS_UARROW WACS_UARROW
#endif
#endif
#ifndef MY_ACS_LANTERN
#ifdef WACS_LANTERN
#define MY_ACS_LANTERN WACS_LANTERN
#endif
#endif
#ifndef MY_ACS_BOARD
#ifdef WACS_BOARD
#define MY_ACS_BOARD WACS_BOARD
#endif
#endif
#ifndef MY_ACS_CKBOARD
#ifdef WACS_CKBOARD
#define MY_ACS_CKBOARD WACS_CKBOARD
#endif
#endif
#ifndef MY_ACS_BLOCK
#ifdef WACS_BLOCK
#define MY_ACS_BLOCK WACS_BLOCK
#endif
#endif
#ifndef MY_ACS_LARROW
#ifdef WACS_LARROW
#define MY_ACS_LARROW WACS_LARROW
#endif
#endif
#ifndef MY_ACS_LEQUAL
#ifdef WACS_LEQUAL
#define MY_ACS_LEQUAL WACS_LEQUAL
#endif
#endif
#ifndef MY_ACS_RARROW
#ifdef WACS_RARROW
#define MY_ACS_RARROW WACS_RARROW
#endif
#endif
#ifndef MY_ACS_GEQUAL
#ifdef WACS_GEQUAL
#define MY_ACS_GEQUAL WACS_GEQUAL
#endif
#endif
#ifndef MY_ACS_PI
#ifdef WACS_PI
#define MY_ACS_PI WACS_PI
#endif
#endif
#ifndef MY_ACS_STERLING
#ifdef WACS_STERLING
#define MY_ACS_STERLING WACS_STERLING
#endif
#endif

#else /* ! USE_WIDEC_SUPPORT */

#ifndef MY_ACS_BDDB
#ifdef ACS_BDDB
#define MY_ACS_BDDB ACS_BDDB
#endif
#endif
#ifndef MY_ACS_BSSB
#ifdef ACS_BSSB
#define MY_ACS_BSSB ACS_BSSB
#endif
#endif
#ifndef MY_ACS_ULCORNER
#ifdef ACS_ULCORNER
#define MY_ACS_ULCORNER ACS_ULCORNER
#endif
#endif
#ifndef MY_ACS_DDBB
#ifdef ACS_DDBB
#define MY_ACS_DDBB ACS_DDBB
#endif
#endif
#ifndef MY_ACS_SSBB
#ifdef ACS_SSBB
#define MY_ACS_SSBB ACS_SSBB
#endif
#endif
#ifndef MY_ACS_LLCORNER
#ifdef ACS_LLCORNER
#define MY_ACS_LLCORNER ACS_LLCORNER
#endif
#endif
#ifndef MY_ACS_BBDD
#ifdef ACS_BBDD
#define MY_ACS_BBDD ACS_BBDD
#endif
#endif
#ifndef MY_ACS_BBSS
#ifdef ACS_BBSS
#define MY_ACS_BBSS ACS_BBSS
#endif
#endif
#ifndef MY_ACS_URCORNER
#ifdef ACS_URCORNER
#define MY_ACS_URCORNER ACS_URCORNER
#endif
#endif
#ifndef MY_ACS_DBBD
#ifdef ACS_DBBD
#define MY_ACS_DBBD ACS_DBBD
#endif
#endif
#ifndef MY_ACS_SBBS
#ifdef ACS_SBBS
#define MY_ACS_SBBS ACS_SBBS
#endif
#endif
#ifndef MY_ACS_LRCORNER
#ifdef ACS_LRCORNER
#define MY_ACS_LRCORNER ACS_LRCORNER
#endif
#endif
#ifndef MY_ACS_DBDD
#ifdef ACS_DBDD
#define MY_ACS_DBDD ACS_DBDD
#endif
#endif
#ifndef MY_ACS_SBSD
#ifdef ACS_SBSD
#define MY_ACS_SBSD ACS_SBSD
#endif
#endif
#ifndef MY_ACS_DBDS
#ifdef ACS_DBDS
#define MY_ACS_DBDS ACS_DBDS
#endif
#endif
#ifndef MY_ACS_SBSS
#ifdef ACS_SBSS
#define MY_ACS_SBSS ACS_SBSS
#endif
#endif
#ifndef MY_ACS_RTEE
#ifdef ACS_RTEE
#define MY_ACS_RTEE ACS_RTEE
#endif
#endif
#ifndef MY_ACS_DDDB
#ifdef ACS_DDDB
#define MY_ACS_DDDB ACS_DDDB
#endif
#endif
#ifndef MY_ACS_SDSB
#ifdef ACS_SDSB
#define MY_ACS_SDSB ACS_SDSB
#endif
#endif
#ifndef MY_ACS_DSDB
#ifdef ACS_DSDB
#define MY_ACS_DSDB ACS_DSDB
#endif
#endif
#ifndef MY_ACS_SSSB
#ifdef ACS_SSSB
#define MY_ACS_SSSB ACS_SSSB
#endif
#endif
#ifndef MY_ACS_LTEE
#ifdef ACS_LTEE
#define MY_ACS_LTEE ACS_LTEE
#endif
#endif
#ifndef MY_ACS_DDBD
#ifdef ACS_DDBD
#define MY_ACS_DDBD ACS_DDBD
#endif
#endif
#ifndef MY_ACS_SDBD
#ifdef ACS_SDBD
#define MY_ACS_SDBD ACS_SDBD
#endif
#endif
#ifndef MY_ACS_DSBS
#ifdef ACS_DSBS
#define MY_ACS_DSBS ACS_DSBS
#endif
#endif
#ifndef MY_ACS_SSBS
#ifdef ACS_SSBS
#define MY_ACS_SSBS ACS_SSBS
#endif
#endif
#ifndef MY_ACS_BTEE
#ifdef ACS_BTEE
#define MY_ACS_BTEE ACS_BTEE
#endif
#endif
#ifndef MY_ACS_BDDD
#ifdef ACS_BDDD
#define MY_ACS_BDDD ACS_BDDD
#endif
#endif
#ifndef MY_ACS_BDSD
#ifdef ACS_BDSD
#define MY_ACS_BDSD ACS_BDSD
#endif
#endif
#ifndef MY_ACS_BSDS
#ifdef ACS_BSDS
#define MY_ACS_BSDS ACS_BSDS
#endif
#endif
#ifndef MY_ACS_BSSS
#ifdef ACS_BSSS
#define MY_ACS_BSSS ACS_BSSS
#endif
#endif
#ifndef MY_ACS_TTEE
#ifdef ACS_TTEE
#define MY_ACS_TTEE ACS_TTEE
#endif
#endif
#ifndef MY_ACS_BDSS
#ifdef ACS_BDSS
#define MY_ACS_BDSS ACS_BDSS
#endif
#endif
#ifndef MY_ACS_BDSB
#ifdef ACS_BDSB
#define MY_ACS_BDSB ACS_BDSB
#endif
#endif
#ifndef MY_ACS_SSDB
#ifdef ACS_SSDB
#define MY_ACS_SSDB ACS_SSDB
#endif
#endif
#ifndef MY_ACS_BSDB
#ifdef ACS_BSDB
#define MY_ACS_BSDB ACS_BSDB
#endif
#endif
#ifndef MY_ACS_SDBS
#ifdef ACS_SDBS
#define MY_ACS_SDBS ACS_SDBS
#endif
#endif
#ifndef MY_ACS_SDBB
#ifdef ACS_SDBB
#define MY_ACS_SDBB ACS_SDBB
#endif
#endif
#ifndef MY_ACS_DSSB
#ifdef ACS_DSSB
#define MY_ACS_DSSB ACS_DSSB
#endif
#endif
#ifndef MY_ACS_DSBB
#ifdef ACS_DSBB
#define MY_ACS_DSBB ACS_DSBB
#endif
#endif
#ifndef MY_ACS_BSSD
#ifdef ACS_BSSD
#define MY_ACS_BSSD ACS_BSSD
#endif
#endif
#ifndef MY_ACS_BBSD
#ifdef ACS_BBSD
#define MY_ACS_BBSD ACS_BBSD
#endif
#endif
#ifndef MY_ACS_SBDS
#ifdef ACS_SBDS
#define MY_ACS_SBDS ACS_SBDS
#endif
#endif
#ifndef MY_ACS_BBDS
#ifdef ACS_BBDS
#define MY_ACS_BBDS ACS_BBDS
#endif
#endif
#ifndef MY_ACS_SSBD
#ifdef ACS_SSBD
#define MY_ACS_SSBD ACS_SSBD
#endif
#endif
#ifndef MY_ACS_SBBD
#ifdef ACS_SBBD
#define MY_ACS_SBBD ACS_SBBD
#endif
#endif
#ifndef MY_ACS_DBSS
#ifdef ACS_DBSS
#define MY_ACS_DBSS ACS_DBSS
#endif
#endif
#ifndef MY_ACS_DBBS
#ifdef ACS_DBBS
#define MY_ACS_DBBS ACS_DBBS
#endif
#endif
#ifndef MY_ACS_BDBD
#ifdef ACS_BDBD
#define MY_ACS_BDBD ACS_BDBD
#endif
#endif
#ifndef MY_ACS_BSBS
#ifdef ACS_BSBS
#define MY_ACS_BSBS ACS_BSBS
#endif
#endif
#ifndef MY_ACS_HLINE
#ifdef ACS_HLINE
#define MY_ACS_HLINE ACS_HLINE
#endif
#endif
#ifndef MY_ACS_DBDB
#ifdef ACS_DBDB
#define MY_ACS_DBDB ACS_DBDB
#endif
#endif
#ifndef MY_ACS_SBSB
#ifdef ACS_SBSB
#define MY_ACS_SBSB ACS_SBSB
#endif
#endif
#ifndef MY_ACS_VLINE
#ifdef ACS_VLINE
#define MY_ACS_VLINE ACS_VLINE
#endif
#endif
#ifndef MY_ACS_DDDD
#ifdef ACS_DDDD
#define MY_ACS_DDDD ACS_DDDD
#endif
#endif
#ifndef MY_ACS_SDSD
#ifdef ACS_SDSD
#define MY_ACS_SDSD ACS_SDSD
#endif
#endif
#ifndef MY_ACS_DSDS
#ifdef ACS_DSDS
#define MY_ACS_DSDS ACS_DSDS
#endif
#endif
#ifndef MY_ACS_SSSS
#ifdef ACS_SSSS
#define MY_ACS_SSSS ACS_SSSS
#endif
#endif
#ifndef MY_ACS_PLUS
#ifdef ACS_PLUS
#define MY_ACS_PLUS ACS_PLUS
#endif
#endif
#ifndef MY_ACS_DIAMOND
#ifdef ACS_DIAMOND
#define MY_ACS_DIAMOND ACS_DIAMOND
#endif
#endif
#ifndef MY_ACS_DEGREE
#ifdef ACS_DEGREE
#define MY_ACS_DEGREE ACS_DEGREE
#endif
#endif
#ifndef MY_ACS_BBBB
#ifdef ACS_BBBB
#define MY_ACS_BBBB ACS_BBBB
#endif
#endif
#ifndef MY_ACS_BULLET
#ifdef ACS_BULLET
#define MY_ACS_BULLET ACS_BULLET
#endif
#endif
#ifndef MY_ACS_DARROW
#ifdef ACS_DARROW
#define MY_ACS_DARROW ACS_DARROW
#endif
#endif
#ifndef MY_ACS_UARROW
#ifdef ACS_UARROW
#define MY_ACS_UARROW ACS_UARROW
#endif
#endif
#ifndef MY_ACS_LANTERN
#ifdef ACS_LANTERN
#define MY_ACS_LANTERN ACS_LANTERN
#endif
#endif
#ifndef MY_ACS_BOARD
#ifdef ACS_BOARD
#define MY_ACS_BOARD ACS_BOARD
#endif
#endif
#ifndef MY_ACS_CKBOARD
#ifdef ACS_CKBOARD
#define MY_ACS_CKBOARD ACS_CKBOARD
#endif
#endif
#ifndef MY_ACS_BLOCK
#ifdef ACS_BLOCK
#define MY_ACS_BLOCK ACS_BLOCK
#endif
#endif
#ifndef MY_ACS_LARROW
#ifdef ACS_LARROW
#define MY_ACS_LARROW ACS_LARROW
#endif
#endif
#ifndef MY_ACS_LEQUAL
#ifdef ACS_LEQUAL
#define MY_ACS_LEQUAL ACS_LEQUAL
#endif
#endif
#ifndef MY_ACS_RARROW
#ifdef ACS_RARROW
#define MY_ACS_RARROW ACS_RARROW
#endif
#endif
#ifndef MY_ACS_GEQUAL
#ifdef ACS_GEQUAL
#define MY_ACS_GEQUAL ACS_GEQUAL
#endif
#endif
#ifndef MY_ACS_PI
#ifdef ACS_PI
#define MY_ACS_PI ACS_PI
#endif
#endif
#ifndef MY_ACS_STERLING
#ifdef ACS_STERLING
#define MY_ACS_STERLING ACS_STERLING
#endif
#endif

#endif /* ! USE_WIDEC_SUPPORT */

#ifndef SIZE
#define SIZE "big"
#endif
static char SIZE_str[] = SIZE;
#undef SIZE
#define SIZE SIZE_str

#ifndef TILEFILE
#define TILEFILE "tile5x2.txt"
#endif

#ifdef BUILTIN_TILE
#undef TILEFILE
#define TILEFILE 0
#define tile_storage extern
tile_storage char *builtin_tilefile;
#else
static char TILEFILE_str[] = TILEFILE;
#undef TILEFILE
#define TILEFILE TILEFILE_str
#define tile_storage static
#define builtin_tilefile TILEFILE
#endif
#ifndef TILE_W
#define TILE_W 5
#endif
#ifndef TILE_H
#define TILE_H 2
#endif

#if (TILE_H + TILE_H) <= TILE_W
#define SQUARE 0
#else
#define SQUARE 1
#endif

tile_storage char tile[256 * TILE_H * TILE_W];
tile_storage int tile_used[256];
#if USE_COLOR
tile_storage int tile_color[256];
#endif

#ifndef STILEFILE
#define STILEFILE "stile7x3.txt"
#endif

#ifdef BUILTIN_STILE
#undef STILEFILE
#define STILEFILE 0
#define stile_storage extern
stile_storage char *builtin_stilefile;
#else
static char STILEFILE_str[] = STILEFILE;
#undef STILEFILE
#define STILEFILE STILEFILE_str
#define stile_storage static
#define builtin_stilefile STILEFILE
#endif
#ifndef STILE_W
#define STILE_W 7
#endif
#ifndef STILE_H
#define STILE_H 3
#endif

#ifndef VISIBLE_EYES
#define VISIBLE_EYES 1
#endif

stile_storage char stile[256 * STILE_H * STILE_W];
stile_storage int stile_used[256];
#if USE_COLOR
stile_storage int stile_color[256];
#endif

/* mapping from CP437 to VT-100 altcharset */
#if USE_WIDEC_SUPPORT
static cchar_t *
#else
static chtype
#endif
altcharset_cp437[256];

/* mapping from CP437 to ASCII */
static chtype
ascii_cp437[256];

#if USE_WIDEC_SUPPORT

/* for wcwidth(3) */
#include <wchar.h>

static int
my_wcswidth(const wchar_t *s, size_t n)
{
    int ret = 0;

    while ((n --) && (*s != L'\0'))
    {
        int cret = wcwidth(*s);
        if (cret != -1)
        {
            ret += cret;
        }
        s ++;
    }
    return ret;
}

#endif

static void
init_trans(int swapdots)
{
    int i;

    for (i = 0; i < 256; i ++)
        if (isprint(i))
        {
            altcharset_cp437[i] =
#if USE_WIDEC_SUPPORT
                NULL;
#endif
            ascii_cp437[i] = i;
        }
        else
        {
            altcharset_cp437[i] =
#if USE_WIDEC_SUPPORT
                NULL;
#endif
            ascii_cp437[i] =
                '\?';
        }
#if USE_ATTR
#ifdef MY_A_REVERSE
    if (! isprint(8))
    {
#if ! USE_WIDEC_SUPPORT
        altcharset_cp437[8] |= MY_A_REVERSE;
#endif
        ascii_cp437[8] |= MY_A_REVERSE;
    }
#endif
#endif
    altcharset_cp437[201] =
#ifdef MY_ACS_BDDB
        MY_ACS_BDDB;
#endif
    altcharset_cp437[218] =
#ifdef MY_ACS_BSSB
        MY_ACS_BSSB;
#else
#ifdef MY_ACS_ULCORNER
    MY_ACS_ULCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[201] =
        ascii_cp437[218] =
        '+';
    altcharset_cp437[200] =
#ifdef MY_ACS_DDBB
        MY_ACS_DDBB;
#endif
    altcharset_cp437[192] =
#ifdef MY_ACS_SSBB
        MY_ACS_SSBB;
#else
#ifdef MY_ACS_LLCORNER
    MY_ACS_LLCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[200] =
        ascii_cp437[192] =
        '+';
    altcharset_cp437[187] =
#ifdef MY_ACS_BBDD
        MY_ACS_BBDD;
#endif
    altcharset_cp437[191] =
#ifdef MY_ACS_BBSS
        MY_ACS_BBSS;
#else
#ifdef MY_ACS_URCORNER
    MY_ACS_URCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[187] =
        ascii_cp437[191] =
        '+';
    altcharset_cp437[188] =
#ifdef MY_ACS_DBBD
        MY_ACS_DBBD;
#endif
    altcharset_cp437[217] =
#ifdef MY_ACS_SBBS
        MY_ACS_SBBS;
#else
#ifdef MY_ACS_LRCORNER
    MY_ACS_LRCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[188] =
        ascii_cp437[217] =
        '+';
    altcharset_cp437[185] =
#ifdef MY_ACS_DBDD
        MY_ACS_DBDD;
#endif
    altcharset_cp437[181] =
#ifdef MY_ACS_SBSD
        MY_ACS_SBSD;
#endif
    altcharset_cp437[182] =
#ifdef MY_ACS_DBDS
        MY_ACS_DBDS;
#endif
    altcharset_cp437[180] =
#ifdef MY_ACS_SBSS
        MY_ACS_SBSS;
#else
#ifdef MY_ACS_RTEE
    MY_ACS_RTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[189] =
        ascii_cp437[183] =
        ascii_cp437[185] =
        ascii_cp437[181] =
        ascii_cp437[182] =
        ascii_cp437[180] =
        '+';
    altcharset_cp437[204] =
#ifdef MY_ACS_DDDB
        MY_ACS_DDDB;
#endif
    altcharset_cp437[198] =
#ifdef MY_ACS_SDSB
        MY_ACS_SDSB;
#endif
    altcharset_cp437[199] =
#ifdef MY_ACS_DSDB
        MY_ACS_DSDB;
#endif
    altcharset_cp437[195] =
#ifdef MY_ACS_SSSB
        MY_ACS_SSSB;
#else
#ifdef MY_ACS_LTEE
    MY_ACS_LTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[211] =
        ascii_cp437[214] =
        ascii_cp437[204] =
        ascii_cp437[198] =
        ascii_cp437[199] =
        ascii_cp437[195] =
        '+';
    altcharset_cp437[202] =
#ifdef MY_ACS_DDBD
        MY_ACS_DDBD;
#endif
    altcharset_cp437[207] =
#ifdef MY_ACS_SDBD
        MY_ACS_SDBD;
#endif
    altcharset_cp437[208] =
#ifdef MY_ACS_DSBS
        MY_ACS_DSBS;
#endif
    altcharset_cp437[193] =
#ifdef MY_ACS_SSBS
        MY_ACS_SSBS;
#else
#ifdef MY_ACS_BTEE
    MY_ACS_BTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[190] =
        ascii_cp437[212] =
        ascii_cp437[202] =
        ascii_cp437[207] =
        ascii_cp437[208] =
        ascii_cp437[193] =
        '+';
    altcharset_cp437[203] =
#ifdef MY_ACS_BDDD
        MY_ACS_BDDD;
#endif
    altcharset_cp437[209] =
#ifdef MY_ACS_BDSD
        MY_ACS_BDSD;
#endif
    altcharset_cp437[210] =
#ifdef MY_ACS_BSDS
        MY_ACS_BSDS;
#endif
    altcharset_cp437[194] =
#ifdef MY_ACS_BSSS
        MY_ACS_BSSS;
#else
#ifdef MY_ACS_TTEE
    MY_ACS_TTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[184] =
        ascii_cp437[213] =
        ascii_cp437[203] =
        ascii_cp437[209] =
        ascii_cp437[210] =
        ascii_cp437[194] =
        '+';
    altcharset_cp437[213] =
#ifdef MY_ACS_BDSS
        MY_ACS_BDSS;
#else
#ifdef MY_ACS_BDSB
    MY_ACS_BDSB;
#else
    altcharset_cp437[194];
#endif
#endif
    altcharset_cp437[214] =
#ifdef MY_ACS_SSDB
        MY_ACS_SSDB;
#else
#ifdef MY_ACS_BSDB
    MY_ACS_BSDB;
#else
    altcharset_cp437[195];
#endif
#endif
    altcharset_cp437[212] =
#ifdef MY_ACS_SDBS
        MY_ACS_SDBS;
#else
#ifdef MY_ACS_SDBB
    MY_ACS_SDBB;
#else
    altcharset_cp437[193];
#endif
#endif
    altcharset_cp437[211] =
#ifdef MY_ACS_DSSB
        MY_ACS_DSSB;
#else
#ifdef MY_ACS_DSBB
    MY_ACS_DSBB;
#else
    altcharset_cp437[195];
#endif
#endif
    altcharset_cp437[184] =
#ifdef MY_ACS_BSSD
        MY_ACS_BSSD;
#else
#ifdef MY_ACS_BBSD
    MY_ACS_BBSD;
#else
    altcharset_cp437[194];
#endif
#endif
    altcharset_cp437[183] =
#ifdef MY_ACS_SBDS
        MY_ACS_SBDS;
#else
#ifdef MY_ACS_BBDS
    MY_ACS_BBDS;
#else
    altcharset_cp437[180];
#endif
#endif
    altcharset_cp437[190] =
#ifdef MY_ACS_SSBD
        MY_ACS_SSBD;
#else
#ifdef MY_ACS_SBBD
    MY_ACS_SBBD;
#else
    altcharset_cp437[193];
#endif
#endif
    altcharset_cp437[189] =
#ifdef MY_ACS_DBSS
        MY_ACS_DBSS;
#else
#ifdef MY_ACS_DBBS
    MY_ACS_DBBS;
#else
    altcharset_cp437[180];
#endif
#endif
    altcharset_cp437[205] =
#ifdef MY_ACS_BDBD
        MY_ACS_BDBD;
#endif
    altcharset_cp437[196] =
#ifdef MY_ACS_BSBS
        MY_ACS_BSBS;
#else
#ifdef MY_ACS_HLINE
    MY_ACS_HLINE;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[205] =
        ascii_cp437[196] =
        '-';
    altcharset_cp437[186] =
#ifdef MY_ACS_DBDB
        MY_ACS_DBDB;
#endif
    altcharset_cp437[179] =
#ifdef MY_ACS_SBSB
        MY_ACS_SBSB;
#else
#ifdef MY_ACS_VLINE
    MY_ACS_VLINE;
#endif
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[186] =
        ascii_cp437[179] =
        '|';
    altcharset_cp437[206] =
#ifdef MY_ACS_DDDD
        MY_ACS_DDDD;
#endif
    altcharset_cp437[215] =
#ifdef MY_ACS_SDSD
        MY_ACS_SDSD;
#endif
    altcharset_cp437[216] =
#ifdef MY_ACS_DSDS
        MY_ACS_DSDS;
#endif
    altcharset_cp437[197] =
#ifdef MY_ACS_SSSS
        MY_ACS_SSSS;
#else
#ifdef MY_ACS_PLUS
    MY_ACS_PLUS;
#endif
#endif
    altcharset_cp437[4] =
#ifdef MY_ACS_DIAMOND
        MY_ACS_DIAMOND;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[206] =
        ascii_cp437[215] =
        ascii_cp437[216] =
        ascii_cp437[197] =
        ascii_cp437[4] =
        '+';
    altcharset_cp437[248] =
#ifdef MY_ACS_DEGREE
        MY_ACS_DEGREE;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[248] =
        '\'';
    altcharset_cp437[241] =
#ifdef  MY_ACS_PLMINUS
        MY_ACS_PLMINUS;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[241] =
        '#';
    altcharset_cp437[7] =
#ifdef MY_ACS_BBBB
        MY_ACS_BBBB;
#endif
#if ! USE_WIDEC_SUPPORT
    altcharset_cp437[8] =
#endif
        altcharset_cp437[9] =
        altcharset_cp437[254] =
#ifdef MY_ACS_BULLET
        MY_ACS_BULLET;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[7] =
        ascii_cp437[8] =
        ascii_cp437[9] =
        ascii_cp437[254] =
        'o';
#if USE_ATTR
#ifdef MY_A_REVERSE
#if ! USE_WIDEC_SUPPORT
    altcharset_cp437[8] |= MY_A_REVERSE;
#endif
    ascii_cp437[8] |= MY_A_REVERSE;
#endif
#endif
    altcharset_cp437[25] =
        altcharset_cp437[31] =
#ifdef MY_ACS_DARROW
        MY_ACS_DARROW;
#else
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[25] =
        ascii_cp437[31] =
        'v';
#endif
    altcharset_cp437[24] =
        altcharset_cp437[30] =
#ifdef MY_ACS_UARROW
        MY_ACS_UARROW;
#else
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[24] =
        ascii_cp437[30] =
        '^';
#endif
    altcharset_cp437[15] =
#ifdef MY_ACS_LANTERN
        MY_ACS_LANTERN;
#endif
    altcharset_cp437[176] =
#ifdef MY_ACS_BOARD
        MY_ACS_BOARD;
#endif
    altcharset_cp437[177] =
        altcharset_cp437[178] =
#ifdef MY_ACS_CKBOARD
        MY_ACS_CKBOARD;
#endif
    altcharset_cp437[10] =
        altcharset_cp437[219] =
#ifdef MY_ACS_BLOCK
        MY_ACS_BLOCK;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[15] =
        ascii_cp437[176] =
        ascii_cp437[177] =
        ascii_cp437[178] =
        ascii_cp437[10] =
        ascii_cp437[219] =
        '#';
    altcharset_cp437[27] =
        altcharset_cp437[17] =
#ifdef MY_ACS_LARROW
        MY_ACS_LARROW;
#endif
    altcharset_cp437[174] =
        altcharset_cp437[243] =
#ifdef MY_ACS_LEQUAL
        MY_ACS_LEQUAL;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[174] =
        ascii_cp437[243] =
        ascii_cp437[27] =
        ascii_cp437[17] =
        '<';
    altcharset_cp437[26] =
        altcharset_cp437[16] =
#ifdef MY_ACS_RARROW
        MY_ACS_RARROW;
#endif
    altcharset_cp437[175] =
        altcharset_cp437[242] =
#ifdef MY_ACS_GEQUAL
        MY_ACS_GEQUAL;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[175] =
        ascii_cp437[242] =
        ascii_cp437[26] =
        ascii_cp437[16] =
        '>';
    altcharset_cp437[227] =
#ifdef MY_ACS_PI
        MY_ACS_PI;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[227] =
        '*';
    altcharset_cp437[156] =
#ifdef MY_ACS_STERLING
        MY_ACS_STERLING;
#endif
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = NULL;
#endif
    ascii_cp437[156] =
        'f';
#if ! USE_WIDEC_SUPPORT
    altcharset_cp437[0] =
#endif
        ascii_cp437[0] =
        ' ';
#if ! USE_WIDEC_SUPPORT
    altcharset_cp437[240] =
#endif
        ascii_cp437[240] =
        '=';
#if ! USE_WIDEC_SUPPORT
    altcharset_cp437[247] =
#endif
        ascii_cp437[247] =
        '=';
    ascii_cp437[249] =
        ascii_cp437[250] =
        '.';
    ascii_cp437[254] =
        'o';
    if (swapdots)
    {
        altcharset_cp437[249] =
            altcharset_cp437[250] = altcharset_cp437[254];
        altcharset_cp437[254] =
#if USE_WIDEC_SUPPORT
            NULL;
#else
        'o';
#endif
    }
    else
    {
        altcharset_cp437[249] =
            altcharset_cp437[250] =
#if USE_WIDEC_SUPPORT
            NULL;
#else
        '.';
#endif
    }
    altcharset_cp437[255] =
#if USE_WIDEC_SUPPORT
        NULL;
#endif
    ascii_cp437[255] =
        ' ';
}

static int use_raw = USE_RAW;

static int use_raw_ucs = USE_RAW_UCS;

static int use_acs = USE_ACS;

static int use_dim_and_bright = 1;

#ifndef VARIANT
#define VARIANT "myman"
#endif
static char VARIANT_str[] = VARIANT;
#undef VARIANT
#define VARIANT VARIANT_str


#ifndef MAZEFILE
#define MAZEFILE "maze.txt"
#endif

#ifdef BUILTIN_MAZE
#undef MAZEFILE
#define MAZEFILE 0
extern char *builtin_mazefile;
#else
static char MAZEFILE_str[] = MAZEFILE;
#undef MAZEFILE
#define MAZEFILE MAZEFILE_str
#define builtin_mazefile MAZEFILE
#endif
#ifndef MAZE_W
#define MAZE_W 28
#endif
#ifndef MAZE_H
#define MAZE_H 31
#endif
#ifndef MAZE_N
#define MAZE_N 1
#endif
#ifndef MSGLEN
#define MSGLEN 10
#endif
#ifndef DOTS
#define DOTS 244
#endif
#ifndef DOTS_FRUIT1
#define DOTS_FRUIT1 70
#endif
#ifndef DOTS_FRUIT2
#define DOTS_FRUIT2 140
#endif

#ifdef BUILTIN_MAZE
extern char *maze_data[MAZE_N][MAZE_H];
#endif
static char maze[MAZE_N][MAZE_H][MAZE_W + 1];
static char blank_maze[MAZE_N][MAZE_H][MAZE_W + 1];
static char msgbuf[MSGLEN], msgbuf2[MSGLEN];

#define SPRITES 16

static unsigned char sprite[SPRITES];
static unsigned char sprite_frame[SPRITES];
static int sprite_x[SPRITES], sprite_y[SPRITES];
static int sprite_used[SPRITES];
static int sprite_timer[SPRITES];
#if USE_COLOR
static int sprite_color[SPRITES];
#endif

#ifndef GHOSTS
#define GHOSTS 4
#endif

static int ghost_dir[GHOSTS], ghost_mem[GHOSTS], ghost_man[GHOSTS], ghost_timer[GHOSTS];
static unsigned char home_dir[GHOSTS][MAZE_H][MAZE_W + 1];

#define WHOSE_HOME_DIR(r,c) (home_dir[PINKY][(r)][(c)] ? PINKY \
: home_dir[INKY][(r)][(c)] ? INKY \
: home_dir[CLYDE][(r)][(c)] ? CLYDE \
: BLINKY)

#define HOME_DIR(r,c) (home_dir[WHOSE_HOME_DIR((r),(c))][(r)][(c)])

/* sprite numbers */
#define INKY 0
#define BLINKY 1
#define PINKY 2
#define CLYDE 3
#define MEAN(ghost) ((ghost) + 4)
#define HERO 8
#define BLUE(ghost) ((ghost) + 9)
#define FRUIT 13
#define GHOST_SCORE 14
#define FRUIT_SCORE 15

#define STILE_FRUIT 0
#define STILE_MEAN 8
#define STILE_EYES 10
#define STILE_BLUE 14
#define STILE_HERO 16
#define STILE_FRUIT_SCORE 41
#define STILE_200 49
#define STILE_400 50
#define STILE_800 51
#define STILE_1600 52
#define STILE_WHITE 53

#define XTILE(x) ((x) / TILE_W)
#define YTILE(y) ((y) / TILE_H)

#define PIX_W ((MAZE_W + 1) * TILE_W)
#define PIX_H (MAZE_H * TILE_H)

#define XPIXWRAP(x) (((x) + PIX_W) % PIX_W)
#define YPIXWRAP(y) (((y) + PIX_H) % PIX_H)

#define XWRAP(x) (((x) + MAZE_W + 1) % (MAZE_W + 1))
#define YWRAP(y) (((y) + MAZE_H) % MAZE_H)

#define ISDOT(c) (((c) == 249) || ((c) == '.'))
#define ISPELLET(c) (((c) == 254) || ((c) == 'o'))
#define ISOPEN(c) (((c) == ' ') || ISDOT(c) || ISPELLET(c) || \
((c) == '!') || (((c) >= 'A') && ((c) <= 'Z')))
#define ISDOOR(c) (((c) == '=') || (c == ':') || \
((c) == 240) || (c == 255))

#define NOTTOP(y) (y >= (TILE_H / 2))
#define NOTBOTTOM(y) (y <= (TILE_H / 2))
#define NOTLEFT(x) (x >= (TILE_W / 2))
#define NOTRIGHT(x) (x <= (TILE_W / 2))

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4

#define DIRWRAP(dir) (((dir) + 3) % 4 + 1)

#define XDIR(dir) (((dir) == RIGHT) - ((dir) == LEFT))
#define YDIR(dir) (((dir) == DOWN) - ((dir) == UP))

#define YLEAVING(dir, y) ( -(((dir) == UP) && ! NOTTOP(y)) + \
(((dir) == DOWN) && ! NOTBOTTOM(y)))
#define XLEAVING(dir, x) ( -(((dir) == LEFT) && ! NOTLEFT(x)) + \
(((dir) == RIGHT) && ! NOTRIGHT(x)))

#ifndef CHERO
#define CHERO (MAZE_W * 0.5)
#endif
#define XHERO (CHERO * TILE_W)

#ifndef RHERO
#define RHERO 23.5
#endif
#define YHERO (RHERO * TILE_H)

#ifndef CFRUIT
#define CFRUIT CHERO
#endif
#define XFRUIT (CFRUIT * TILE_W)

#ifndef RFRUIT
#define RFRUIT 17.5
#endif
#define YFRUIT (RFRUIT * TILE_H)

#ifndef RGHOST
#define RGHOST (RFRUIT - 3.0)
#endif
#define YGHOST (RGHOST * TILE_H)

#ifndef CGHOST
#define CGHOST CHERO
#endif
#define XGHOST (CGHOST * TILE_W)

#ifndef COGHOST
#define COGHOST 2
#endif

#ifndef ROGHOST
#define ROGHOST 3
#endif

#ifndef RTOP
#define RTOP (RFRUIT - 5.0)
#endif
#define YTOP (RTOP * TILE_H)

#ifndef CMSG
#define CMSG 9
#endif

#ifndef RMSG
#define RMSG 17
#endif

#ifndef CMSG2
#define CMSG2 9
#endif

#ifndef RMSG2
#define RMSG2 11
#endif

#ifndef PLAYER1
#define PLAYER1  "PLAYER ONE"
#endif

#ifndef PLAYER2
#define PLAYER2  "PLAYER TWO"
#endif

#define PLAYER(n) \
(((n) == 1) \
? PLAYER1 \
: PLAYER2)

#ifndef READY
#define READY    "  READY!  "
#endif

#ifndef GAMEOVER
#define GAMEOVER "GAME  OVER"
#endif

#ifndef PAUSE
#define PAUSE " - PAUSED - "
#endif

#define BONUS(n) \
(((n) < 4) \
? ((n) - 1) \
: (((n) < 15) \
? ((n) + 1) / 2 \
: 7))

static int bonus_score[8] = {100, 300, 500, 700, 1000, 2000, 3000, 5000};

#ifndef DELAY
#define DELAY 200000
#endif

#ifndef SPEEDUP
#define SPEEDUP 1000
#endif

static int
readfont(char *fontfile, int w, int h, char *font, int *used
#if USE_COLOR
             , int *color
#endif
    )
{
    FILE *infile;
    int c, i, j, k;
    int rw, rh;
    char X;

    for (i = 0; i < 256; i ++) {
        used[i] = 0;
#if USE_COLOR
        color[i] = 0;
#endif
    }
    if (! (infile = fopen(fontfile, "rb"))) {
        perror(fontfile);
        return 1;
    }
    if ((fscanf(infile, "%d%c%d", &rw, &X, &rh) != 3) ||
        (tolower(X) != 'x')) {
        fprintf(stderr, "%s: can't find a dimension specification WxH\n",
                fontfile);
        return 1;
    }
    if ((rw != w) || (rh != h)) {
        fprintf(stderr, "%s: dimension specification %dx%d doesn't match %dx%d\n",
                fontfile, rw, rh, w, h);
        return 1;
    }
    while (! feof(infile)) {
        if (fscanf(infile, "%x", &i) != 1) {
            if (! feof(infile)) {
                fprintf(stderr, "%s: can't find an index\n",
                        fontfile);
                return 1;
            }
            continue;
        }
        if ((i < 0) || (i > 255)) {
            fprintf(stderr, "%s: invalid index %2.2X ignored\n", fontfile, i);
            continue;
        }
        if (used[i])
            fprintf(stderr, "%s: duplicate definition for %2.2X\n", fontfile, i);
        used[i] = 1;
#if USE_COLOR
        if ((c = fgetc(infile)) == '~') {
            if (fscanf(infile, "%x", &c) != 1) {
                if (! feof(infile)) {
                    fprintf(stderr, "%s: can't find a color for index %2.2X\n",
                            fontfile, i);
                    return 1;
                }
                continue;
            }
            if ((c < 0) || (c > 15))
                fprintf(stderr, "%s: invalid color %2.2X ignored\n", fontfile, c);
            else
                color[i] = c;
        }
        else
            ungetc(c, infile);
#endif
        for (j = 0; j < h; j ++)
            for (k = 0; k < w; k ++)
                font[(i * h + j) * w + k] = ' ';
        for (j = 0; j < h; j ++) {
            while ((c = fgetc(infile)) != ':')
                if (c == EOF) {
                    if (! feof(infile))
                        perror(fontfile);
                    else
                        fprintf(stderr, "%s: premature EOF in index %2.2X\n",
                                fontfile, i);
                    return 1;
                }
            if ((c = fgetc(infile)) == EOF)
                if (! feof(infile)) {
                    perror(fontfile);
                    return 1;
                }
            for (k = 0;
                 (k < w) &&
                     (c != '\v') && (c != '\f') &&
                     (c != '\n') && (c != '\r') && ! feof(infile);
                 k ++) {
                font[(i * h + j) * w + k] = c;
                if ((c = fgetc(infile)) == EOF) {
                    if (feof(infile))
                        continue;
                    perror(fontfile);
                    return 1;
                }
            }
            while ((c != '\v') && (c != '\f') &&
                   (c != '\n') && (c != '\r') && ! feof(infile)) {
                if (c == EOF) {
                    if (feof(infile))
                        continue;
                    perror(fontfile);
                    return 1;
                }
                c = fgetc(infile);
            }
        }
    }
    fclose(infile);
    return 0;
}

#define collide(s1, s2) ((XTILE(sprite_x[s1] - STILE_W / 2) == XTILE(sprite_x[s2] - STILE_W / 2)) \
&& (YTILE(sprite_y[s1] - STILE_H / 2) == YTILE(sprite_y[s2] - STILE_H / 2)))

/* color palette for USE_PALETTE and HTML snapshots */
static short
pen_pal[16][3] =
{
/*{ red,green, blue} */
    {   0,    0,    0}, /* 0: black */
    {   0,    0,  867}, /* 1: blue */
    {   0,  718,    0}, /* 2: green (peach stem) */
    {   0,  867,  867}, /* 3: cyan */
    { 867,  589,  277}, /* 4: brown (apple/cherry stem) */
    {1000,  128,  589}, /* 5: magenta (mypal trim?) */
    {1000,  718,  277}, /* 6: salmon (clyde, peach, PUSH START BUTTON) */
    {1000,  718,  589}, /* 7: light yellow (dot, blue face, BONUS MYMAN FOR x Pts) */
    { 400,  400,  400}, /* 8: grey */
    { 128,  128,  867}, /* 9: light blue (wall, blue ghost) */
    { 589, 1000,    0}, /* A: light green (mypal?, super dot?) */
    {   0, 1000,  867}, /* B: light cyan (inky, key top, 1 PLAYER ONLY, ghost scores) */
    {1000,    0,    0}, /* C: light red (blinky, apple/cherry, GAME  OVER) */
    {1000,  718,  867}, /* D: pink (pinky, door, NEW MAN - X, fruit scores) */
    {1000, 1000,    0}, /* E: yellow (myman, ship) */
    { 867,  867,  867}, /* F: light grey (text, eye, apple/cherry shine, key, bell) */
};

static chtype
pen[16];

#if USE_COLOR

#define trans_color(i) \
((i == 0) ? COLOR_BLACK \
: (i == 1) ? COLOR_BLUE \
: (i == 2) ? COLOR_GREEN \
: (i == 3) ? COLOR_CYAN \
: (i == 4) ? COLOR_RED \
: (i == 5) ? COLOR_MAGENTA \
: (i == 6) ? COLOR_YELLOW \
: COLOR_WHITE)

#if USE_ATTR
#ifndef PEN_BRIGHT
#ifdef MY_A_BOLD
#define PEN_BRIGHT MY_A_BOLD
#endif
#endif
#ifndef PEN_DIM
#ifdef MY_A_DIM
#define PEN_DIM MY_A_DIM
#endif
#endif
#endif

#ifndef PEN_BRIGHT
#define PEN_BRIGHT 0
#endif

#ifndef PEN_DIM
#define PEN_DIM 0
#endif

#if USE_PALETTE
static short old_pal[16][3];
#endif

static short old_pair[16][2];

#ifndef MY_INIT_PAIR_RET
#define MY_INIT_PAIR_RET
#endif
#define my_init_pair(x,y,z) init_pair(x,y,z) MY_INIT_PAIR_RET

static void
destroy_pen(void)
{
    int i;

#if USE_PALETTE
    if (can_change_color() && (COLORS >= 16) && (COLOR_PAIRS >= 16)) {
        for (i = 0; i < 16; i ++)
            init_color(i, old_pal[i][0], old_pal[i][1], old_pal[i][2]);
    }
#endif
    for (i = 0; i < 16; i ++)
        if (i < COLOR_PAIRS)
            my_init_pair(i, old_pair[i][0], old_pair[i][1]);
}

static void
init_pen(void)
{
    int i;

    for (i = 0; i < 16; i ++)
        if (i < COLOR_PAIRS)
            pair_content(i, old_pair[i], old_pair[i] + 1);
#if USE_PALETTE
    if (can_change_color() && (COLORS >= 16) && (COLOR_PAIRS >= 16)) {
        for (i = 0; i < 16; i ++)
            color_content(i, old_pal[i], old_pal[i] + 1, old_pal[i] + 2);
        for (i = 0; i < 16; i ++) {
            init_color(i,
                       pen_pal[i][0],
                       pen_pal[i][1],
                       pen_pal[i][2]);
            my_init_pair(i, i, 0);
            pen[i] = COLOR_PAIR(i);
        }
    }
    else
#endif
    {
        for (i = 0; i < 8; i ++) {
            if (i && (i < COLOR_PAIRS) &&
                (my_init_pair(i, trans_color(i), COLOR_BLACK) != ERR)) {
                pen[i] = COLOR_PAIR(i);
                pen[8 + i] = COLOR_PAIR(i) |
                    (use_dim_and_bright ? PEN_BRIGHT : 0);
            }
            else
            {
                pen[i] = COLOR_PAIR(0);
                pen[8 + i] = pen[i] |
                    (use_dim_and_bright
                     ? ((i == 8) ? PEN_DIM : PEN_BRIGHT)
                     : 0);
            }
        }
    }
}

#endif

/* UCS/Unicode mapping for CP437 (used in HTML snapshots) */

static unsigned long
uni_cp437[256] =
{
/* 0x00 */
    0x2008, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
    0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,
/* 0x10 */
    0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
    0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,
/* 0x20 */
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
/* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
/* 0x40 */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
/* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
/* 0x60 */
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
/* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x2302,
/* 0x80 */
    0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
    0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
/* 0x90 */
    0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
    0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
/* 0xA0 */
    0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
    0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
/* 0xB0 */
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
/* 0xC0 */
    0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
    0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
/* 0xD0 */
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
    0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
/* 0xE0 */
    0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
    0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
/* 0xF0 */
    0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
    0x00B0, 0x00B7, 0x2022, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

/* alternate UCS/Unicode mapping used in CJK fullwidth mode */

static unsigned long
uni_cp437_fullwidth[256] =
{
/* 0x00 */
    0x3000, 0x25CB, 0x25CF, 0x25CF, 0x25CF, 0x25A0, 0x25A0, 0x25CF,
    0x25A0, 0x25CB, 0x25A0, 0x2642, 0x2640, 0xFF03, 0xFF03, 0x2606,
/* 0x10 */
    0x2605, 0x2605, 0xFF29, 0xFF01, 0xFF30, 0x00A7, 0x25A0, 0xFF2C,
    0x2191, 0x2193, 0x2192, 0x2190, 0xFF2C, 0x2500, 0x25B2, 0x25BC,
/* 0x20 */
    0x3000, 0xFF01, 0x201D, 0xFF03, 0xFF04, 0xFF05, 0xFF06, 0x2019,
    0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0x2500, 0xFF0E, 0xFF0F,
/* 0x30 */
    0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17,
    0xFF18, 0xFF19, 0xFF1A, 0xFF1B, 0xFF1C, 0xFF1D, 0xFF1E, 0xFF1F,
/* 0x40 */
    0xFF20, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27,
    0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F,
/* 0x50 */
    0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37,
    0xFF38, 0xFF39, 0xFF3A, 0xFF3B, 0xFF3C, 0xFF3D, 0xFF3E, 0xFF3F,
/* 0x60 */
    0x2018, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47,
    0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F,
/* 0x70 */
    0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57,
    0xFF58, 0xFF59, 0xFF5A, 0xFF5B, 0xFF5C, 0xFF5D, 0x301C, 0x0394,
/* 0x80 */
    0xFF23, 0xFF55, 0xFF45, 0xFF41, 0xFF41, 0xFF41, 0xFF41, 0xFF43,
    0xFF45, 0xFF45, 0xFF45, 0xFF49, 0xFF49, 0xFF49, 0xFF21, 0xFF21,
/* 0x90 */
    0xFF25, 0xFF45, 0xFF25, 0xFF4F, 0xFF4F, 0xFF4F, 0xFF55, 0xFF55,
    0xFF59, 0xFF2F, 0xFF35, 0xFFE0, 0xFFE1, 0xFFE5, 0xFF50, 0xFF46,
/* 0xA0 */
    0xFF41, 0xFF49, 0xFF4F, 0xFF55, 0xFF4E, 0xFF2E, 0xFF41, 0xFF4F,
    0xFF1F, 0x250C, 0xFFE2, 0x00BD, 0x00BC, 0xFF01, 0x00AB, 0x00BB,
/* 0xB0 */
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2524, 0x2524, 0x2510,
    0x2510, 0x2524, 0x2502, 0x2510, 0x2518, 0x2518, 0x2518, 0x2510,
/* 0xC0 */
    0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x251C, 0x251C,
    0x2514, 0x250C, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x2534,
/* 0xD0 */
    0x2534, 0x252C, 0x252C, 0x2514, 0x2514, 0x250C, 0x250C, 0x253C,
    0x253C, 0x2518, 0x250C, 0x25A0, 0x25A0, 0x25A0, 0x25A0, 0x25A0,
/* 0xE0 */
    0x03B1, 0x03B2, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x03BC, 0x03C4,
    0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
/* 0xF0 */
    0x2261, 0x00B1, 0xFF1E, 0xFF1C, 0x222B, 0x222B, 0x00F7, 0xFF1D,
    0x00B0, 0x30FB, 0x30FB, 0x221A, 0xFF4E, 0xFF12, 0x25CF, 0x3000
};

#define CJK_MODE (uni_cp437[0x20] == uni_cp437_fullwidth[0x20])

/* wrappers around some curses functions to allow raw CP437-mode and
 * snapshots; note that these wrappers support only a small subset of
 * the corresponding curses behavior */

static FILE *snapshot = 0;
static int snapshot_x = 0;
static int snapshot_y = 0;
static chtype snapshot_attrs = 0;
static int snapshot_colorize = 0;

/* simulate a subset of curses attributes in HTML; note that this
 * generates presentational markup (<font color="...">, <u>, <b>,
 * etc.) which is considered deprecated in modern HTML; however there
 * is really no acceptable alternative since this markup needs to look
 * colorful even in older browsers */
static void
snapshot_attrset(chtype attrs)
{
    if (! snapshot)
    {
        return;
    }
    if (snapshot_attrs != attrs)
    {
        int i = 16;

        if (snapshot_colorize)
        {
            for (i = 0; i < 16; i ++)
            {
                if (pen[i] &&
                    snapshot_attrs == pen[i])
                {
                    fprintf(snapshot,
                            "</font>");
                    break;
                }
            }
        }
#ifdef MY_A_BOLD
        if (i == 16)
        {
            if (snapshot_attrs & MY_A_BOLD)
            {
                fprintf(snapshot,
                        "</b>");
            }
        }
#endif
#ifdef MY_A_UNDERLINE
        if (i == 16)
        {
            if (snapshot_attrs & MY_A_UNDERLINE)
            {
                fprintf(snapshot,
                        "</u>");
            }
        }
#endif
        snapshot_attrs = attrs;
        if (snapshot_colorize)
        {
            for (i = 0; i < 16; i ++)
            {
                if (pen[i] &&
                    snapshot_attrs == pen[i])
                {
                    unsigned long r, g, b;

                    r = (255 * pen_pal[i][0]) / 1000;
                    g = (255 * pen_pal[i][1]) / 1000;
                    b = (255 * pen_pal[i][2]) / 1000;
                    fprintf(snapshot,
                            "<font color=\"#%2.2lX%2.2lX%2.2lX\">",
                            r, g, b);
                    break;
                }
            }
        }
#ifdef MY_A_UNDERLINE
        if (i == 16)
        {
            if (snapshot_attrs & MY_A_UNDERLINE)
            {
                fprintf(snapshot,
                        "<u>");
            }
        }
#endif
#ifdef MY_A_BOLD
        if (i == 16)
        {
            if (snapshot_attrs & MY_A_BOLD)
            {
                fprintf(snapshot,
                        "<b>");
            }
        }
#endif
    }
}

static int
my_erase(void)
{
    if (snapshot)
    {
        char *locale = "en";
        int locale_dynamic;

        locale = setlocale(LC_CTYPE, "");
        if ((! locale) || (! *locale))
        {
            locale = "en";
        }
        locale = strdup(locale);
        if (! locale)
        {
            locale = "en";
            locale_dynamic = 0;
        }
        else
        {
            int i;

            locale_dynamic = 1;
            for (i = 0; locale[i]; i ++)
            {
                if (locale[i] == '_')
                {
                    locale[i] = '-';
                    continue;
                }
                if ((locale[i] == '.')
                    ||
                    (locale[i] == '@'))
                {
                    locale[i] = '\0';
                    break;
                }
            }
            if ((! locale[0])
                ||
                (! strcmp(locale, "C"))
                ||
                (! strcmp(locale, "POSIX")))
            {
                free(locale);
                locale_dynamic = 0;
                locale = "en";
            }
        }
        snapshot_x = 0;
        snapshot_y = 0;
        snapshot_attrs = 0;
        fprintf(
            snapshot,
            "%s"
            CRLF
            "%s"
            CRLF
            "<html xmlns=\"%s\" xml:lang=\"%s\" lang=\"%s\">"
            CRLF
            "<head>"
            CRLF
            "<meta name=\"generator\" content=\"%s %s\" />"
            CRLF
            "<meta http-equiv=\"Content-type\" content=\"%s\" />"
            CRLF
            "<title>%s</title>"
            CRLF
            "</head>"
            CRLF
            "<body%s%s><pre><font face=\"%s\">"
            CRLF,

            ((use_acs && use_raw && ! use_raw_ucs)
             ? "<\?xml version=\"1.0\" encoding=\"CP437\"\?>"
             : "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>"),

            "<!DOCTYPE html"
            CRLF
            "     PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
            CRLF
            "    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">",

            "http://www.w3.org/1999/xhtml",

            locale, locale,

            MYMAN, VERSION,

            ((use_acs && use_raw && ! use_raw_ucs)
             ? "text/html; charset=CP437"
             : "text/html; charset=UTF-8"),

            "MyMan Screenshot [" MYMAN " " VERSION "]",

            snapshot_colorize ? " text=\"white\"" : "",
            snapshot_colorize ? " bgcolor=\"black\"" : "",

            "Terminal, vga, monospace, mono, courier");
        if (locale)
        {
            if (locale_dynamic)
            {
                free(locale);
            }
            setlocale(LC_CTYPE, locale);
        }
    }

    return erase();
}

static int
my_refresh(void)
{
    if (snapshot)
    {
        fprintf(snapshot,
                CRLF
                "</font></pre></body></html>"
                CRLF);
        fflush(snapshot);
        fclose(snapshot);
        snapshot = (FILE *) 0;
    }
    return refresh();
}

static void
my_move(int y, int x)
{
    if (snapshot)
    {
        while ((y > snapshot_y) || (x < snapshot_x))
        {
            fprintf(snapshot,
                    CRLF);
            snapshot_y ++;
            snapshot_x = 0;
        }
        while (x > snapshot_x)
        {
            fprintf(snapshot,
                    " ");
            snapshot_x ++;
        }
    }

    do
    {
        int cur_y, cur_x;

        getyx(stdscr, cur_y, cur_x);
        if ((y != cur_y) || (x != cur_x))
        {
            move(y, x);
        }
    }
    while(0);
}

static int
my_attrset(chtype attrs)
{
    snapshot_attrset(attrs);
#if USE_ATTR || USE_COLOR
    attrs ^= snapshot ?
#ifdef MY_A_REVERSE
      MY_A_REVERSE
#else
      0
#endif
      : 0;
    attrset(attrs);
#endif
    return 1;
}

/* add a cp437 string to the HTML snapshot */
static void
snapshot_addch(unsigned long inbyte)
{
    if (snapshot)
    {
        unsigned long codepoint;

        if (use_acs)
        {
            codepoint = uni_cp437[inbyte];
            if (CJK_MODE)
            {
#if USE_WIDEC_SUPPORT
                snapshot_x += wcwidth(codepoint) - 1;
#else
                snapshot_x ++;
#endif
            }
        }
        else
        {
            inbyte = (unsigned char)
                (ascii_cp437[inbyte] & MY_A_CHARTEXT);
            codepoint = inbyte;
        }
        if (codepoint == '&')
        {
            fprintf(snapshot,
                    "&amp;");
        }
        else if (codepoint == '<')
        {
            fprintf(snapshot,
                    "&lt;");
        }
        else if (codepoint == '>')
        {
            fprintf(snapshot,
                    "&gt;");
        }
        else if (codepoint == '\"')
        {
            fprintf(snapshot,
                    "&quot;");
        }
        else if (use_acs && use_raw && ! use_raw_ucs)
        {
            fprintf(snapshot,
                    "%c",
                    (char) inbyte);
        }
        else if ((codepoint >= 0x20) && (codepoint <= 0x7E))
        {
            fprintf(snapshot,
                    "%c",
                    (char) codepoint);
        }
        else
        {
            fprintf(snapshot,
                    "&#%lu;",
                    codepoint);
        }
        snapshot_x ++;
    }
}

/* add CP437 byte b with attributes attrs */
static int
my_addch(unsigned long b, chtype attrs)
{
    int ret;
    chtype c = '\?';
    int old_y, old_x;
    int new_y, new_x;

    getyx(stdscr, old_y, old_x);
    my_attrset(attrs);
    snapshot_addch(b);
    if (use_acs && use_raw && ! use_raw_ucs)
    {
        char buf[2];
        buf[0] = (char) (unsigned char) (b & 0xFF);
        buf[1] = '\0';
        ret = addstr(buf);
        getyx(stdscr, new_y, new_x);
        if ((old_x != new_x) || (old_y != new_y))
            return ret;
    }
    if (b <= 0xFF)
    {
        if (use_acs)
        {
#if USE_WIDEC_SUPPORT
            if (use_raw && use_raw_ucs)
            {
                wchar_t my_wch;
                int my_wcw;

                my_wch = uni_cp437[b];
                my_wcw = wcwidth(my_wch);
                if (my_wcw && (my_wcw <= ((CJK_MODE) ? 2 : 1)))
                {
                    ret = addnwstr(&my_wch, 1);
                    getyx(stdscr, new_y, new_x);
                    if ((old_x != new_x) || (old_y != new_y))
                        return ret;
                    /* U+30FB KATAKANA MIDDLE DOT -> 0xFF0E FULLWIDTH FULL STOP */
                    if (my_wch == 0x30fb)
                    {
                        my_wch = 0xff0e;
                        ret = addnwstr(&my_wch, 1);
                        getyx(stdscr, new_y, new_x);
                        if ((old_x != new_x) || (old_y != new_y))
                            return ret;
                    }
                    /* U+301C WAVE DASH -> 0xFF5E FULLWIDTH TILDE */
                    if (my_wch == 0x301c)
                    {
                        my_wch = 0xff5e;
                        ret = addnwstr(&my_wch, 1);
                        getyx(stdscr, new_y, new_x);
                        if ((old_x != new_x) || (old_y != new_y))
                            return ret;
                    }
                }
            }
            if (altcharset_cp437[b])
            {
                wchar_t my_wchbuf[CCHARW_MAX];
                attr_t my_acs_attrs;
                short my_color_pair;
                attr_t my_current_attrs;
                int my_len;

                my_len =
                    getcchar(
                        altcharset_cp437[b],
                        NULL,
                        &my_acs_attrs,
                        &my_color_pair,
                        NULL);
                if (my_len &&
                    (getcchar(
                        altcharset_cp437[b],
                        my_wchbuf,
                        &my_acs_attrs,
                        &my_color_pair,
                        NULL) != ERR) &&
                    (my_wcswidth(my_wchbuf, my_len) == 1))
                {
                    attr_get(
                        & my_current_attrs,
                        & my_color_pair,
                        NULL);
                    attr_set(
                        my_current_attrs | my_acs_attrs,
                        my_color_pair,
                        NULL);
                    ret = addnwstr(
                        my_wchbuf,
                        my_len);
                    attr_set(
                        my_current_attrs,
                        my_color_pair,
                        NULL);
                    getyx(stdscr, new_y, new_x);
                    if ((old_x != new_x) || (old_y != new_y))
                        return ret;
                }
            }
#else /* ! USE_WIDEC_SUPPORT */
            if (use_raw && use_raw_ucs)
            {
                c = uni_cp437[b];
                ret = addch(c);
                getyx(stdscr, new_y, new_x);
                if ((old_x != new_x) || (old_y != new_y))
                    return ret;
            }
            c = altcharset_cp437[b];
#ifdef SLCURSES
#ifdef A_CHARTEXT
            my_attrset(attrs | (c & ~A_CHARTEXT));
#endif
#endif
            ret = addch(c);
#ifdef SLCURSES
#ifdef A_CHARTEXT
            my_attrset(attrs);
#endif
#endif
            getyx(stdscr, new_y, new_x);
            if ((old_x != new_x) || (old_y != new_y))
                return ret;
#endif /* ! USE_WIDEC_SUPPORT */
        }
    }
    c = ascii_cp437[b];
    return addch(c);
}

/* add CP437 string s with attributes attrs */
static int
my_addstr(const char *s, chtype attrs)
{
    size_t i;
    int ret;
    int y, x;

    getyx(stdscr, y, x);
    for (i = 0; s[i]; i ++)
    {
        unsigned long b;

        b = (unsigned long) (unsigned char) s[i];
        move(y, x + i * (CJK_MODE ? 2 : 1));
        ret = my_addch(b, attrs);
        if (ret == ERR)
        {
            break;
        }
    }
    return ret;
}

static void
escape(const char *s, int len)
{
    int c;
    int i;

    for (i = 0; i < len; i ++)
        if (isprint((c = (unsigned char) s[i]))) {
            if ((c == '"') || (c == '\\') || (c == '\'') || (c == '\?'))
                putchar('\\');
            putchar(c);
        }
        else
        {
            if (((i + 1 == len) || ! isdigit((unsigned char) s[i + 1])) && (c == '\0'))
                printf("\\0");
            else
                printf("\\%3.3o", c);
        }
}

static void
writefont(const char *file,
          const char *prefix,
          int w, int h,
          const char *font,
          int *used
#if USE_COLOR
          , int *color
#endif
    )
{
    int c, i, j;

    printf("static char builtin_%sfile_str[] = \"", prefix);
    escape(file, strlen(file));
    printf("\";\n");
    printf("char *builtin_%sfile = builtin_%sfile_str;\n", prefix, prefix);
    printf("char %s[256 * %d * %d] = {", prefix, h, w);
    for (c = 0; c < 256; c ++) {
        printf("\n/* 0x%2.2X */", c);
        for (i = 0; i < h; i ++) {
            printf("\n ");
            for (j = 0; j < w; j ++) {
                printf(" \'");
                escape(font + (c * h + i) * w + j, 1);
                printf("\'");
                if (((c + 1) < 256) || ((i + 1) < h) || ((j + 1) < w))
                    printf(",");
            }
        }
    }
    printf("};\n");
    printf("int %s_used[256] = {\n", prefix);
    for (c = 0; c < 256; c ++) {
        if (c && ! (c & 3))
            printf(",\n");
        else if (c)
            printf(", ");
        printf("/* 0x%2.2X */ %d", c, used[c]);
    }
    printf("};\n");
#if USE_COLOR
    printf("int %s_color[256] = {\n", prefix);
    for (c = 0; c < 256; c ++) {
        if (c && ! (c & 3))
            printf(",\n");
        else if (c)
            printf(", ");
        printf("/* 0x%2.2X */ 0x%X", c, color[c]);
    }
    printf("};\n");
#endif
}

#ifdef XCURSES
#define XCURSES_USAGE \
" [ -- toolkit-options]"
#else
#define XCURSES_USAGE
#endif

#define SUMMARY(progname) \
"Usage: %s [-h] [options]" \
XCURSES_USAGE \
"\n", progname

int
main(int argc, char *argv[], char *envp[])
{
    FILE *infile;
    int n, i, j, k, kk = ERR;
    long c;
    int dir;
    int debug = 0;
    int nogame = 0;
    int sound = SOUND;
    int
        dump_maze = 0,
        dump_stile = 0,
        dump_tile = 0;
    int use_idlok = 1;
    char *tilefile = TILEFILE;
    char *stilefile = STILEFILE;
    char *mazefile = MAZEFILE;
    int
        level = 0,
        maze_level = 0,
        cycles = 0,
        score = 0,
        dots = 0,
        points = 0,
        lives = 3,
        earned = 0,
        dying = 0,
        dead = 0, deadpan = 0;
    int
        oldplayer = 0,
        player = 1;
    unsigned long winning = 1;
    unsigned long delay = DELAY;
    unsigned long uli;
    unsigned long
        pellet_timer = 0,
        pellet_time = 0;
    int pause_timer = 0;
    int munched = HERO;
    int cjk_mode = 0;
    int colorize = 0;
    int colorize_p = 0;
    int underline = 0;
    int swapdots = 0;;
    int option_index;
    char *defvariant = VARIANT;
    char *defsize = SIZE;
    static struct option long_options[] =
        {
            {"version", 0, 0, 'V'},
            {"help", 0, 0, 'h'},
            {"variant", 1, 0, 'v'},
            {"size", 1, 0, 'z'},
            {0, 0, 0, 0}
        };
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if (! setlocale(LC_CTYPE, ""))
    {
        fprintf(
            stderr,
            "warning: setlocale(LC_CTYPE, \"\") failed\n");
        fflush(stderr);
    }
    swapdots = SWAPDOTS;
#ifdef BUILTIN_MAZE
    for (n = 0; n < MAZE_N; n ++)
    {
        for (i = 0; i < MAZE_H; i ++)
        {
            memcpy((void *) maze[n][i],
                   (void *) maze_data[n][i],
                   MAZE_W + 1);
        }
    }
#endif
    for (i = 0; i < SPRITES; i ++) {
        sprite_used[i] = 0;
        sprite_frame[i] = '\0';
#if USE_COLOR
        sprite_color[i] = 0x7;
#endif
    }
#if USE_COLOR
    for (i = 0; i < 256; i ++) {
#ifndef BUILTIN_TILE
        tile_color[i] = 0x7;
#endif
#ifndef BUILTIN_STILE
        stile_color[i] = 0x7;
#endif
    }
#endif
    while ((i = getopt_long(argc, argv, "Vv:z:bcd:hm:noprqs:t:uUMRSTf:F:aAeEBNiI12",
                            long_options, &option_index))
           != -1)
        switch (i) {
        case 'V':
            printf("%s-%s (%s) %s\n",
                   VARIANT,
                   SIZE,
                   MYMAN,
                   VERSION);
            fflush(stdout), fflush(stderr), exit(0);
        case 'v':
            defvariant = optarg;
            break;
        case 'z':
            defsize = optarg;
            break;
        case 'b':
            sound = 1;
            break;
        case 'q':
            sound = 0;
            break;
	case 'i':
            use_idlok = 0;
            break;
	case 'I':
            use_idlok = 1;
            break;
        case 'r':
            use_raw = 1;
            break;
        case 'R':
            use_raw = 0;
            break;
        case 'e':
            use_raw_ucs = 1;
            break;
        case 'E':
            use_raw_ucs = 0;
            break;
        case 'a':
            use_acs = 0;
            break;
        case 'A':
            use_acs = 1;
            break;
        case 'd':
        {
            char garbage;

            if (sscanf(optarg, "%lu%c", &uli, &garbage) != 1) {
                fprintf(stderr,
                        "%s: argument to -d must be an unsigned long integer.\n",
                        *argv);
                fflush(stderr), exit(1);
            }
            delay = uli;
            break;
        }
        case 'h':
            printf("Usage: %s [options]"
                   XCURSES_USAGE
                   "\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s"
                   "\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
                   *argv,
                   "-h \tdisplay this help and exit",
                   "-b \tenable sounds",
                   "-q \tdisable sounds",
#if USE_COLOR
                   "-c \tenable color support",
#else
                   "-c \tenable color support (must recompile first)",
#endif
                   "-n \tdisable color support",
                   "-B \tuse dim and bright attributes for missing colors",
                   "-N \tdon't use dim and bold attributes for extra colors",
                   "-o \tuse 'o' for power pellets and bullet for dots",
                   "-p \tuse bullet for power pellets and '.' for dots",
#if DELAY
                   "-d NUM \tdelay NUM microseconds/refresh",
#else
                   "-d NUM \tdelay NUM microseconds/refresh (must recompile first)",
#endif
#if USE_ATTR
                   "-u \tuse the underline attribute for maze walls",
#else
                   "-u \tuse the underline attribute for maze walls (must recompile first)",
#endif
                   "-U \tdon't use the underline attribute for maze walls",
                   "-r \tuse raw tile characters (CP437 or UCS/Unicode character graphics)",
                   "-R \tuse altcharset translations (VT100-style graphics)",
                   "-e \tuse UCS/Unicode for internal representation of raw tile characters",
                   "-E \tuse CP437 for internal representation of raw tile characters",
                   "-a \tuse ASCII for altcharset translation",
                   "-A \tuse your terminal's altcharset translations",
                   "-i \tscroll vertically by redrawing the screen",
                   "-I \tuse your terminal's vertical scrolling capabilities",
                   "-1 \tuse default UCS/Unicode mapping",
                   "-2 \tuse CJK (fullwidth) UCS/Unicode mapping",
                   "-m FILE \tuse the maze in FILE",
                   "-s FILE \tuse sprites from FILE",
                   "-t FILE \tuse tiles from FILE",
                   "-M \twrite the maze to stdout in C format and exit",
                   "-S \twrite sprites to stdout in C format and exit",
                   "-T \twrite tiles to stdout in C format and exit",
                   "-f FILE \tredirect stdout to FILE (append)",
                   "-F FILE \tredirect stdout to FILE (truncate)");
            printf("Defaults:%s%s%s%s%s%s%s%s%s%s -d %lu -m \"",
                   use_raw ? " -r" : " -R",
                   use_raw_ucs ? " -e" : " -E",
                   use_acs ? " -A" : " -a",
                   use_idlok ? " -I" : " -i",
                   sound ? " -b" : " -q",
                   colorize ? " -c" : " -n",
                   use_dim_and_bright ? " -B" : " -N",
                   swapdots ? " -o" : " -p",
                   underline ? " -u" : " -U",
                   cjk_mode ? " -2" : " -1",
                   delay ? delay : 0);
            if (mazefile)
                escape(mazefile, strlen(mazefile));
            else {
                printf("(");
                escape(builtin_mazefile, strlen(builtin_mazefile));
                printf(")");
            }
            printf("\" -s \"");
            if (stilefile)
                escape(stilefile, strlen(stilefile));
            else {
                printf("(");
                escape(builtin_stilefile, strlen(builtin_stilefile));
                printf(")");
            }
            printf("\" -t \"");
            if (tilefile)
                escape(tilefile, strlen(tilefile));
            else {
                printf("(");
                escape(builtin_tilefile, strlen(builtin_tilefile));
                printf(")");
            }
            printf("\"\n");
            fflush(stdout), fflush(stderr), exit(0);
            break;
        case 'u':
            underline = 1;
            break;
        case 'U':
            underline = 0;
            break;
        case 'M':
            dump_maze = 1;
            nogame = 1;
            break;
        case 'S':
            dump_stile = 1;
            nogame = 1;
            break;
        case 'T':
            dump_tile = 1;
            nogame = 1;
            break;
        case 'm':
#ifdef BUILTIN_MAZE
            if ((*optarg == '(') &&
                (strlen(optarg) == (strlen(builtin_mazefile) + 2)) &&
                (optarg[strlen(optarg) - 1] == ')') &&
                ! strncmp(optarg + 1, builtin_mazefile, strlen(builtin_mazefile))) {
                mazefile = 0;
                break;
            }
#endif
            mazefile = optarg;
            break;
        case 'n':
            colorize_p = 1;
            colorize = 0;
            break;
        case 'o':
            swapdots = 1;
            break;
        case 'p':
            swapdots = 0;
            break;
        case '2':
            cjk_mode = 1;
            break;
        case '1':
            cjk_mode = 0;
            break;
        case 'c':
            colorize_p = 1;
            colorize = 1;
            break;
        case 'B':
            use_dim_and_bright = 1;
            break;
        case 'N':
            use_dim_and_bright = 0;
            break;
        case 't':
#ifdef BUILTIN_TILE
            if ((*optarg == '(') &&
                (strlen(optarg) == (strlen(builtin_tilefile) + 2)) &&
                (optarg[strlen(optarg) - 1] == ')') &&
                ! strncmp(optarg + 1, builtin_tilefile, strlen(builtin_tilefile))) {
                tilefile = 0;
                break;
            }
#endif
            tilefile = optarg;
            break;
        case 's':
#ifdef BUILTIN_STILE
            if ((*optarg == '(') &&
                (strlen(optarg) == (strlen(builtin_stilefile) + 2)) &&
                (optarg[strlen(optarg) - 1] == ')') &&
                ! strncmp(optarg + 1, builtin_stilefile, strlen(builtin_stilefile))) {
                stilefile = 0;
                break;
            }
#endif
            stilefile = optarg;
            break;
        case 'f':
            if (freopen(optarg, "a", stdout) == NULL)
            {
                perror(optarg);
                fflush(stderr), exit(1);
            }
            break;
        case 'F':
            if (freopen(optarg, "w", stdout) == NULL)
            {
                perror(optarg);
                fflush(stderr), exit(1);
            }
            break;
        default:
            fprintf(stderr, SUMMARY(argv[0]));
            fflush(stderr), exit(2);
        }
#ifdef XCURSES
    argv[optind - 1] = argv[0];
    argc -= optind;
    argc ++;
    argv += optind;
    argv --;
#else
    if (optind < argc)
    {
        fprintf(stderr, SUMMARY(argv[0]));
        fflush(stderr), exit(2);
    }
#endif
    if (strcmp(defvariant, VARIANT))
    {
        fprintf(stderr,
                "%s: game variant `%s' not included in this binary\n",
                *argv,
                defvariant);
        exit(2);
    }
    if (strcmp(defsize, SIZE))
    {
        fprintf(stderr,
                "%s: game size `%s' not included in this binary\n",
                *argv,
                defsize);
        exit(2);
    }

#define FIFTH     (TILE_W * (SQUARE ? 2 : 1))

#if ! DELAY
    if (delay)
    {
        fprintf(stderr,
                "%s: compile with -DDELAY=1 to enable the -d option.\n",
                *argv);
        fflush(stderr), exit(1);
    }
#else
    delay = delay / FIFTH;
#endif

#define TWOSECS    (10 * FIFTH)
#define ONESEC     (TWOSECS / 2)
#define FRUITLIFE  (TWOSECS * 15 / 4)
#define DEATHSHIFT 3
#define DEATHDELAY (1 << (DEATHSHIFT + 2))
#define MEMDELAY   (3 * TWOSECS)

    pellet_time = 7 * ONESEC;
#if ! USE_ATTR
    if (underline)
    {
        fprintf(stderr,
                "%s: compile with -DUSE_ATTR=1 to enable the -u option.\n",
                *argv);
        fflush(stderr), exit(1);
    }
#endif
#if ! USE_COLOR
    if (colorize)
    {
        fprintf(stderr,
                "%s: compile with -DUSE_COLOR=1 to enable color support.\n",
                *argv);
        fflush(stderr), exit(1);
    }
#endif
    if ((tilefile && readfont(tilefile, TILE_W, TILE_H, tile, tile_used
#if USE_COLOR
                              , tile_color
#endif
             )) ||
        (stilefile && readfont(stilefile, STILE_W, STILE_H, stile, stile_used
#if USE_COLOR
                               , stile_color
#endif
            )))
        return 1;
    if (mazefile) {
        if (! (infile = fopen(mazefile, "rb"))) {
            perror(mazefile);
            return 1;
        }
        {
            int rn, rw, rh;
            char X;

            if ((fscanf(infile, "%d%d%c%d", &rn, &rw, &X, &rh) != 4) ||
                (tolower(X) != 'x') || (fgetc(infile) != '\n'))  {
                fprintf(stderr, "%s: can't find a dimension specification N WxH\n",
                        mazefile);
                return 1;
            }
            if ((rw != MAZE_W) || (rh != MAZE_H) || (rn != MAZE_N)) {
                fprintf(stderr, "%s: dimension specification %d %dx%d doesn't match %d %dx%d\n",
                        mazefile, rn, rw, rh, MAZE_N, MAZE_W, MAZE_H);
                return 1;
            }
        }
        for (n = 0; n < MAZE_N; n ++)
        {
            for (i = 0; i < MAZE_H; i ++)
            {
                for (j = 0; j < MAZE_W; j ++)
                    if ((c = fgetc(infile)) == EOF)
                    {
                        if (feof(infile))
                            fprintf(stderr, "%s: premature EOF\n", mazefile);
                        else
                            perror(mazefile);
                        return 1;
                    } else if ((c == '\r') || (c == '\n') || (c == '\f') || (c == '\v'))
                        j --;
                    else
                        maze[n][i][j] = (char) (unsigned char) c;
                maze[n][i][MAZE_W] = (char) (unsigned char) c;
            }
        }
        fclose(infile);
    }
    for (i = 0; i < MAZE_H; i ++)
    {
        memcpy((void *)blank_maze,
               (void *)maze,
               (MAZE_W + 1) * MAZE_H * MAZE_N * sizeof(unsigned char));
    }
    sprite[FRUIT] = STILE_FRUIT;
    sprite[FRUIT_SCORE] = STILE_FRUIT_SCORE;
    sprite_frame[FRUIT] =
        sprite_frame[FRUIT_SCORE] = BONUS(level);
    sprite_x[FRUIT] =
        sprite_x[FRUIT_SCORE] = (int) XFRUIT;
    sprite_y[FRUIT] =
        sprite_y[FRUIT_SCORE] = (int) YFRUIT;

    sprite[GHOST_SCORE] = STILE_200;
    sprite_frame[GHOST_SCORE] = 0;

    sprite[HERO] = STILE_HERO + 4;
    dir = LEFT;
    sprite_frame[HERO] = 0;
    sprite_x[HERO] = (int) XHERO;
    sprite_y[HERO] = (int) YHERO;
    sprite_used[HERO] = 0;

#if USE_COLOR
    sprite_color[HERO] = 0xE;
#endif

    for (i = 0; i < GHOSTS; i++) {
        int mean, blue;

        mean = MEAN(i);
        blue = BLUE(i);
        sprite[i] = STILE_EYES;
        sprite[mean] = STILE_MEAN;
        sprite[blue] = STILE_BLUE;
        sprite_used[i] =
            sprite_used[mean] =
            sprite_used[blue] = 0;
        sprite_frame[i] =
            sprite_frame[mean] =
            sprite_frame[blue] = 0;
        ghost_mem[i] = 0;
        ghost_timer[i] = TWOSECS;
        ghost_man[i] = 0;
#if USE_COLOR
        sprite_color[i] = 0xF;
        sprite_color[blue] = 0x9;
        sprite_color[mean] = 0xA;
#endif
    }

#if USE_COLOR
    sprite_color[MEAN(INKY)] = 0xB;
    sprite_color[MEAN(BLINKY)] = 0xC;
    sprite_color[MEAN(PINKY)] = 0xD;
    sprite_color[MEAN(CLYDE)] = 0x6;
#endif

    if (dump_maze) {
        printf("static char builtin_mazefile_str[] = \"");
        escape(mazefile ? mazefile : builtin_mazefile,
               strlen(mazefile ? mazefile : builtin_mazefile));
        printf("\";\n");
        printf("char *builtin_mazefile = builtin_mazefile_str;\n");
        printf("const char *maze_data[%d][%d] = {\n", MAZE_N, MAZE_H);
        for (n = 0; n < MAZE_N; n ++)
        {
            if (n > 0)
            {
                printf(",");
            }
            printf("{\n");
            for (i = 0; i < MAZE_H; i ++) {
                printf("  \"");
                escape(maze[n][i], MAZE_W + 1);
                printf("\"%s\n",
                       ((i + 1) < MAZE_H) ? "," : "}");
            }
        }
        printf("};\n");
    }
    if (dump_stile)
        writefont(stilefile ? stilefile : builtin_stilefile,
                  "stile", STILE_W, STILE_H, stile, stile_used
#if USE_COLOR
                  , stile_color
#endif
            );
    if (dump_tile)
        writefont(tilefile ? tilefile : builtin_tilefile,
                  "tile", TILE_W, TILE_H, tile, tile_used
#if USE_COLOR
                  , tile_color
#endif
            );
    if (nogame)
        fflush(stdout), fflush(stderr), exit(0);

#define MY_COLS (COLS / (cjk_mode ? 2 : 1))
    if (cjk_mode)
    {
        memcpy((void *)uni_cp437,
               (void *)uni_cp437_fullwidth,
               sizeof(uni_cp437));
    }

#ifdef SLANG_VERSION
#if SLANG_VERSION >= 20000
    SLutf8_enable(-1);
    SLtt_utf8_enable(1);
    SLsmg_utf8_enable(1);
    SLinterp_utf8_enable(1);
#endif
#endif
#ifdef XCURSES
    Xinitscr(argc, argv);
#else
    initscr();
#endif
    clear();
    cbreak();
    noecho();
    nonl();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);
    my_attrset(0);
#if USE_ATTR
    curs_set(0);
#endif
#if USE_KEYPAD
    keypad(stdscr, TRUE);
#endif
#ifndef DISABLE_IDLOK
    if (use_idlok)
    {
	idlok(stdscr, TRUE);
    }
    else
    {
	idlok(stdscr, FALSE);
    }
#endif
    leaveok(stdscr, TRUE);
    init_trans(swapdots);
#if USE_COLOR
#if COLORIZE
    if (! colorize_p) {
        colorize = has_colors();
        colorize_p = 1;
    }
#endif
    start_color();
    if (colorize)
        init_pen();
#endif
    while ((lives || dead || dying || pause_timer) &&
           ((k = getch()) != 'q') && (k != 'Q'))
    {
        int s, x1, y1, line, col, xtile, ytile, xoff, yoff;
        int roff, coff;

#if DELAY
        if (delay)
        {
            struct timeval tv2;
            unsigned long actual_delay = delay;

            if (gettimeofday(&tv2, 0))
            {
                tv2.tv_sec = 0;
                tv2.tv_usec = 0;
            }
            if ((tv2.tv_sec || tv2.tv_usec)
                &&
                (tv.tv_usec || tv.tv_sec))
            {
                if ((tv2.tv_sec > tv.tv_sec)
                    ||
                    ((tv2.tv_sec == tv.tv_sec)
                     &&
                     (tv2.tv_usec > tv.tv_usec)))
                {
                    unsigned long delta;

                    delta = (unsigned long)
                        ((tv2.tv_sec * 1e6 + tv2.tv_usec)
                         -
                         (tv.tv_sec * 1e6 + tv.tv_usec));
                    if (delta <= actual_delay)
                    {
                        actual_delay -= delta;
                    }
                    else
                    {
                        actual_delay = 0;
                    }
                }
            }
            if (actual_delay)
                usleep(actual_delay);
            if (gettimeofday(&tv, 0))
            {
                tv.tv_usec = 0;
                tv.tv_sec = 0;
            }
        }
#endif
        if ((k == 'r') || (k == 'R')) {
            clear();
            clearok(curscr, TRUE);
            wrefresh(stdscr);
            continue;
        }
	if ((k == 'i') || (k == 'I'))
	{
            use_idlok = ! use_idlok;
#ifndef DISABLE_IDLOK
	    if (use_idlok)
	    {
		idlok(stdscr, TRUE);
	    }
	    else
	    {
		idlok(stdscr, FALSE);
	    }
#endif
	}
#if USE_COLOR
        if ((k == 'c') || (k == 'C')) {
            if ((colorize = ! colorize))
                init_pen();
            else
                destroy_pen();
            my_attrset(0);
            clear();
            clearok(curscr, TRUE);
            continue;
        }
#endif
        if ((k == 'b') || (k == 'B')) {
            use_dim_and_bright =
                ! use_dim_and_bright;
#if USE_COLOR
            if (colorize)
            {
                destroy_pen();
                init_pen();
            }
#endif
            my_attrset(0);
            clear();
            clearok(curscr, TRUE);
            continue;
        }
#if USE_ATTR
        if ((k == 'u') || (k == 'U')) {
            underline = ! underline;
            clear();
            clearok(curscr, TRUE);
            continue;
        }
#endif
        if ((k == 's') || (k == 'S')) {
            sound = ! sound;
            continue;
        }
        if ((k == 'o') || (k == 'O') || (k == '0'))
        {
            swapdots = ! swapdots;
            init_trans(swapdots);
            clear();
            clearok(curscr, TRUE);
            continue;
        }
        if ((k == 'a') || (k == 'A'))
        {
            use_acs = ! use_acs;
            clear();
            clearok(curscr, TRUE);
            continue;
        }
        if ((k == 'x') || (k == 'X'))
        {
            use_raw = ! use_raw;
            clear();
            clearok(curscr, TRUE);
            continue;
        }
        if ((k == 'e') || (k == 'E'))
        {
            use_raw_ucs = ! use_raw_ucs;
            clear();
            clearok(curscr, TRUE);
            continue;
        }
        if ((k == 't') || (k == 'T'))
        {
            char buf[128];
            unsigned int idx;

            /* try to find a free slot */
            for (idx = 0; idx <= 9999; idx ++)
            {
                sprintf(buf,
                        "snap%4.4u%s",
                        idx, HTM_SUFFIX);
                if (access(buf, F_OK))
                {
                    break;
                }
            }
            snapshot = fopen(buf, "wb");
            snapshot_colorize = colorize;
            continue;
        }
        if ((k == 'p') || (k == 'P') || (k == 27)) {
            standout();
            mvprintw(LINES / 2, ((COLS - strlen(PAUSE)) & ~(cjk_mode ? 1 : 0)) / 2, PAUSE);
            standend();
            wrefresh(stdscr);
            while (getch() == ERR) {
#if DELAY
                if (delay)
                    usleep(delay);
#endif
            }
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            continue;
        }
        if (k == ERR)
            k = kk;
        else
            kk = k;
        xoff = sprite_x[HERO] % TILE_W;
        yoff = sprite_y[HERO] % TILE_H;
        xtile = XTILE(sprite_x[HERO]);
        ytile = YTILE(sprite_y[HERO]);
        if (pause_timer && ! -- pause_timer) {
            sprite_used[GHOST_SCORE] = 0;
            sprite_frame[GHOST_SCORE] ++;
            if (sprite_used[HERO]) {
                memcpy(maze[maze_level][RMSG] + CMSG, msgbuf, MSGLEN);
                if (sound && lives)
                    beep();
            }
            else
            {
                sprite_used[HERO] = 1;
                sprite_used[munched] = 1;
            }
        }
        if (winning || dying) {
            int reset = 0;
            static int init = 0;

            if (dying && ! -- dying) {
                reset = 1;
                -- lives;
            } else if ((! pause_timer) && winning && ! -- winning) {
                memcpy((void *)maze,
                       (void *)blank_maze,
                       (MAZE_W + 1) * MAZE_H * MAZE_N * sizeof(unsigned char));
                dots = 0;
                pellet_timer = 0;
                reset = 1;
                ++ level;
                maze_level = level % MAZE_N;
                sprite_frame[FRUIT] =
                    sprite_frame[FRUIT_SCORE] = BONUS(level);
                if (pellet_time > ONESEC)
                    pellet_time -= ONESEC;
                else
                    pellet_time = 0;
                if (delay > SPEEDUP)
                    delay -= SPEEDUP;
            }
            if ((! init) && (winning || reset)) {
                init = 1;
                sprite_used[FRUIT] =
                    sprite_used[FRUIT_SCORE] =
                    sprite_used[GHOST_SCORE] = 0;
                for (i = 0; i < GHOSTS; i++) {
                    int mean, blue;

                    mean = MEAN(i);
                    blue = BLUE(i);
                    sprite_used[i] =
                        sprite_used[mean] =
                        sprite_used[blue] = 0;
                }
                sprite_frame[HERO] = 0;
            }
            if (reset) {
                memset((void *)home_dir, 0, sizeof(home_dir));
                for (i = 0; i < GHOSTS; i++) {
                    int mean, blue;

                    mean = MEAN(i);
                    blue = BLUE(i);
                    sprite_used[i] =
                        sprite_used[mean] =
                        sprite_used[blue] = 0;
                    sprite_frame[i] = (ghost_dir[i] = i + 1) - 1;
                    ghost_mem[i] = 0;
                    ghost_timer[i] = TWOSECS;
                    ghost_man[i] = 0;
                    sprite_x[mean] =
                        (sprite_x[i] = (int) (XGHOST + COGHOST * TILE_W *
                                              ((i == CLYDE) - (i == INKY))));
                    sprite_y[mean] =
                        (sprite_y[i] = (int) (YGHOST - ROGHOST * TILE_H *
                                              (i == BLINKY)));
                }
                kk = ERR;
                init = 0;
                cycles = 0;
                sprite_used[HERO] = 0;
                sprite_frame[HERO] = 0;
                sprite_x[HERO] = (int) XHERO;
                sprite_y[HERO] = (int) YHERO;
                dead =
                    deadpan = (int) (YHERO - YGHOST);
                if (dead < 2)
                    dead = 2;
                memcpy(msgbuf, maze[maze_level][RMSG] + CMSG, MSGLEN);
                memcpy(maze[maze_level][RMSG] + CMSG, lives ? READY : GAMEOVER, MSGLEN);
                memcpy(msgbuf2, maze[maze_level][RMSG2] + CMSG2, MSGLEN);
                if (player != oldplayer) {
                    oldplayer = player;
                    pause_timer = ONESEC;
                    munched = HERO;
                    memcpy(maze[maze_level][RMSG2] + CMSG2, PLAYER(player), MSGLEN);
                }
            }
            if (dying)
                sprite_frame[HERO] = ((DEATHDELAY - dying) >> DEATHSHIFT) % 4;
        } else if (dead && ! pause_timer) {
            if (deadpan)
                deadpan --;
            if (! -- dead) {
                for (i = 0; i < GHOSTS; i ++) {
                    int mean, blue;

                    mean = MEAN(i);
                    blue = BLUE(i);
                    sprite_used[i] = lives ? VISIBLE_EYES : 0;
                    sprite_used[mean] = lives ? 1 : 0;
                }
                sprite_used[HERO] = 1;
                pause_timer = ONESEC;
                munched = HERO;
                memcpy(maze[maze_level][RMSG2] + CMSG2, msgbuf2, MSGLEN);
                sprite[HERO] = STILE_HERO + 4;
                dir = LEFT;
#if USE_COLOR
                if (! colorize)
#endif
                    if (underline)
                    {
                        clear();
                        clearok(curscr, TRUE);
                    }
            }
        }
        else
        {
            if (((score >= 10000) && ! earned) ||
                ((score >= 50000) && (earned == 1))) {
                earned ++;
                lives ++;
            }
            if (ISPELLET((c = (unsigned char) maze[maze_level][ytile][xtile]))
                || ISDOT(c)) {
                maze[maze_level][ytile][xtile] = ' ';
                sprite_frame[HERO] = 0;
                score += 10 + 40 * ISPELLET(c);
                if (ISPELLET(c)) {
                    points = 200;
                    sprite_frame[GHOST_SCORE] = 0;
                    pellet_timer = pellet_time + 1;
                    for (s = 0; s < GHOSTS; s++) {
                        int mean, blue;

                        blue = BLUE(s);
                        mean = MEAN(s);
                        sprite_frame[blue] = 0;
                        if (sprite_used[mean] || sprite_used[blue]) {
                            if (sprite_used[mean]) {
                                sprite_x[blue] = sprite_x[s];
                                sprite_y[blue] = sprite_y[s];
                            }
                            sprite_used[s] = 0;
                            sprite_used[mean] = 0;
                            sprite_used[blue] = 1;
                            sprite[blue] = STILE_BLUE;
                            ghost_dir[s] = DIRWRAP(ghost_dir[s] + 2);
                        }
                    }
                }
                if ((++ dots == DOTS_FRUIT1) || (dots == DOTS_FRUIT2)) {
                    sprite_used[FRUIT] = 1;
                    sprite_timer[FRUIT] = FRUITLIFE;
                    if (sound)
                        beep();
                } else if (dots == DOTS) {
                    munched = HERO;
                    winning = 2 * TWOSECS;
                    if (sound)
                        beep();
                }
            } else if (sprite_used[FRUIT] &&
                       collide(HERO, FRUIT)) {
                sprite_used[FRUIT] = 0;
                sprite_used[FRUIT_SCORE] = 1;
                sprite_timer[FRUIT_SCORE] = TWOSECS;
                score += bonus_score[sprite_frame[FRUIT]];
                sprite_frame[HERO] = 0;
                if (sound)
                    beep();
            }
            if ((k == 'w') || (k == 'W')) {
                dots = DOTS - 1;
                kk = ERR;
            } else if ((k == 'd') || (k == 'D')) {
                debug = ! debug;
                kk = ERR;
            } else if (((k == 'h') || (k == 'H') || (k == '4')
#if USE_KEYPAD
#ifdef KEY_LEFT
                        || (k == KEY_LEFT)
#endif
#endif
                           )
                       && ISOPEN((unsigned char) maze[maze_level][ytile][XWRAP(xtile - NOTRIGHT(xoff))])) {
                dir = LEFT;
                sprite[HERO] = STILE_HERO + 4;
            } else if (((k == 'l') || (k == 'L') || (k == '6')
#if USE_KEYPAD
#ifdef KEY_RIGHT
                        || (k == KEY_RIGHT)
#endif
#endif
                           )
                       && ISOPEN((unsigned char) maze[maze_level][ytile][XWRAP(xtile + NOTLEFT(xoff))])) {
                dir = RIGHT;
                sprite[HERO] = STILE_HERO + 12;
            } else if (((k == 'k') || (k == 'K') || (k == '8')
#if USE_KEYPAD
#ifdef KEY_UP
                        || (k == KEY_UP)
#endif
#endif
                           )
                       && ISOPEN((unsigned char) maze[maze_level][YWRAP(ytile - NOTBOTTOM(yoff))][xtile])) {
                dir = UP;
                sprite[HERO] = STILE_HERO;
            } else if (((k == 'j') || (k == 'J') || (k == '2')
#if USE_KEYPAD
#ifdef KEY_DOWN
                        || (k == KEY_DOWN)
#endif
#endif
                           )
                       && ISOPEN((unsigned char) maze[maze_level][YWRAP(ytile + NOTTOP(yoff))][xtile])) {
                dir = DOWN;
                sprite[HERO] = STILE_HERO + 16;
            }
            if (! pause_timer) {
                if (XDIR(dir))
                    sprite_y[HERO] = (1 + 2 * ytile) * TILE_H / 2;
                else if (dir)
                    sprite_x[HERO] = (1 + 2 * xtile) * TILE_W / 2;
                if (ISOPEN((unsigned char)
                           maze[maze_level][YWRAP(ytile + YLEAVING(dir, yoff + YDIR(dir)))]
                           [XWRAP(xtile + XLEAVING(dir, xoff + XDIR(dir)))])) {
                    sprite_x[HERO] = XPIXWRAP(sprite_x[HERO] + XDIR(dir) * (SQUARE ? (cycles & 1) : 1));
                    sprite_y[HERO] = YPIXWRAP(sprite_y[HERO] + YDIR(dir) * (cycles & 1));
                    sprite_frame[HERO] = (sprite_frame[HERO] + 1) % 4;
                }
            }
            for (s = 0; s < GHOSTS; s ++) {
                int blue, mean;

                blue = BLUE(s);
                mean = MEAN(s);
                if (! pause_timer)
                    sprite_frame[blue] =
                        (sprite_frame[mean] = ((cycles / FIFTH) & 2) ? 1 : 0);
                if (dying || winning) {
                    sprite_used[s] =
                        (sprite_used[blue] =
                         (sprite_used[mean] = 0));
                } else if (sprite_used[mean] && collide(mean, HERO) && ! pause_timer) {
                    if (sound)
                        beep();
                    dying = DEATHDELAY;
                    sprite[HERO] = STILE_HERO + 8;
                    sprite_frame[HERO] = 0;
                    pause_timer = ONESEC;
                    munched = HERO;
                    s = -1;
                    continue;
                } else if (sprite_used[blue] && collide(blue, HERO) && ! pause_timer) {
                    if (sound)
                        beep();
                    score += points;
                    points *= 2;
                    pause_timer = ONESEC;
                    sprite_used[HERO] = 0;
                    munched = s;
                    sprite_used[GHOST_SCORE] = 1;
                    sprite_x[GHOST_SCORE] = sprite_x[blue];
                    sprite_y[GHOST_SCORE] = sprite_y[blue];
                    sprite_x[s] = sprite_x[blue];
                    sprite_y[s] = sprite_y[blue];
                    sprite_used[blue] = 0;
                }
                else
                {
                    int dir0, dir1, dir2, o0, o1, o2, x, y, i1, j1, i2, j2;

                    dir1 = ghost_dir[s];
                    dir0 = DIRWRAP(dir1 + (s != PINKY) - (s == PINKY));
                    dir2 = DIRWRAP(dir1 - (s != PINKY) + (s == PINKY));
                    if (sprite_used[blue] && ! pause_timer)
                    {
                        /* runnin' scared */
                        if (cycles & 1) {
                            x = sprite_x[blue];
                            y = sprite_y[blue];
                            i1 = XTILE(x);
                            j1 = YTILE(y);
                            i2 = XTILE(sprite_x[HERO]);
                            j2 = YTILE(sprite_y[HERO]);
                            if ((i1 == i2) && (j1 <= j2)) {
                                for (j = j1; j <= j2; j ++)
                                    if (! ISOPEN((unsigned char) maze[maze_level][j][i1]))
                                        break;
                                if (j > j2) {
                                    ghost_mem[s] = dir;
                                    ghost_timer[s] = MEMDELAY;
                                }
                            } else if ((i1 == i2) && (j1 >= j2)) {
                                for (j = j2; j <= j1; j ++)
                                    if (! ISOPEN((unsigned char) maze[maze_level][j][i1]))
                                        break;
                                if (j > j1) {
                                    ghost_mem[s] = dir;
                                    ghost_timer[s] = MEMDELAY;
                                }
                            } else if ((j1 == j2) && (i1 <= i2)) {
                                for (i = i1; i <= i2; i ++)
                                    if (! ISOPEN((unsigned char) maze[maze_level][j1][i]))
                                        break;
                                if (i > i2) {
                                    ghost_mem[s] = dir;
                                    ghost_timer[s] = MEMDELAY;
                                }
                            } else if ((j1 == j2) && (i1 >= i2)) {
                                for (i = i2; i <= i1; i ++)
                                    if (! ISOPEN((unsigned char) maze[maze_level][j1][i]))
                                        break;
                                if (i > i1) {
                                    ghost_mem[s] = dir;
                                    ghost_timer[s] = MEMDELAY;
                                }
                            }
                            o0 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir0))]
                                        [XWRAP(i1 + XDIR(dir0))]);
                            o1 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir1))]
                                        [XWRAP(i1 + XDIR(dir1))]);
                            o2 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir2))]
                                        [XWRAP(i1 + XDIR(dir2))]);
                            if (((TILE_W / 2 == x % TILE_W) && XDIR(dir1))
                                || ((TILE_H / 2 == y % TILE_H) && YDIR(dir1))) {
                                if (o0 || o2) {
                                    if (! ghost_mem[s]) {
                                        if (o2)
                                            dir1 = dir2;
                                        else if (! o1)
                                            dir1 = dir0;
                                    }
                                    else
                                    {
                                        if (o0 && (dir0 != ghost_mem[s]))
                                            dir1 = dir0;
                                        else if (o2 && (dir2 != ghost_mem[s]))
                                            dir1 = dir2;
                                        else if (o1 && (dir1 != ghost_mem[s]))
                                            ;
                                        else if (o0)
                                            dir1 = dir0;
                                        else
                                            dir1 = dir2;
                                    }
                                } else if (! o1)
                                    dir1 = DIRWRAP(dir1 + 2);
                            }
                            if (XDIR(dir1))
                                y = (1 + 2 * j1) * TILE_H / 2;
                            else if (dir1)
                                x = (1 + 2 * i1) * TILE_W / 2;
                            if (SQUARE ? (cycles & 2) : 1)
                                sprite_x[blue] = XPIXWRAP(x + XDIR(dir1));
                            if (cycles & 2)
                                sprite_y[blue] = YPIXWRAP(y + YDIR(dir1));
                            if (! home_dir[s][YTILE(sprite_y[blue])][XTILE(sprite_x[blue])])
                                home_dir[s][YTILE(sprite_y[blue])][XTILE(sprite_x[blue])] =
                                    DIRWRAP(dir1 + 2);
                        }
                    }
                    else if (sprite_used[mean] && ! pause_timer)
                    {
                        /* out huntin' */
                        int d0, d1, d2;

                        x = sprite_x[mean];
                        y = sprite_y[mean];
                        i1 = XTILE(x);
                        j1 = YTILE(y);
                        i2 = XTILE(sprite_x[HERO]);
                        j2 = YTILE(sprite_y[HERO]);
                        if ((i1 == i2) && (j1 <= j2)) {
                            for (j = j1; j <= j2; j ++)
                                if (! ISOPEN((unsigned char) maze[maze_level][j][i1]))
                                    break;
                            if (j > j2) {
                                ghost_mem[s] = dir;
                                ghost_timer[s] = MEMDELAY;
                            }
                        } else if ((i1 == i2) && (j1 >= j2)) {
                            for (j = j2; j <= j1; j ++)
                                if (! ISOPEN((unsigned char) maze[maze_level][j][i1]))
                                    break;
                            if (j > j1) {
                                ghost_mem[s] = dir;
                                ghost_timer[s] = MEMDELAY;
                            }
                        } else if ((j1 == j2) && (i1 <= i2)) {
                            for (i = i1; i <= i2; i ++)
                                if (! ISOPEN((unsigned char) maze[maze_level][j1][i]))
                                    break;
                            if (i > i2) {
                                ghost_mem[s] = dir;
                                ghost_timer[s] = MEMDELAY;
                            }
                        } else if ((j1 == j2) && (i1 >= i2)) {
                            for (i = i2; i <= i1; i ++)
                                if (! ISOPEN((unsigned char) maze[maze_level][j1][i]))
                                    break;
                            if (i > i1) {
                                ghost_mem[s] = dir;
                                ghost_timer[s] = MEMDELAY;
                            }
                        }
                        o0 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir0))]
                                    [XWRAP(i1 + XDIR(dir0))]);
                        o1 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir1))]
                                    [XWRAP(i1 + XDIR(dir1))]);
                        o2 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir2))]
                                    [XWRAP(i1 + XDIR(dir2))]);
                        d0 = d2 = 0;
                        d1 = ISDOOR((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir1))]
                                    [XWRAP(i1 + XDIR(dir1))]);
                        if (! ISDOOR((unsigned char) maze[maze_level][j1][i1]))
                        {
                            d0 = ISDOOR((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir0))]
                                        [XWRAP(i1 + XDIR(dir0))]);
                            d2 = ISDOOR((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir2))]
                                        [XWRAP(i1 + XDIR(dir2))]);
                        }
                        d0 = d0 && (dir0 !=
                                    HOME_DIR(YWRAP(j1 + YDIR(dir0)),
                                             XWRAP(i1 + XDIR(dir0))))
                            && dir0 != DOWN
                            && dir0 != LEFT;
                        d2 = d2 && (dir2 !=
                                    HOME_DIR(YWRAP(j1 + YDIR(dir2)),
                                             XWRAP(i1 + XDIR(dir2))))
                            && dir2 != DOWN
                            && dir2 != LEFT;
                        d1 = d1 && (dir1 !=
                                    HOME_DIR(YWRAP(j1 + YDIR(dir1)),
                                             XWRAP(i1 + XDIR(dir1))))
                            && dir1 != DOWN
                            && dir1 != LEFT;
                        if (((TILE_W / 2 == x % TILE_W) && XDIR(dir1))
                            || ((TILE_H / 2 == y % TILE_H) && YDIR(dir1))) {
                            if (d2 || (o2 && (dir2 == ghost_mem[s])))
                                dir1 = dir2;
                            else if (d1 || (o1 && (dir1 == ghost_mem[s])))
                                ;
                            else if (d0 || (o0 && (dir0 == ghost_mem[s])))
                                dir1 = dir0;
                            else if (o1)
                                ;
                            else if (o2)
                                dir1 = dir2;
                            else if (o0)
                                dir1 = dir0;
                            else
                                dir1 = DIRWRAP(dir1 + 2);
                        }
                        if (XDIR(dir1))
                            y = (1 + 2 * j1) * TILE_H / 2;
                        else if (dir1)
                            x = (1 + 2 * i1) * TILE_W / 2;
                        if (SQUARE ? (cycles & 1) : 1)
                            sprite_x[mean] = (sprite_x[s] = XPIXWRAP(x + XDIR(dir1)));
                        if (cycles & 1)
                            sprite_y[mean] = (sprite_y[s] = YPIXWRAP(y + YDIR(dir1)));
                        if (! home_dir[s][YTILE(sprite_y[mean])][XTILE(sprite_x[mean])])
                            home_dir[s][YTILE(sprite_y[mean])][XTILE(sprite_x[mean])] =
                                DIRWRAP(dir1 + 2);
                    } else if (sprite_used[s] && ! pause_timer) {
                        int dx, dy;
                        unsigned char d, d1;

                        /* goin' home */
                        ghost_timer[s] = MEMDELAY;
                        x = sprite_x[s];
                        y = sprite_y[s];
                        i1 = XTILE(x);
                        j1 = YTILE(y);
                        dx = (int) ((XGHOST - x) / TILE_W);
                        dy = (int) (((dx ? YTOP : YGHOST) - y) / TILE_H);
                        if ((d = HOME_DIR(j1, i1)) &&
                            (! ((d1 = DIRWRAP(HOME_DIR(YWRAP(j1 + YDIR(d)), XWRAP(i1 + XDIR(d))) + 2)) &&
                                (d1 == d))))
                            ghost_mem[s] = d;
                        else {
                            if (dx * dx > dy * dy) {
                                if (dx > 0)
                                    ghost_mem[s] = RIGHT;
                                else
                                    ghost_mem[s] = LEFT;
                            }
                            else
                            {
                                if (dy < 0)
                                    ghost_mem[s] = UP;
                                else
                                    ghost_mem[s] = DOWN;
                            }
                        }
                        if (! (dx || dy)) {
                            sprite_used[s] = lives ? VISIBLE_EYES : 0;
                            sprite_used[mean] = 1;
                            sprite_x[mean] = x;
                            sprite_y[mean] = y;
                            sprite_frame[s] = (ghost_dir[s] = s + 1) - 1;
                            ghost_mem[s] = 0;
                            continue;
                        }
                        o0 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir0))][XWRAP(i1 + XDIR(dir0))])
                            || ISDOOR((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir0))][XWRAP(i1 + XDIR(dir0))]);
                        o1 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir1))][XWRAP(i1 + XDIR(dir1))])
                            || ISDOOR((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir1))][XWRAP(i1 + XDIR(dir1))]);
                        o2 = ISOPEN((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir2))][XWRAP(i1 + XDIR(dir2))])
                            || ISDOOR((unsigned char) maze[maze_level][YWRAP(j1 + YDIR(dir2))][XWRAP(i1 + XDIR(dir2))]);
                        if (o2 && (dir2 == ghost_mem[s]))
                            dir1 = dir2;
                        else if (o1 && (dir1 == ghost_mem[s]))
                            ;
                        else if (o0 && (dir0 == ghost_mem[s]))
                            dir1 = dir0;
                        else if (o1)
                            ;
                        else if (o2)
                            dir1 = dir2;
                        else if (o0)
                            dir1 = dir0;
                        else
                            dir1 = DIRWRAP(dir1 + 2);
                        sprite_x[mean] = (sprite_x[s] = XPIXWRAP(x + XDIR(dir1)));
                        sprite_x[s] = XPIXWRAP(x + XDIR(dir1));
                        sprite_y[s] = YPIXWRAP(y + YDIR(dir1));
                        if (! home_dir[s][YTILE(sprite_y[s])][XTILE(sprite_x[s])])
                            home_dir[s][YTILE(sprite_y[s])][XTILE(sprite_x[s])] =
                                DIRWRAP(dir1 + 2);
                    }
                    if ((! pause_timer) && ghost_timer[s] && ! -- ghost_timer[s]) {
                        ghost_timer[s] = MEMDELAY;
                        if (ghost_mem[s]) {
                            ghost_mem[s] = 0;
                        }
                        else
                            dir1 = DIRWRAP(ghost_dir[s] + 2);
                    }
                    ghost_dir[s] = (sprite_frame[s] = dir1 - 1) + 1;
                    if (sprite_used[mean] && collide(mean, HERO) && ! pause_timer) {
                        if (sound)
                            beep();
                        dying = DEATHDELAY;
                        sprite[HERO] = STILE_HERO + 8;
                        sprite_frame[HERO] = 0;
                        pause_timer = ONESEC;
                        munched = HERO;
                        s = -1;
                        continue;
                    } else if (sprite_used[blue] && collide(blue, HERO) && ! pause_timer) {
                        if (sound)
                            beep();
                        score += points;
                        points *= 2;
                        pause_timer = ONESEC;
                        sprite_used[HERO] = 0;
                        munched = s;
                        sprite_used[GHOST_SCORE] = 1;
                        sprite_x[GHOST_SCORE] = sprite_x[blue];
                        sprite_y[GHOST_SCORE] = sprite_y[blue];
                        sprite_x[s] = sprite_x[blue];
                        sprite_y[s] = sprite_y[blue];
                        sprite_used[blue] = 0;
                    }
                }
            }
            if (! pause_timer)
                for (s = 0; s < SPRITES; s ++) {
                    if (sprite_used[s] && sprite_timer[s])
                        if (! -- sprite_timer[s])
                            sprite_used[s] = 0;
                }
        }
        my_erase();
        x1 = sprite_x[HERO] - MY_COLS / 2;
        y1 = sprite_y[HERO] - LINES / 2 - deadpan;
        if (x1 + MY_COLS > MAZE_W * TILE_W)
            x1 = MAZE_W * TILE_W - MY_COLS;
        if (y1 + LINES - 1 > MAZE_H * TILE_H)
            y1 = MAZE_H * TILE_H - (LINES - 1);
        if (x1 < 0)
            x1 = 0;
        if (y1 < 0)
            y1 = 0;
        roff = 0;
        coff = 0;
        if (TILE_H * MAZE_H < LINES)
            roff = (LINES - 1 - TILE_H * MAZE_H) / 2;
        if (TILE_W * MAZE_W < MY_COLS)
            coff = (MY_COLS - TILE_W * MAZE_W) / 2;
        for (line = 0; (line < LINES) && (line < TILE_H * MAZE_H); line++) {
            for (col = 0; (col < MY_COLS) && (col < TILE_W * MAZE_W); col++) {
                int s;
                int a;

                my_move(line + roff, (coff + col) * (cjk_mode ? 2 : 1));
                a = 0;
                c = 0;
                xtile = XTILE((i = col + x1));
                ytile = YTILE((j = line + y1));
                for (s = 0; s < SPRITES; s ++) {
                    int t, x, y;

                    t = sprite[s] + sprite_frame[s];
                    if (debug
                        && sprite_used[s]
                        && ((x = sprite_x[s]) == i)
                        && ((y = sprite_y[s]) == j)) {
#if USE_COLOR
                        if (colorize) {
                            if (! (a = stile_color[t]))
                                a = sprite_color[s];
                            a = pen[a];
                        }
                        else
#endif
#if USE_ATTR
#ifdef MY_A_BOLD
                            if (s == HERO)
                                a |= use_dim_and_bright ? MY_A_BOLD : 0;
#endif
#endif
                        ;
                        if (s < GHOSTS)
                            c = (".^<v>")[ghost_mem[s]];
                        else
                            c = '.';
                        break;
                    } else if (sprite_used[s]
                               && stile_used[t]
                               && ((x = sprite_x[s] - STILE_W / 2) <= i)
                               && ((xoff = i - x) < STILE_W)
                               && ((y = sprite_y[s] - STILE_H / 2) <= j)
                               && ((yoff = j - y) < STILE_H)
                               && (c = stile[(t * STILE_H + yoff) * STILE_W + xoff])) {
#if USE_COLOR
                        if (colorize) {
                            if (! (a = stile_color[t]))
                                a = sprite_color[s];
                            a = pen[a];
                        }
                        else
#endif
#if USE_ATTR
#ifdef MY_A_BOLD
                            if (s == HERO)
                                a |= use_dim_and_bright ? MY_A_BOLD : 0;
#endif
#endif
                        ;
                        break;
                    }
                }
                if ((! c) && (ytile < MAZE_H) && (xtile < MAZE_W)) {
                    if (tile_used[(c = (unsigned char) maze[maze_level][ytile][xtile])]) {
#if USE_COLOR
                        if (colorize) {
                            a = tile_color[c];
                            a = pen[a];
                        }
                        else
#endif
#if USE_ATTR
                        {
#ifdef MY_A_BOLD
                            if (ISPELLET(c))
                                a |= use_dim_and_bright ? MY_A_BOLD : 0;
#endif
#ifdef MY_A_UNDERLINE
                            if (underline)
                                if (! (ISDOT(c) || ISPELLET(c) || ISOPEN(c) || ISDOOR(c)))
                                    a |= MY_A_UNDERLINE;
#endif
                        }
#endif
                        ;
                        if (debug) {
                            int s = WHOSE_HOME_DIR(ytile, xtile);
                            unsigned char d = home_dir[s][ytile][xtile];

                            c = (d == UP) ? '^'
                                : (d == DOWN) ? 'v'
                                : (d == LEFT) ? '<'
                                : (d == RIGHT) ? '>'
                                : ISDOT(c) ? ','
                                : ISPELLET(c) ? ';'
                                : ISOPEN(c) ? ' '
                                : ISDOOR(c) ? 'X'
                                : '@';
#if USE_COLOR
                            if (colorize && d) {
                                a = stile_color[sprite[MEAN(s)] + sprite_frame[MEAN(s)]];
                                if (! a)
                                    a = sprite_color[MEAN(s)];
                                a = pen[a];
                            }
#endif
                        }
                        else
                        {
                            if ((ISPELLET(c) && ((cycles / FIFTH) & 4) && (! dead) &&
                                 ! (sprite_used[HERO] && pause_timer)) ||
                                ((winning < ONESEC) && winning &&
                                 (ISDOT(c) || ISPELLET(c) || (((winning / FIFTH) & 4) && ISDOOR(c)))))
                                c = ' ';
                            else if ((winning < (2 * TWOSECS)) && ((winning / FIFTH) & 4) && ! pause_timer) {
#if USE_COLOR
                                if (colorize)
                                    a = pen[0xF];
                                else
#endif
                                    c = ' ';
                            }
                            c = tile[(c * TILE_H
                                      + j % TILE_H) * TILE_W
                                     + i % TILE_W];
                        }
                    }
                    else
                        c = ' ';
                }
                if (! c)
                    c = ' ';
                my_addch((unsigned long) (unsigned char) c, a);
            }
        }
        my_attrset(0);
        if (sprite_used[FRUIT] && (LINES > 6) && ! sound) {
            static char msg[8][12] = {" <  <N>  > ",
                                      "<  <ONU>  >",
                                      "  <BONUS>  ",
                                      " < BONUS > ",
                                      "<  BONUS  >",
                                      " > BONUS < ",
                                      "  >BONUS<  ",
                                      ">  >ONU<  <"};

            my_move(LINES - 1, 1 * (cjk_mode ? 2 : 1));
            my_addstr(msg[(cycles / FIFTH) & 7], 0);
        }
        if (pellet_timer && ! pause_timer) {
            int s, blue, mean;

            if (! -- pellet_timer) {
                for (s = 0; s < GHOSTS; s ++)
                    if (sprite_used[(blue = BLUE(s))]) {
                        sprite_used[(mean = MEAN(s))] = 1;
                        sprite_used[blue] = 0;
                        sprite_used[s] = VISIBLE_EYES;
                        sprite_x[s] =
                            (sprite_x[mean] = sprite_x[blue]);
                        sprite_y[s] =
                            (sprite_y[mean] = sprite_y[blue]);
                        sprite_frame[s] = (ghost_dir[s] = s + 1) - 1;
                    }
            } else if (pellet_timer <= TWOSECS)
                for (s = 0; s < GHOSTS; s ++)
                    sprite[BLUE(s)] = ((pellet_timer / ONESEC) & 1)
                        ? STILE_BLUE
                        : STILE_WHITE;
        }
        if ((LINES > 6) && (MY_COLS > 46))
        {
            static char buf[128];

            sprintf(buf,
                    " Level: %-10u Lives: %d Score: %-10u ",
                    level, lives, score);
            my_move(LINES - 1, (MY_COLS - 46) * (cjk_mode ? 2 : 1));
            my_addstr(buf, 0);
        }
        my_refresh();
        cycles ++;
    }
    my_attrset(0);
#if USE_ATTR
    curs_set(1); /* slcurses doesn't do this in endwin() */
#endif
    clear();
#if USE_COLOR
    if (colorize)
    {
        standout();
        mvprintw(LINES ? 1 : 0, 0, " ");
        standend();
        refresh();
        destroy_pen();
        mvprintw(LINES ? 1 : 0, 0, " ");
        addch('\n');
    }
#endif
    refresh();
    echo();
    endwin();
    fprintf(stderr, "%s: scored %d points, reached level %d\n",
            argv[0], score, level);
#ifdef XCURSES
    XCursesExit();
#endif
    return 0;
}
