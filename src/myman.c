/*BINFMTC:-DRAWCURSES -I../inc utils.c logic.c
 * myman.c - game logic for the MyMan video game
 * Copyright 1997-2009, Benjamin C. Wiley Sittler <bsittler@gmail.com>
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

/* configuration information */
#ifdef HAVE_CONFIG_H
#ifndef MYMAN_CONFIG_H_INCLUDED
#ifdef MY_CONFIG_H
#include MY_CONFIG_H
#else
#include "config.h"
#endif
#endif

#ifndef MYMAN_CONFIG_H_INCLUDED
#error "Got wrong \"config.h\"; add -DMY_CONFIG_H=\"{{builddir}}/config.h\""
#endif

#endif

/* feature guessing */
#ifndef MYMAN_GUESS_H_INCLUDED
#include "guess.h"
#endif

#ifndef MYMAN_GUESS_H_INCLUDED
#error "Got wrong \"guess.h\"; add -I{{srcdir}}/inc"
#endif

/* used in various WIN32 workaround functions */
#ifdef __BORLANDC__
#ifndef LIT64
#define LIT64(lit) lit##L
#endif
#endif

#ifndef LIT64
#define LIT64(lit) lit##LL
#endif

/* some Win32 bits use _UNICODE, others use UNICODE */
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE UNICODE
#endif /* ! defined(_UNICODE) */
#endif /* ! defined(UNICODE) */

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE _UNICODE
#endif /* ! defined(UNICODE) */
#endif /* defined(_UNICODE) */

/* needed for the argv[0] trick */
#if defined(macintosh)
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON

/* Case 1: flat Carbon headers */
#include <Carbon.h>

#else /* ! (defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON) */

/* Case 2: Toolbox */
#ifndef ANCIENT_HEADERS
#include <ConditionalMacros.h>
#endif

#ifdef __CARBON__
#ifdef TARGET_CPU_PPC
#undef GENERATINGPOWERPC
#define GENERATINGPOWERPC TARGET_CPU_PPC
#endif /* defined(TARGET_CPU_PPC) */
#ifdef TARGET_CPU_68K
#undef GENERATING68K
#define GENERATING68K TARGET_CPU_68K
#endif /* defined(TARGET_CPU_68K) */
#ifdef TARGET_RT_MAC_68881
#undef GENERATING68881
#define GENERATING68881 TARGET_RT_MAC_68881
#endif /* defined(TARGET_RT_MAC_68881) */
#ifdef TARGET_RT_MAC_CFM
#undef GENERATINGCFM
#define GENERATINGCFM TARGET_RT_MAC_CFM
#endif /* defined(TARGET_RT_MAC_CFM) */
#ifdef TARGET_RT_MAC_CFM
#undef CFMSYSTEMCALLS
#define CFMSYSTEMCALLS TARGET_RT_MAC_CFM
#endif /* defined(TARGET_RT_MAC_CFM) */
#endif /* defined(__CARBON__) */

/* when USE_OLD_TOOLBOX is non-zero we directly access Toolbox
 * internals which were deprecated or removed in Carbon, and avoid
 * routines which are available only in Carbon and MacOS PowerPC
 * runtime environments */
#ifndef USE_OLD_TOOLBOX
#if defined(__CARBON__) || (defined(GENERATINGPOWERPC) && GENERATINGPOWERPC)
#define USE_OLD_TOOLBOX 0
#else
#define USE_OLD_TOOLBOX 1
#endif
#endif /* ! defined(USE_OLD_TOOLBOX) */

#include <AppleEvents.h>
#include <CursorCtl.h>
#include <Errors.h>
#include <Files.h>
#include <Processes.h>
#include <Resources.h>
#include <Traps.h>

#endif /* ! (defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON) */

#else /* ! defined(macintosh) */

#ifdef MACCURSES

/* Case 3: Carbon framework */
#include <Carbon/Carbon.h>

#endif /* defined(MACCURSES) */

#endif /* ! defined(macintosh) */

#ifndef USE_OLD_TOOLBOX
#define USE_OLD_TOOLBOX 0
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifndef SIG_ERR
#define SIG_ERR ((void (*) (int)) -1)
#endif

#if defined(__PACIFIC__) || defined(HI_TECH_C) || defined(SMALL_C)
#ifndef HAVE_RAISE
#define HAVE_RAISE 0
#endif
#endif

#ifndef HAVE_RAISE
#define HAVE_RAISE 1
#endif

#if ! HAVE_RAISE
#ifdef raise
#undef raise
#endif
#define raise(sig) kill(getpid(), (sig))
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#include <math.h>
#include <ctype.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#if HAVE_IO_H
#include <io.h>
#endif
#include <stdio.h>
#if HAVE_UNIXIO_H
#include <unixio.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_TIME_H
#include <time.h>
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE (!(EXIT_SUCCESS))
#endif

#ifndef MYMAN_EXIT_USAGE
#if EXIT_SUCCESS
#define MYMAN_EXIT_USAGE EXIT_FAILURE
#else
#define MYMAN_EXIT_USAGE 2
#endif
#endif

/* MyMan utilities; also defines cruft like __MSDOS__ under some circumstances */
#ifndef MYMAN_UTILS_H_INCLUDED
#include "utils.h"
#endif

#ifndef MYMAN_UTILS_H_INCLUDED
#error "Got wrong \"utils.h\"; add -I{{srcdir}}/inc"
#endif

#define MYMAN__ENDSWITH(s1,s2) (myman_strrstr((s1), (s2)) && ! strcmp(myman_strrstr((s1), (s2)), (s2)))
#define MYMAN__CONTAINS(s1,s2) (strstr((s1), (s2)))
/* NOTE: s2 must be a string literal, and this relies on the compiler
 * merging successive string literals, as in ANSI C */
#define MYMAN__HASFLAG(s1,s2) (MYMAN__CONTAINS((s1), ("-"s2"-")) || MYMAN__ENDSWITH((s1), ("-"s2)))

#if HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#ifndef F_OK
#define F_OK 0
#endif

#ifndef USE_SDL_MIXER
#define USE_SDL_MIXER 0
#endif

#ifndef USE_SDL
#ifdef USE_SDL_MIXER
#undef USE_SDL_MIXER
#endif
#define USE_SDL_MIXER 0
#else
#if ! USE_SDL
#ifdef USE_SDL_MIXER
#undef USE_SDL_MIXER
#endif
#define USE_SDL_MIXER 0
#endif
#endif

/* SDL sometimes redefines main */
#ifdef USE_SDL
#if USE_SDL
#ifdef main
#define _SDL_main_h
#else
#define MAIN_NO_ENVP
#endif
#include <SDL.h>
#if USE_SDL_MIXER
#include <SDL_mixer.h>
#endif
#endif
#endif

/* terminal-screen handling library */

/* to get wcwidth */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#ifndef __USE_XOPEN
#define __USE_XOPEN 1
#endif

#ifdef NCURSES_XOPEN_HACK
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#endif

#ifndef MY_CURSES_H
#ifdef SLCURSES
#define MY_CURSES_H <slcurses.h>
#endif
#endif

#ifndef MY_CURSES_H
#ifdef XCURSES
#define MY_CURSES_H <xcurses.h>
#endif
#endif

#ifndef MY_CURSES_H
#ifdef CACACURSES
#include "cacacurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef GTKCURSES
#include "gtkcurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef FLTKCURSES
#include "fltkcurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef AACURSES
#define MY_CURSES_H "aacurses.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef NEWTCURSES
#include "newtcurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef DISPCURSES
#include "dispcurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef EFICURSES
#include "eficurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef CONIOCURSES
#include "coniocur.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef GRAPHCURSES
#include "graphcur.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef RAWCURSES
#include "rawcurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef MACCURSES
#include "maccurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef ALLEGROCURSES
#include "allegcur.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef TWCURSES
#include "twcurses.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef GGICURSES
#include "ggicurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifndef MY_CURSES_H
#ifdef SDLCURSES
#include "sdlcurs.h"
#define MY_CURSES_H "optcurs.h"
#endif
#endif

#ifdef MY_CURSES_H
#include MY_CURSES_H
#else
#include <curses.h>
#endif

#ifdef NCURSES_VERSION
#ifndef USE_TERM_H
#define USE_TERM_H 1
#endif
#endif

#ifndef USE_TERM_H
#define USE_TERM_H 0
#endif

#if USE_TERM_H
#ifdef MY_TERM_H
#include MY_TERM_H
#else
#include <term.h>
#endif
#endif

#ifdef OPTCURSES
#include "optcurs.h"
#endif

#if defined(__NetBSD__) && defined(_CURSES_H_) && (! defined(MY_CURSES_H)) && (! defined(NCURSES_VERSION))
#ifndef USE_NETBSD_CURSES
#define USE_NETBSD_CURSES 1
#endif
#endif

#ifndef USE_NETBSD_CURSES
#define USE_NETBSD_CURSES 0
#endif

/* work-arounds for NetBSD curses */
#if USE_NETBSD_CURSES

#ifndef use_env
#ifndef HAVE_USE_ENV
#define HAVE_USE_ENV 0
#endif
#endif

#ifndef USE_3ARGS_FOR_ATTR_SET
#define USE_3ARGS_FOR_ATTR_SET 1
#endif

/* NetBSD 5.1.2 curses uses wchar_t for the WACS_* #defines */
#ifndef USE_CCHAR_FOR_ALTCHARSET
#define USE_CCHAR_FOR_ALTCHARSET 0
#endif

#endif /* USE_NETBSD_CURSES */

/* work-arounds for old VMS curses */
#if defined(_VMS_CURSES) || defined(__VMS_CURSES)

#include <smg$routines.h>
#include <ssdef.h>

#pragma __nostandard

#ifndef OLDCURSES
#define OLDCURSES 1
#endif

#ifdef _BOLD
#ifndef HAVE_SETATTR
#define HAVE_SETATTR 1
#endif
#endif

#ifndef cbreak
#define cbreak() crmode()
#endif

#ifndef idlok
#define idlok(s,f)
#endif

#ifndef mvprintw
#define mvprintw mvaddstr
#endif

static int
vmscurses_getch(void)
{
    int res;
    my_ulong kbdid;
    my_ushort keycode = ERR;
    int timeout;

    kbdid = stdkb->_id;
    timeout = 0;
    res = smg$read_keystroke(&kbdid, &keycode, 0, &timeout);
    if (res == SS$_TIMEOUT) return ERR;
    if (res & 1) return keycode;
    return getch();
}

#ifdef getch
#undef getch
#endif
#define getch vmscurses_getch

#ifndef KEY_UP
#define KEY_UP SMG$K_TRM_UP
#endif

#ifndef KEY_DOWN
#define KEY_DOWN SMG$K_TRM_DOWN
#endif

#ifndef KEY_LEFT
#define KEY_LEFT SMG$K_TRM_LEFT
#endif

#ifndef KEY_RIGHT
#define KEY_RIGHT SMG$K_TRM_RIGHT
#endif

#ifndef keypad
#define keypad(s,f) OK
#endif

#ifndef curs_set
#define curs_set(f) ((smg$set_cursor_mode(&(stdkb->_id),(f)?SMG$M_CURSOR_ON:SMG$M_CURSOR_OFF)&1)?1:-1)
#endif

#pragma __standard

#endif

/* work-arounds for BSD 4.4 curses as seen on OpenVMS and BSD curses as seen on MacMiNT */
#if defined(_BSD44_CURSES) || defined(__BSD44_CURSES) || ! (defined(A_STANDOUT) || defined(A_UNDERLINE) || defined(A_BOLD) || defined(COLORS) || defined(COLOR_WHITE) || defined(KEY_LEFT) || defined(ACS_HLINE))

#ifndef OLDCURSES
#define OLDCURSES 1
#endif

#endif

/* work-arounds for old BSD curses */
#ifdef OLDCURSES

#ifndef DANGEROUS_ATTRS
#define DANGEROUS_ATTRS 1
#endif

#ifndef intrflush
#define intrflush(win, x)
#endif

#ifndef use_env
#ifndef HAVE_USE_ENV
#define HAVE_USE_ENV 0
#endif
#endif

#ifndef HAVE_NODELAY
#ifndef nodelay
#define HAVE_NODELAY 0
#endif
#endif

#ifndef HAVE_CURS_SET
#ifndef curs_set
#define HAVE_CURS_SET 0
#endif
#endif

#ifndef A_STANDOUT
#ifndef HAVE_ATTRSET
#ifndef attrset
#define HAVE_ATTRSET 0
#endif
#endif
#endif

#ifndef USE_ATTR
#define USE_ATTR 1
#endif

#if USE_ATTR
#ifndef A_STANDOUT
#ifdef _STANDOUT
#define A_STANDOUT _STANDOUT
#else
#ifdef __STANDOUT
#define A_STANDOUT ((__STANDOUT) << 8)
#else
#ifdef _REVERSE
#ifdef _BOLD
#define A_STANDOUT ((_BOLD | _REVERSE) << 8)
#else
#define A_STANDOUT ((_REVERSE) << 8)
#endif
#else
#define A_STANDOUT 0x100
#endif
#endif
#endif
#endif
#ifndef A_BLINK
#ifdef _BLINK
#define A_BLINK ((_BLINK) << 8)
#endif
#endif
#ifndef A_BOLD
#ifdef _BOLD
#define A_BOLD ((_BOLD) << 8)
#endif
#endif
#ifndef A_REVERSE
#ifdef _REVERSE
#define A_REVERSE ((_REVERSE) << 8)
#endif
#ifndef A_UNDERLINE
#ifdef _UNDERLINE
#define A_UNDERLINE ((_UNDERLINE) << 8)
#endif
#endif
#endif

#endif

#ifndef beep
#define beep() do{fputc('\a', stdout);fflush(stdout);}while(0)
#endif

#ifndef HAVE_CHTYPE
#ifndef chtype
#define HAVE_CHTYPE 0
#endif
#endif

#ifdef A_STANDOUT
#if A_STANDOUT == 0x80
#ifndef A_CHARTEXT
#define A_CHARTEXT 0x7f
#endif
#endif
#endif

#ifndef A_CHARTEXT
#define A_CHARTEXT 0xff
#endif

#ifndef USE_A_CHARTEXT
#define USE_A_CHARTEXT 1
#endif

#endif

/* work-arounds for slcurses */
#ifdef SLCURSES

#ifndef use_env
#ifndef HAVE_USE_ENV
#define HAVE_USE_ENV 0
#endif
#endif

#ifndef USE_A_CHARTEXT
#define USE_A_CHARTEXT 1
#endif

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
((*(f) = (p) ? ((SLtt_get_color_object ((p))) >> 16) : 7), \
 (*(b) = (p) ? (((SLtt_get_color_object ((p))) >> 8) & 0xff) : 0))
#endif

#endif /* defined(SLCURSES) */

/* HAVE_USE_ENV: does our curses implementation include use_env()? */

#ifndef HAVE_USE_ENV
#define HAVE_USE_ENV 1
#endif

#if ! HAVE_USE_ENV
#ifndef use_env
#define use_env(x)
#endif
#endif

/* HAVE_SETATTR: does our curses implementation include setattr()/clrattr()? */

#ifndef HAVE_SETATTR
#define HAVE_SETATTR 0
#endif

/* HAVE_NODELAY: does our curses implementation include nodelay()? */

#ifndef HAVE_NODELAY
#define HAVE_NODELAY 1
#endif

/* for resizing */

#ifndef USE_IOCTL
#ifdef __PDCURSES__
#define USE_IOCTL 0
#else
#define USE_IOCTL 1
#endif
#endif

#if USE_IOCTL
#if HAVE_IOCTL_H
#include <ioctl.h>
#else
#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#endif
#ifdef TIOCGWINSZ
#ifndef VMS
#include <termios.h>
#endif
#endif
#endif

#ifndef USE_SIGWINCH
#ifdef __PDCURSES__
#define USE_SIGWINCH 0
#else
#ifdef SIGWINCH
#define USE_SIGWINCH 1
#else
#define USE_SIGWINCH 0
#endif
#endif
#endif

/* Classic MacOS on the PowerPC (at least with MPW) seems to crash
 * when fileno(std{in,out,err}) is called. */
#ifndef HAVE_SAFE_FILENO
#if defined(macintosh) && ! USE_OLD_TOOLBOX
#define HAVE_SAFE_FILENO 0
#else
#define HAVE_SAFE_FILENO 1
#endif
#endif

/* command-line argument parser */
#ifndef MYGETOPT_H
#ifdef MYGETOPT
#define MYGETOPT_H "getopt.h"
#endif
#endif

#ifdef MYGETOPT_H
#include MYGETOPT_H
#else
#include <getopt.h>
#endif

#ifdef LC_CTYPE
static int locale_is_broken = 0;
static const char *myman_locale = 0;
#endif

/* character set conversion library */
#ifndef USE_ICONV
#define USE_ICONV 0
#endif

#if USE_ICONV
#ifdef MY_ICONV_H
#include MY_ICONV_H
#else
#include <iconv.h>
#endif
#ifndef wcwidth
#if HAVE_WCWIDTH
#if HAVE_WCHAR_H
#include <wchar.h>
#endif
#endif
#endif
#ifdef LC_CTYPE
#ifndef uint32_t
/* for uint32_t */
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#endif
#endif

#ifdef LC_CTYPE



#endif

static iconv_t cd_to_wchar = (iconv_t) -1;

static iconv_t cd_to_uni = (iconv_t) -1;

#ifdef LC_CTYPE
static int locale_is_ascii(void);
#endif

static wchar_t ucs_to_wchar(my_ulong ucs)
{
    wchar_t wcbuf[6];
#ifdef LC_CTYPE
    uint32_t ucsbuf[6];
    char *ibuf;
    char *obuf;
    size_t ibl;
    size_t obl;
    size_t u2w_ret;
    size_t w2u_ret;
    const char *my_locale = NULL;
    static const char *ucs_4_charset = NULL;
    static const char *wchar_t_charset = NULL;

    if (ucs_4_charset == NULL)
    {
        ucs_4_charset = myman_getenv("MYMAN_UCS4INTERNAL");
    }
    if ((ucs_4_charset == NULL) || ('\0' == *ucs_4_charset))
    {
        ucs_4_charset = "UCS-4-INTERNAL";
    }
    if (wchar_t_charset == NULL)
    {
        wchar_t_charset = myman_getenv("MYMAN_WCHAR_T");
    }
    if ((wchar_t_charset == NULL) || ('\0' == *wchar_t_charset))
    {
        wchar_t_charset = "WCHAR_T";
    }
    do
    {
        if (locale_is_ascii())
        {
            wcbuf[0] = 0;
            break;
        }
        my_locale = locale_is_broken ? NULL : setlocale(LC_CTYPE, myman_locale);
        if ((cd_to_wchar == (iconv_t) -1)
            &&
            ((cd_to_wchar = iconv_open(wchar_t_charset, ucs_4_charset)) == (iconv_t) -1))
        {
            wcbuf[0] = 0;
            break;
        }
        iconv(cd_to_wchar, NULL, NULL, NULL, NULL);
        memset((void *) ucsbuf, 0, sizeof(ucsbuf));
        ucsbuf[0] = ucs;
        ucsbuf[1] = 0;
        memset((void *) wcbuf, 0, sizeof(wcbuf));
        wcbuf[0] = 0;
        wcbuf[1] = 0;
        ibuf = (char *) (void *) ucsbuf;
        obuf = (char *) (void *) wcbuf;
        ibl = 2 * sizeof(*ucsbuf);
        obl = sizeof(wcbuf);
        u2w_ret = iconv(cd_to_wchar, &ibuf, &ibl, &obuf, &obl);
        if ((u2w_ret == (size_t) -1)
            ||
            wcbuf[1]
            ||
            (! wcbuf[0]))
        {
            wcbuf[0] = 0;
            break;
        }
        if ((cd_to_uni == (iconv_t) -1) &&
            (cd_to_uni = iconv_open(ucs_4_charset, wchar_t_charset)) == (iconv_t) -1)
        {
            wcbuf[0] = 0;
            break;
        }
        if (my_locale)
        {
            /* attempting this conversion in a broken locale causes
             * infinite loops inside iconv */
            iconv(cd_to_uni, NULL, NULL, NULL, NULL);
            ucsbuf[0] = 0;
            ibuf = (char *) (void *) wcbuf;
            obuf = (char *) (void *) ucsbuf;
            ibl = 2 * sizeof(*wcbuf);
            obl = sizeof(ucsbuf);
            w2u_ret = iconv(cd_to_uni, &ibuf, &ibl, &obuf, &obl);
            if ((w2u_ret == (size_t) -1)
                ||
                (ucsbuf[0] != ucs))
            {
                /* does not round-trip, probably a broken character */
                wcbuf[0] = 0;
                break;
            }
        }
    }
    while (0);
    if (my_locale)
    {
        setlocale(LC_CTYPE, my_locale);
    }
#else /* ! defined(LC_CTYPE) */
    wcbuf[0] = 0;
#endif /* ! defined(LC_CTYPE) */
    return wcbuf[0] ? wcbuf[0] : (((ucs >= 0x20) && (ucs <= 0x7e)) ? ((wchar_t) ucs) : 0);
}

#else

#define ucs_to_wchar(ucs) ((((my_ulong) (wchar_t) (my_ulong) (ucs)) == ((my_ulong) (ucs))) ? ((wchar_t) (my_ulong) (ucs)) : ((wchar_t) 0))

#endif

/* resize handler */
static int ever_got_sigwinch = 0;
static volatile int got_sigwinch = 0;

#if USE_SIGWINCH

static void (*old_sigwinch_handler)(int);

static void sigwinch_handler(int signum)
{
    if (signum == SIGWINCH)
    {
        got_sigwinch = 1;
    }
}

#endif

#ifndef CRLF
#define CRLF "\r\n"
#endif

#ifdef __PDCURSES__
#ifdef USE_SDL
#if USE_SDL
#ifndef USE_RAW
#define USE_RAW 1
#endif
#ifndef USE_RAW_UCS
#define USE_RAW_UCS 0
#endif /* ! defined(USE_RAW_UCS) */
#endif
#endif
#ifdef PDC_BUILD
#if PDC_BUILD >= 3300
/* PDCurses does implement init_color as of release 3.3 */
#ifndef USE_PALETTE
#define USE_PALETTE 1
#endif
#endif
#endif
/* PDCurses didn't implement init_color as of release 2.2 */
#ifndef USE_PALETTE
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

#ifndef CCHARW_MAX
#define CCHARW_MAX 6
#endif

#ifndef USE_KEYPAD
#ifdef KEY_LEFT
#define USE_KEYPAD 1
#else
#define USE_KEYPAD 0
#endif
#endif

#ifndef USE_A_CHARTEXT
#define USE_A_CHARTEXT 0
#endif

#ifndef MYMANCTRL
#define MYMANCTRL(x) (((x) == '\?') ? 0x7f : ((x) & ~0x60))
#endif

#ifndef KEY_LEFT
#define KEY_LEFT MYMANCTRL('B')
#endif

#ifndef KEY_RIGHT
#define KEY_RIGHT MYMANCTRL('F')
#endif

#ifndef KEY_UP
#define KEY_UP MYMANCTRL('P')
#endif

#ifndef KEY_DOWN
#define KEY_DOWN MYMANCTRL('N')
#endif

#define IS_LEFT_ARROW(k) ((k == 'h') || (k == 'H') || (k == '4') || (k == KEY_LEFT) || (k == MYMANCTRL('B')))
#define IS_RIGHT_ARROW(k) ((k == 'l') || (k == 'L') || (k == '6') || (k == KEY_RIGHT) || (k == MYMANCTRL('F')))
#define IS_UP_ARROW(k) ((k == 'k') || (k == 'K') || (k == '8') || (k == KEY_UP) || (k == MYMANCTRL('P')))
#define IS_DOWN_ARROW(k) ((k == 'j') || (k == 'J') || (k == '2') || (k == KEY_DOWN) || (k == MYMANCTRL('N')))

#ifndef USE_DIM_AND_BRIGHT
#define USE_DIM_AND_BRIGHT 1
#endif

#ifndef SWAPDOTS
#ifdef __PDCURSES__
#define SWAPDOTS 1
#endif
#endif

#ifndef SWAPDOTS
#define SWAPDOTS ((USE_WIDEC_SUPPORT) ? locale_is_utf8() : 0)
#endif

static int locale_charset_is(const char *charset)
{
    const char *my_locale;
    char *my_locale_lower;
    int is_same_charset = 0;
    int i;

#ifdef LC_CTYPE
    my_locale = locale_is_broken ? NULL : setlocale(LC_CTYPE, myman_locale);
    if (my_locale)
    {
        my_locale_lower = strdup(my_locale);
        if (my_locale_lower)
        {
            for (i = 0; my_locale_lower[i]; i ++)
            {
                my_locale_lower[i] = tolower(my_locale_lower[i]);
            }
            if (strstr(my_locale_lower, charset))
            {
                is_same_charset = 1;
            }
            free((void *) my_locale_lower);
            my_locale_lower = NULL;
        }
#ifdef CODESET
        {
            char *codeset, *codeset_lower;

            codeset = nl_langinfo(CODESET);
            if (codeset)
            {
                codeset_lower = strdup(codeset);
                if (codeset_lower)
                {
                    for (i = 0; codeset_lower[i]; i ++)
                    {
                        codeset_lower[i] = tolower(codeset_lower[i]);
                    }
                    if (! strcmp(codeset_lower, charset))
                    {
                        is_same_charset = 1;
                    }
                    free((void *) codeset_lower);
                    codeset_lower = NULL;
                }
            }
        }
#endif /* defined(CODESET) */
        setlocale(LC_CTYPE, my_locale);
    }
#endif /* defined(LC_CTYPE) */
    /* for broken systems that do not yet support UTF-8 locales
     * (Cygwin comes to mind) */
    my_locale = myman_getenv("MYMAN_LOCALE");
    if (! (my_locale && *my_locale)) my_locale = myman_getenv("LC_ALL");
    if (! (my_locale && *my_locale)) my_locale = myman_getenv("LC_CTYPE");
    if (! (my_locale && *my_locale)) my_locale = myman_getenv("LANG");
#ifdef __GLIBC__
    if (! (my_locale && *my_locale)) my_locale = myman_getenv("LANGUAGE");
#endif
    if (my_locale && *my_locale)
    {
        my_locale_lower = strdup(my_locale);
        if (my_locale_lower)
        {
            for (i = 0; my_locale_lower[i]; i ++)
            {
                my_locale_lower[i] = tolower(my_locale_lower[i]);
            }
            if (strstr(my_locale_lower, charset))
            {
                is_same_charset = 1;
            }
            free((void *) my_locale_lower);
            my_locale_lower = NULL;
        }
    }
    return is_same_charset;
}

static int cached_locale_is_utf8 = -1;

static int locale_is_utf8(void)
{
    if (cached_locale_is_utf8 == -1)
    {
        cached_locale_is_utf8 =
            locale_charset_is("utf8") ||
            locale_charset_is("utf8-mac") ||
            locale_charset_is("utf-8") ||
            locale_charset_is("utf-8-mac") ||
            locale_charset_is("u8") ||
            locale_charset_is("fss_utf") ||
            locale_charset_is("tf-8") ||
            locale_charset_is("utf-fss") ||
            locale_charset_is("utf-2");
    }
    return cached_locale_is_utf8;
}

#if USE_ICONV
#ifdef LC_CTYPE

static int cached_locale_is_cp437 = -1;

static int locale_is_cp437(void)
{
    if (locale_is_utf8())
    {
        return 0;
    }
    if (cached_locale_is_cp437 == -1)
    {
        cached_locale_is_cp437 =
            locale_charset_is("437") ||
            locale_charset_is("cp437") ||
            locale_charset_is("ibm-cp437") ||
            locale_charset_is("ibm437") ||
            locale_charset_is("ibm-pc") ||
            locale_charset_is("pc") ||
            locale_charset_is("msdos") ||
            locale_charset_is("dos");
    }
    return cached_locale_is_cp437;
}

static int cached_locale_is_ascii = -1;

static int locale_is_ascii(void)
{
    if (locale_is_utf8())
    {
        return 0;
    }
    if (locale_is_cp437())
    {
        return 0;
    }
    if (cached_locale_is_ascii == -1)
    {
        const char *my_locale;

        my_locale = locale_is_broken ? NULL : setlocale(LC_CTYPE, myman_locale);
        cached_locale_is_ascii =
            (! my_locale) ||
            (! *my_locale) ||
            (! strcmp(my_locale, "C")) ||
            (! strcmp(my_locale, "POSIX"));
        if (cached_locale_is_ascii)
        {
            return cached_locale_is_ascii;
        }
        if (my_locale)
        {
            setlocale(LC_CTYPE, my_locale);
        }
        cached_locale_is_ascii =
            locale_charset_is("367") ||
            locale_charset_is("ansi_x3.4-1968") ||
            locale_charset_is("ansi_x3.4-1986") ||
            locale_charset_is("ascii") ||
            locale_charset_is("ibm367") ||
            locale_charset_is("iso646-us") ||
            locale_charset_is("iso646.1991-irv") ||
            locale_charset_is("iso-ir-6") ||
            locale_charset_is("iso_646.irv:1991") ||
            locale_charset_is("us") ||
            locale_charset_is("us-ascii");
    }
    return cached_locale_is_ascii;
}

#endif
#endif

#ifndef MY_A_BOLD
#ifdef A_BOLD
#define MY_A_BOLD A_BOLD
#endif
#endif

#ifndef MY_A_UNDERLINE
#ifdef A_UNDERLINE
#define MY_A_UNDERLINE A_UNDERLINE
#endif
#endif

#ifndef MY_A_STANDOUT
#ifdef A_STANDOUT
#define MY_A_STANDOUT A_STANDOUT
#endif
#endif

#ifndef MY_A_REVERSE
#ifdef A_REVERSE
#define MY_A_REVERSE A_REVERSE
#endif
#endif

#ifndef MY_A_DIM
#ifdef A_DIM
#define MY_A_DIM A_DIM
#endif
#endif

#ifndef MY_A_ATTRIBUTES
#ifdef A_ATTRIBUTES
#define MY_A_ATTRIBUTES A_ATTRIBUTES
#endif
#endif

#ifndef MY_A_CHARTEXT
#ifdef A_CHARTEXT
#define MY_A_CHARTEXT A_CHARTEXT
#endif
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

#ifndef USE_UNDERLINE
#define USE_UNDERLINE 0
#endif

#ifndef USE_ATTR
#ifdef MY_A_BOLD
#define USE_ATTR 1
#else
#define USE_ATTR 0
#endif
#endif

#ifndef DANGEROUS_ATTRS
#define DANGEROUS_ATTRS 0
#endif

#ifndef HAVE_CURS_SET
#define HAVE_CURS_SET USE_ATTR
#endif

#if ! USE_ATTR
#ifndef USE_COLOR
#define USE_COLOR 0
#endif
#endif

#ifndef USE_BEEP
#define USE_BEEP 1
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
#else
#ifdef WACS_D_ULCORNER
#define MY_ACS_BDDB WACS_D_ULCORNER
#endif
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
#else
#ifdef WACS_D_LLCORNER
#define MY_ACS_DDBB WACS_D_LLCORNER
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
#else
#ifdef WACS_D_URCORNER
#define MY_ACS_BBDD WACS_D_URCORNER
#endif
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
#else
#ifdef WACS_D_LRCORNER
#define MY_ACS_DBBD WACS_D_LRCORNER
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
#else
#ifdef WACS_D_RTEE
#define MY_ACS_DBDD WACS_D_RTEE
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
#else
#ifdef WACS_D_LTEE
#define MY_ACS_DDDB WACS_D_LTEE
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
#else
#ifdef WACS_D_BTEE
#define MY_ACS_DDBD WACS_D_BTEE
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
#else
#ifdef WACS_D_TTEE
#define MY_ACS_BDDD WACS_D_TTEE
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
#else
#ifdef WACS_D_HLINE
#define MY_ACS_BDBD WACS_D_HLINE
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
#else
#ifdef WACS_D_VLINE
#define MY_ACS_DBDB WACS_D_VLINE
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
#else
#ifdef WACS_D_PLUS
#define MY_ACS_DDDD WACS_D_PLUS
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
#ifndef MY_ACS_PLMINUS
#ifdef WACS_PLMINUS
#define MY_ACS_PLMINUS WACS_PLMINUS
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
#ifndef MY_ACS_PLMINUS
#ifdef ACS_PLMINUS
#define MY_ACS_PLMINUS ACS_PLMINUS
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

#ifdef BUILTIN_SIZE
extern const char *builtin_size;
#ifdef MYMANSIZE
#undef MYMANSIZE
#endif
#define MYMANSIZE builtin_size
#else
#ifndef MYMANSIZE
#define MYMANSIZE "big"
#endif
static const char *MYMANSIZE_str = MYMANSIZE;
#ifdef MYMANSIZE
#undef MYMANSIZE
#endif
#define MYMANSIZE MYMANSIZE_str
#endif

#ifndef TILEFILE
#define TILEFILE "chr/chr5x2.txt"
#endif

#ifdef BUILTIN_TILE
#ifdef TILEFILE
#undef TILEFILE
#endif
#define TILEFILE 0
extern const char *tile_s;
#else
static const char TILEFILE_str[] = TILEFILE;
#ifdef TILEFILE
#undef TILEFILE
#endif
#define TILEFILE TILEFILE_str
#define tile_s TILEFILE
#endif

#ifndef SPRITEFILE
#define SPRITEFILE "spr/spr7x3.txt"
#endif

#ifdef BUILTIN_SPRITE
#ifdef SPRITEFILE
#undef SPRITEFILE
#endif
#define SPRITEFILE 0
extern const char *sprite_s;
#else
static const char SPRITEFILE_str[] = SPRITEFILE;
#ifdef SPRITEFILE
#undef SPRITEFILE
#endif
#define SPRITEFILE SPRITEFILE_str
#define sprite_s SPRITEFILE
#endif

#ifndef HAVE_CHTYPE
#define HAVE_CHTYPE 1
#endif

#ifndef HAVE_ATTRSET
#define HAVE_ATTRSET 1
#endif

#ifndef USE_ISATTY
#define USE_ISATTY 1
#endif

/* NOTE: while this is actually "char" inside the old BSD libcurses,
 * the old C calling conventions mean we can safely use int instead,
 * and it will eventually get coerced */
#if ! HAVE_CHTYPE
#ifndef chtype
#define chtype int
#endif
#endif

#ifdef _XOPEN_SOURCE_EXTENDED
#ifndef USE_3ARGS_FOR_ATTR_SET
#define USE_3ARGS_FOR_ATTR_SET 1
#endif
#endif

#ifdef __PDCURSES__
#ifndef USE_3ARGS_FOR_ATTR_SET
#define USE_3ARGS_FOR_ATTR_SET 1
#endif
#endif

#ifdef NCURSES_VERSION
#ifndef USE_3ARGS_FOR_ATTR_SET
#define USE_3ARGS_FOR_ATTR_SET 1
#endif
#endif

/* older SysV curses apparently took a single argument for attr_set */
#ifndef USE_3ARGS_FOR_ATTR_SET
#define USE_3ARGS_FOR_ATTR_SET 0
#endif

/* mapping from CP437 to VT-100 altcharset */
#if USE_WIDEC_SUPPORT

#ifndef USE_CCHAR_FOR_ALTCHARSET
#define USE_CCHAR_FOR_ALTCHARSET 1
#endif

#if USE_CCHAR_FOR_ALTCHARSET

#ifndef MY_WACS_ARE_PTRS
#ifdef NCURSES_VERSION
#define MY_WACS_ARE_PTRS 1
#endif
#endif

#ifndef MY_WACS_ARE_PTRS
#ifdef __PDCURSES__
#define MY_WACS_ARE_PTRS 1
#endif
#endif

#ifndef MY_WACS_ARE_PTRS
#define MY_WACS_ARE_PTRS 0
#endif

/* ncurses and PDCurses define WACS_* as cchar_t * whereas unix95 (at
 * least according to TenDRA) defines them as cchar_t */
#if MY_WACS_ARE_PTRS
#ifndef MY_WACS_PTR
#define MY_WACS_PTR
#endif
#ifndef MY_WACS_REF
#define MY_WACS_REF *
#endif
#else
#ifndef MY_WACS_PTR
#define MY_WACS_PTR &
#endif
#ifndef MY_WACS_REF
#define MY_WACS_REF
#endif
#endif

static cchar_t MY_WACS_REF

#else

static wchar_t

#endif

#else /* ! USE_WIDEC_SUPPORT */
static chtype
#endif /* ! USE_WIDEC_SUPPORT */
altcharset_cp437[256];

#ifndef USE_CCHAR_FOR_ALTCHARSET
#define USE_CCHAR_FOR_ALTCHARSET 0
#endif

#if USE_CCHAR_FOR_ALTCHARSET
#define MY_NULL_ACS NULL
#else
#define MY_NULL_ACS 0
#endif

/* mapping from CP437 to ASCII */
static chtype
ascii_cp437[256];

#ifdef NEED_CP437_TO_ASCII
static chtype cp437_to_ascii(my_uchar ch)
{
    return ascii_cp437[(((unsigned) ch) & 0xff)];
}
#endif

#ifndef USE_WCWIDTH
#if HAVE_WCWIDTH
#define USE_WCWIDTH 1
#endif
#endif

#ifndef USE_WCWIDTH
#ifdef wcwidth
#define USE_WCWIDTH 1
#endif
#endif

#ifndef USE_WCWIDTH
#if USE_WIDEC_SUPPORT || USE_RAW_UCS || USE_ICONV
#define USE_WCWIDTH 1
#else
#define USE_WCWIDTH 0
#endif
#endif

#ifndef ADDSTR_MULTIBYTE
#ifdef NCURSES_VERSION
#define ADDSTR_MULTIBYTE 1
#endif
#endif

#ifndef ADDSTR_MULTIBYTE
#define ADDSTR_MULTIBYTE 0
#endif

static int terminal_wcwidth = 0;

static int my_builtin_wcwidth(wchar_t wc)
{
    if ((terminal_wcwidth > 0)
        &&
        (wc >= 0x0080))
    {
        return terminal_wcwidth;
    }
/* FIXME: this horrible hack is for Droid Sans Mono */
    if ((terminal_wcwidth == -2)
        &&
        (((wc >= 0x2180) && (wc <= 0x227f))
         ||
         ((wc >= 0x2600) && (wc <= 0x267f))))
    {
        return 2;
    }
    return (((((long) (wc)) >= 0x0020L) && (((long) (wc)) < 0x3000L)) ? 1 : ((((long) (wc)) >= 0x3000L) && (((long) (wc)) <= 0x10fffdL)) ? 2 : -1);
}

#if USE_WCWIDTH

#if ! USE_ICONV
/* for wcwidth(3) */
#ifndef wcwidth
#include <wchar.h>
#endif
#endif

static int wcwidth_is_suspect = -1;

static int my_wcwidth(wchar_t wc)
{
    int len;

    if (terminal_wcwidth > 0)
    {
        return my_builtin_wcwidth(wc);
    }
    len = wcwidth(wc);
/* FIXME: this horrible hack is for Droid Sans Mono */
    if ((terminal_wcwidth == -2)
        &&
        (((wc >= 0x2180) && (wc <= 0x227f))
         ||
         ((wc >= 0x2600) && (wc <= 0x267f))))
    {
        return 2;
    }
    if ((len <= 0) && (terminal_wcwidth != 0))
    {
        return my_builtin_wcwidth(wc);
    }
    if ((len == 1) && (wc & ~0xff))
    {
        if (wcwidth_is_suspect == -1)
        {
            wchar_t twc;

            wcwidth_is_suspect = 0;
            twc = ucs_to_wchar(0xff21); /* U+FF21 FULLWIDTH LATIN CAPITAL LETTER A */
            if (twc
                &&
                (twc != (wchar_t)0xff21)
                &&
                (twc & ~0xff)
                &&
                (wcwidth(twc) == 1))
            {
                wcwidth_is_suspect = 1;
            }
        }
        if (wcwidth_is_suspect)
        {
            len = 2;
        }
    }
    return len;
}

#else

/* FIXME: this is a horrible hack! */
#define my_wcwidth(wc) my_builtin_wcwidth(wc)

#endif

#if USE_WIDEC_SUPPORT

static int
my_wcswidth(const wchar_t *s, size_t n)
{
    int ret = 0;

    while (((n --) > 0) && (*s != L'\0'))
    {
        int cret = my_wcwidth(*s);
        if (cret != -1)
        {
            ret += cret;
        }
        s ++;
    }
    return ret;
}

#endif

static int use_raw = USE_RAW;

static int use_raw_ucs = USE_RAW_UCS;

static int use_raw_cp437 = 1;

static int use_revblock = 0;

static int use_bgspace = 1;

static int use_fgblock = 0;

static int use_fixedpal = 0;

static int fold_lower_case_to_upper_case = 0;

static int fold_upper_case_to_lower_case = 0;

static int use_debug_input = 0;

#if USE_COLOR

static int terminal_color_scale = 1000;

static int terminal_colors = -1;

static int terminal_color_pairs = -1;

#endif

int use_direct = 0;

int use_basic_acs = 0;

int use_extra_acs = 0;

static int location_is_suspect = 0;

static void
init_trans(int use_bullet_for_dots)
{
    int i;

    if (myman_getenv("MYMAN_CP437") && *(myman_getenv("MYMAN_CP437")))
    {
        use_raw_cp437 = atoi(myman_getenv("MYMAN_CP437"));
    }
    else
    {
        use_raw_cp437 = 1;
#if ADDSTR_MULTIBYTE
        if (locale_is_utf8())
        {
            use_raw_cp437 = 0;
        }
#if USE_ICONV
#ifdef LC_CTYPE
        /* Since most machines do not have CP437 locales we treat
         * ASCII as CP437 so that using CP437 only requires an
         * appropriate font and the "C" locale. */
        if (! (locale_is_ascii() || locale_is_cp437()))
        {
            use_raw_cp437 = 0;
        }
#endif
#endif
#endif
    }
#ifdef NCURSES_VERSION
    if (myman_getenv("MYMAN_LEGACY_CODING") && *myman_getenv("MYMAN_LEGACY_CODING")) {
        use_legacy_coding(atoi(myman_getenv("MYMAN_LEGACY_CODING")));
    }
    else if (use_raw && !use_raw_ucs)
    {
#if USE_ICONV
#ifdef LC_CTYPE
        if (locale_is_cp437() && use_raw_cp437)
        {
            use_legacy_coding(2);
        }
        else
#endif
#endif
        {
            if (locale_is_utf8())
            {
                use_legacy_coding(0);
            }
            else
            {
                use_legacy_coding(2);
            }
        }
    }
    else
    {
        use_legacy_coding(0);
    }
#endif
    location_is_suspect = 0;
    for (i = 0; i < 256; i ++)
        if (isprint(i)
            &&
            (i >= ' ') && (i <= '~'))
        {
            altcharset_cp437[i] =
#if USE_WIDEC_SUPPORT
                MY_NULL_ACS;
#endif
            ascii_cp437[i] = i;
        }
        else
        {
            int j;

            j = i;
            while ((j < ' ') || (j > '~')
                   ||
                   (! isprint(j)))
            {
                int k;

                k = fallback_cp437[j];
                if (j == k) break;
                j = k;
            }
            altcharset_cp437[i] =
#if USE_WIDEC_SUPPORT
                MY_NULL_ACS;
#endif
            ascii_cp437[i] = j;
        }
    altcharset_cp437[19] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[19] = '!';
    altcharset_cp437[220] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[220] = ',';
    altcharset_cp437[221] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[221] = '#';
    altcharset_cp437[222] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[222] = '#';
    altcharset_cp437[223] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[223] = '\"';
    
    altcharset_cp437[201] =
        altcharset_cp437[218] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[201] =
        ascii_cp437[218] =
        '+';
    altcharset_cp437[200] =
        altcharset_cp437[192] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[200] =
        ascii_cp437[192] =
        '+';
    altcharset_cp437[187] =
        altcharset_cp437[191] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[187] =
        ascii_cp437[191] =
        '+';
    altcharset_cp437[188] =
        altcharset_cp437[217] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[188] =
        ascii_cp437[217] =
        '+';
    altcharset_cp437[183] =
        altcharset_cp437[189] =
        altcharset_cp437[185] =
        altcharset_cp437[181] =
        altcharset_cp437[182] =
        altcharset_cp437[180] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[189] =
        ascii_cp437[183] =
        ascii_cp437[185] =
        ascii_cp437[181] =
        ascii_cp437[182] =
        ascii_cp437[180] =
        '+';
    altcharset_cp437[214] =
        altcharset_cp437[211] =
        altcharset_cp437[204] =
        altcharset_cp437[198] =
        altcharset_cp437[199] =
        altcharset_cp437[195] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[211] =
        ascii_cp437[214] =
        ascii_cp437[204] =
        ascii_cp437[198] =
        ascii_cp437[199] =
        ascii_cp437[195] =
        '+';
    altcharset_cp437[212] =
        altcharset_cp437[190] =
        altcharset_cp437[202] =
        altcharset_cp437[207] =
        altcharset_cp437[208] =
        altcharset_cp437[193] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[190] =
        ascii_cp437[212] =
        ascii_cp437[202] =
        ascii_cp437[207] =
        ascii_cp437[208] =
        ascii_cp437[193] =
        '+';
    altcharset_cp437[213] =
        altcharset_cp437[184] =
        altcharset_cp437[203] =
        altcharset_cp437[209] =
        altcharset_cp437[210] =
        altcharset_cp437[194] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[184] =
        ascii_cp437[213] =
        ascii_cp437[203] =
        ascii_cp437[209] =
        ascii_cp437[210] =
        ascii_cp437[194] =
        '+';
    altcharset_cp437[205] =
        altcharset_cp437[196] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[205] =
        ascii_cp437[196] =
        '-';
    altcharset_cp437[186] =
        altcharset_cp437[179] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[186] =
        ascii_cp437[179] =
        '|';
    altcharset_cp437[215] =
        altcharset_cp437[216] =
        altcharset_cp437[206] =
        altcharset_cp437[197] =
        altcharset_cp437[4] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[206] =
        ascii_cp437[215] =
        ascii_cp437[216] =
        ascii_cp437[197] =
        ascii_cp437[4] =
        '+';
    altcharset_cp437[248] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[248] =
        '\'';
    altcharset_cp437[241] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[241] =
        '#';
    altcharset_cp437[7] =
#if ! USE_WIDEC_SUPPORT
        altcharset_cp437[8] =
#endif
        altcharset_cp437[9] =
        altcharset_cp437[254] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[7] =
        ascii_cp437[8] =
        ascii_cp437[9] =
        ascii_cp437[254] =
        'o';
    altcharset_cp437[25] =
        altcharset_cp437[31] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[25] =
        ascii_cp437[31] =
        'v';
    altcharset_cp437[24] =
        altcharset_cp437[30] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[24] =
        ascii_cp437[30] =
        '^';
    altcharset_cp437[15] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[15] =
        '#';
    altcharset_cp437[176] =
        altcharset_cp437[177] =
        altcharset_cp437[178] =
        altcharset_cp437[10] =
        altcharset_cp437[219] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[176] =
        ascii_cp437[177] =
        ascii_cp437[178] =
        ascii_cp437[10] =
        ascii_cp437[219] =
        '#';
#if USE_ATTR
#ifdef MY_A_REVERSE
    if (use_revblock ? MY_A_REVERSE : 0)
    {
        if (! (MY_A_REVERSE & 0xff))
        {
            altcharset_cp437[219] =
#if ! USE_WIDEC_SUPPORT
                ' ' | MY_A_REVERSE;
#else
            MY_NULL_ACS;
#endif
        }
        if (! (MY_A_REVERSE & 0x7f))
        {
            ascii_cp437[219] = ' ' | MY_A_REVERSE;
        }
    }
#endif
#endif
    altcharset_cp437[27] =
        altcharset_cp437[17] =
        altcharset_cp437[174] =
        altcharset_cp437[243] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[174] =
        ascii_cp437[243] =
        ascii_cp437[27] =
        ascii_cp437[17] =
        '<';
    altcharset_cp437[26] =
        altcharset_cp437[16] =
        altcharset_cp437[175] =
        altcharset_cp437[242] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[175] =
        ascii_cp437[242] =
        ascii_cp437[26] =
        ascii_cp437[16] =
        '>';
    altcharset_cp437[227] =
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
    ascii_cp437[227] =
        'n';
    altcharset_cp437[156] =
#if USE_WIDEC_SUPPORT
    altcharset_cp437[0] = MY_NULL_ACS;
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
    altcharset_cp437[255] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[255] =
        ' ';
    altcharset_cp437[158] =
#if USE_WIDEC_SUPPORT
        MY_NULL_ACS;
#endif
    ascii_cp437[158] =
        'P';
    if (use_basic_acs)
    {
        altcharset_cp437[201] =
        altcharset_cp437[218] =
#ifdef MY_ACS_BSSB
            MY_ACS_BSSB;
#else
#ifdef MY_ACS_ULCORNER
        MY_ACS_ULCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[201] =
            ascii_cp437[218] =
            '+';
        altcharset_cp437[200] =
            altcharset_cp437[192] =
#ifdef MY_ACS_SSBB
            MY_ACS_SSBB;
#else
#ifdef MY_ACS_LLCORNER
        MY_ACS_LLCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[200] =
            ascii_cp437[192] =
            '+';
        altcharset_cp437[187] =
            altcharset_cp437[191] =
#ifdef MY_ACS_BBSS
            MY_ACS_BBSS;
#else
#ifdef MY_ACS_URCORNER
        MY_ACS_URCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[187] =
            ascii_cp437[191] =
            '+';
        altcharset_cp437[188] =
            altcharset_cp437[217] =
#ifdef MY_ACS_SBBS
            MY_ACS_SBBS;
#else
#ifdef MY_ACS_LRCORNER
        MY_ACS_LRCORNER;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[188] =
            ascii_cp437[217] =
            '+';
        altcharset_cp437[183] =
            altcharset_cp437[189] =
            altcharset_cp437[185] =
            altcharset_cp437[181] =
            altcharset_cp437[182] =
            altcharset_cp437[180] =
#ifdef MY_ACS_SBSS
            MY_ACS_SBSS;
#else
#ifdef MY_ACS_RTEE
        MY_ACS_RTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[189] =
            ascii_cp437[183] =
            ascii_cp437[185] =
            ascii_cp437[181] =
            ascii_cp437[182] =
            ascii_cp437[180] =
            '+';
        altcharset_cp437[214] =
            altcharset_cp437[211] =
            altcharset_cp437[204] =
            altcharset_cp437[198] =
            altcharset_cp437[199] =
            altcharset_cp437[195] =
#ifdef MY_ACS_SSSB
            MY_ACS_SSSB;
#else
#ifdef MY_ACS_LTEE
        MY_ACS_LTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[211] =
            ascii_cp437[214] =
            ascii_cp437[204] =
            ascii_cp437[198] =
            ascii_cp437[199] =
            ascii_cp437[195] =
            '+';
        altcharset_cp437[212] =
            altcharset_cp437[190] =
            altcharset_cp437[202] =
            altcharset_cp437[207] =
            altcharset_cp437[208] =
            altcharset_cp437[193] =
#ifdef MY_ACS_SSBS
            MY_ACS_SSBS;
#else
#ifdef MY_ACS_BTEE
        MY_ACS_BTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[190] =
            ascii_cp437[212] =
            ascii_cp437[202] =
            ascii_cp437[207] =
            ascii_cp437[208] =
            ascii_cp437[193] =
            '+';
        altcharset_cp437[213] =
            altcharset_cp437[184] =
            altcharset_cp437[203] =
            altcharset_cp437[209] =
            altcharset_cp437[210] =
            altcharset_cp437[194] =
#ifdef MY_ACS_BSSS
            MY_ACS_BSSS;
#else
#ifdef MY_ACS_TTEE
        MY_ACS_TTEE;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[184] =
            ascii_cp437[213] =
            ascii_cp437[203] =
            ascii_cp437[209] =
            ascii_cp437[210] =
            ascii_cp437[194] =
            '+';
        altcharset_cp437[205] =
            altcharset_cp437[196] =
#ifdef MY_ACS_BSBS
            MY_ACS_BSBS;
#else
#ifdef MY_ACS_HLINE
        MY_ACS_HLINE;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[205] =
            ascii_cp437[196] =
            '-';
        altcharset_cp437[186] =
            altcharset_cp437[179] =
#ifdef MY_ACS_SBSB
            MY_ACS_SBSB;
#else
#ifdef MY_ACS_VLINE
        MY_ACS_VLINE;
#endif
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[186] =
            ascii_cp437[179] =
            '|';
        altcharset_cp437[215] =
            altcharset_cp437[216] =
            altcharset_cp437[206] =
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
        altcharset_cp437[0] = MY_NULL_ACS;
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
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[248] =
            '\'';
        altcharset_cp437[241] =
#ifdef  MY_ACS_PLMINUS
            MY_ACS_PLMINUS;
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[241] =
            '#';
        altcharset_cp437[7] =
#if ! USE_WIDEC_SUPPORT
            altcharset_cp437[8] =
#endif
            altcharset_cp437[9] =
            altcharset_cp437[254] =
#ifdef MY_ACS_BULLET
            MY_ACS_BULLET;
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[7] =
            ascii_cp437[8] =
            ascii_cp437[9] =
            ascii_cp437[254] =
            'o';
        altcharset_cp437[25] =
            altcharset_cp437[31] =
#ifdef MY_ACS_DARROW
            MY_ACS_DARROW;
#else
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
#endif
        ascii_cp437[25] =
            ascii_cp437[31] =
            'v';
        altcharset_cp437[24] =
            altcharset_cp437[30] =
#ifdef MY_ACS_UARROW
            MY_ACS_UARROW;
#else
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
#endif
        ascii_cp437[24] =
            ascii_cp437[30] =
            '^';
        altcharset_cp437[15] =
#ifdef MY_ACS_LANTERN
            MY_ACS_LANTERN;
#else
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
#endif
        ascii_cp437[15] =
            '#';
        if (! (use_revblock ? MY_A_REVERSE : 0))
        {
            altcharset_cp437[10] =
                altcharset_cp437[219] =
#ifdef MY_ACS_BLOCK
                MY_ACS_BLOCK;
#endif
#if USE_WIDEC_SUPPORT
            altcharset_cp437[0] = MY_NULL_ACS;
#endif
            ascii_cp437[10] =
                ascii_cp437[219] =
                '#';
        }
        altcharset_cp437[176] =
#ifdef MY_ACS_BOARD
            MY_ACS_BOARD;
#endif
        altcharset_cp437[177] =
            altcharset_cp437[178] =
#ifdef MY_ACS_CKBOARD
            MY_ACS_CKBOARD;
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[176] =
            ascii_cp437[177] =
            ascii_cp437[178] =
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
        altcharset_cp437[0] = MY_NULL_ACS;
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
        altcharset_cp437[0] = MY_NULL_ACS;
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
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[227] =
            'n';
        altcharset_cp437[156] =
#ifdef MY_ACS_STERLING
            MY_ACS_STERLING;
#endif
#if USE_WIDEC_SUPPORT
        altcharset_cp437[0] = MY_NULL_ACS;
#endif
        ascii_cp437[156] =
            'f';
    }
    if (use_extra_acs) {
#ifdef MY_ACS_BDDB
        altcharset_cp437[201] =
            MY_ACS_BDDB;
#endif
#ifdef MY_ACS_DDBB
        altcharset_cp437[200] =
            MY_ACS_DDBB;
#endif
#ifdef MY_ACS_BBDD
        altcharset_cp437[187] =
            MY_ACS_BBDD;
#endif
#ifdef MY_ACS_DBBD
        altcharset_cp437[188] =
            MY_ACS_DBBD;
#endif
#ifdef MY_ACS_DBDD
        altcharset_cp437[185] =
            MY_ACS_DBDD;
#endif
#ifdef MY_ACS_SBSD
        altcharset_cp437[181] =
            MY_ACS_SBSD;
#endif
#ifdef MY_ACS_DBDS
        altcharset_cp437[182] =
            MY_ACS_DBDS;
#endif
#ifdef MY_ACS_DDDB
        altcharset_cp437[204] =
            MY_ACS_DDDB;
#endif
#ifdef MY_ACS_SDSB
        altcharset_cp437[198] =
            MY_ACS_SDSB;
#endif
#ifdef MY_ACS_DSDB
        altcharset_cp437[199] =
            MY_ACS_DSDB;
#endif
#ifdef MY_ACS_DDBD
        altcharset_cp437[202] =
            MY_ACS_DDBD;
#endif
#ifdef MY_ACS_SDBD
        altcharset_cp437[207] =
            MY_ACS_SDBD;
#endif
#ifdef MY_ACS_DSBS
        altcharset_cp437[208] =
            MY_ACS_DSBS;
#endif
#ifdef MY_ACS_BDDD
        altcharset_cp437[203] =
            MY_ACS_BDDD;
#endif
#ifdef MY_ACS_BDSD
        altcharset_cp437[209] =
            MY_ACS_BDSD;
#endif
#ifdef MY_ACS_BSDS
        altcharset_cp437[210] =
            MY_ACS_BSDS;
#endif
#ifdef MY_ACS_BDSS
        altcharset_cp437[213] =
            MY_ACS_BDSS;
#else
#ifdef MY_ACS_BDSB
        altcharset_cp437[213] =
            MY_ACS_BDSB;
#endif
#endif
#ifdef MY_ACS_SSDB
        altcharset_cp437[214] =
            MY_ACS_SSDB;
#else
#ifdef MY_ACS_BSDB
        altcharset_cp437[214] =
            MY_ACS_BSDB;
#endif
#endif
#ifdef MY_ACS_SDBS
        altcharset_cp437[212] =
            MY_ACS_SDBS;
#else
#ifdef MY_ACS_SDBB
        altcharset_cp437[212] =
            MY_ACS_SDBB;
#endif
#endif
#ifdef MY_ACS_DSSB
        altcharset_cp437[211] =
            MY_ACS_DSSB;
#else
#ifdef MY_ACS_DSBB
        altcharset_cp437[211] =
            MY_ACS_DSBB;
#endif
#endif
#ifdef MY_ACS_BSSD
        altcharset_cp437[184] =
            MY_ACS_BSSD;
#else
#ifdef MY_ACS_BBSD
        altcharset_cp437[184] =
            MY_ACS_BBSD;
#endif
#endif
#ifdef MY_ACS_SBDS
        altcharset_cp437[183] =
            MY_ACS_SBDS;
#else
#ifdef MY_ACS_BBDS
        altcharset_cp437[183] =
            MY_ACS_BBDS;
#endif
#endif
#ifdef MY_ACS_SSBD
        altcharset_cp437[190] =
            MY_ACS_SSBD;
#else
#ifdef MY_ACS_SBBD
        altcharset_cp437[190] =
            MY_ACS_SBBD;
#endif
#endif
#ifdef MY_ACS_DBSS
        altcharset_cp437[189] =
            MY_ACS_DBSS;
#else
#ifdef MY_ACS_DBBS
        altcharset_cp437[189] =
            MY_ACS_DBBS;
#endif
#endif
#ifdef MY_ACS_BDBD
        altcharset_cp437[205] =
            MY_ACS_BDBD;
#endif
#ifdef MY_ACS_DBDB
        altcharset_cp437[186] =
            MY_ACS_DBDB;
#endif
#ifdef MY_ACS_DDDD
        altcharset_cp437[206] =
            MY_ACS_DDDD;
#endif
#ifdef MY_ACS_DSDS
        altcharset_cp437[215] =
            MY_ACS_DSDS;
#endif
#ifdef MY_ACS_SDSD
        altcharset_cp437[216] =
            MY_ACS_SDSD;
#endif
    }
#if USE_ATTR
#ifdef MY_A_REVERSE
#if ! USE_WIDEC_SUPPORT
    if (! (MY_A_REVERSE & 0xff))
    {
        altcharset_cp437[8] |= MY_A_REVERSE;
    }
#endif
    if (! (MY_A_REVERSE & 0x7f))
    {
        ascii_cp437[8] |= MY_A_REVERSE;
    }
#endif
#endif
    if (use_bullet_for_dots)
    {
        altcharset_cp437[249] =
            altcharset_cp437[250] = altcharset_cp437[254];
        altcharset_cp437[254] =
#if USE_WIDEC_SUPPORT
            MY_NULL_ACS;
#else
        'o';
#endif
    }
    else
    {
        altcharset_cp437[249] =
            altcharset_cp437[250] =
#if USE_WIDEC_SUPPORT
            MY_NULL_ACS;
#else
        '.';
#endif
    }
}

int use_underline = USE_UNDERLINE;

static int use_idlok = 1;

static int use_ioctl = USE_IOCTL;
static int use_resize = 1;
static int use_transparency = 1;

static int use_acs = 1;
static int use_acs_p = 0;

int use_dim_and_bright = 0;
static int use_dim_and_bright_p = 0;

int use_color = 0;
static int use_color_p = 0;

static int use_bullet_for_dots = 0;
static int use_bullet_for_dots_p = 0;

static int quit_requested = 0;

int reinit_requested = 0;

int xoff_received = 0;

int use_fullwidth = 0;
int use_bitmap = 0;

int use_rounded = 0;
int use_double = 0;
int use_single = 0;

int use_sound = SOUND;

#define MY_COLS (COLS / (use_fullwidth ? 2 : 1))

#ifdef BUILTIN_VARIANT
extern const char *builtin_variant;
#ifdef MYMANVARIANT
#undef MYMANVARIANT
#endif
#define MYMANVARIANT builtin_variant
#else
#ifndef MYMANVARIANT
#define MYMANVARIANT "myman"
#endif
static const char *MYMANVARIANT_str = MYMANVARIANT;
#ifdef MYMANVARIANT
#undef MYMANVARIANT
#endif
#define MYMANVARIANT MYMANVARIANT_str
#endif


#ifndef MAZEFILE
#define MAZEFILE "lvl/maze.txt"
#endif

#ifdef BUILTIN_MAZE
extern const char **maze_d;
extern const char **maze_c;
extern const char *maze_s;
#ifdef MAZEFILE
#undef MAZEFILE
#endif
#define MAZEFILE 0
#else
static const char MAZEFILE_str[] = MAZEFILE;
#ifdef MAZEFILE
#undef MAZEFILE
#endif
#define MAZEFILE MAZEFILE_str
#define maze_s MAZEFILE
#endif

my_ushort **inside_walls = NULL;

static chtype
pen[NPENS];

/* color palette for USE_PALETTE and HTML snapshots */
static const short
pen_pal[16][3] =
{
/*  {  red,green, blue } */
    {    0,    0,    0 }, /* 0: black */
    {    0,    0,  867 }, /* 1: blue */
    {    0,  718,    0 }, /* 2: green (peach stem) */
    {    0,  867,  867 }, /* 3: cyan */
    {  867,  589,  277 }, /* 4: brown (apple/cherry stem) */
    { 1000,  128,  589 }, /* 5: magenta (mypal trim?) */
    { 1000,  718,  277 }, /* 6: salmon (clyde, peach, PUSH START BUTTON) */
    { 1000,  718,  589 }, /* 7: light yellow (dot, blue face, BONUS MYMAN FOR x Pts) */
    {  400,  400,  400 }, /* 8: grey */
    {  128,  128,  867 }, /* 9: light blue (wall, blue ghost) */
    {  589, 1000,    0 }, /* A: light green (mypal?, super dot?) */
    {    0, 1000,  867 }, /* B: light cyan (inky, key top, 1 PLAYER ONLY, ghost scores) */
    { 1000,    0,    0 }, /* C: light red (blinky, apple/cherry, GAME  OVER) */
    {  980,  701,  847 }, /* D: pink (pinky, door, NEW MAN - X, fruit scores) */
    { 1000, 1000,    0 }, /* E: yellow (myman, ship, READY!) */
    {  867,  867,  867 }  /* F: light grey (text, eye, apple/cherry shine, key, bell) */
};

static int use_false_color = 0;

static int use_vga_palette = 0;

static int use_ansi_palette = 0;

static int use_revbg = 0;

static int use_altcolor = 0;

#define PEN_PAL(i,c) \
    ((use_vga_palette || use_ansi_palette) ? \
     (1000L * (long) (unsigned) (my_uchar) ( \
         (use_vga_palette && ((i) == 6)) ? \
         ("\x7e\x3f\x00")[c] : \
         ("\x00\x3f\x7e\xbd")[ \
             2 * (!!((4 >> (c)) & (i))) \
             + \
             (!!((i) & 8))]) / 0xbd) : \
     pen_pal[(i)][(c)])

#define MILLE_TO_LINEAR_SCALE(n,s) \
    ((short) (((long) (n)) * ((long) (s)) / 1000L))

/* covert pen c to attributes for the native curses implementation */
static chtype
pen_to_attr(int c)
{
    chtype attrs = 0;

    if ((c >= 0) && (c < NPENS))
    {
        if (use_color && !c)
        {
            c = 7;
        }
        if (use_color && ((c / 16) != (c % 16)))
        {
            attrs = pen[c];
        }
        else if (USE_ATTR)
        {
            c = pen_to_mono_pen(c);
            if (c == MY_PEN__REVDIM) {
#ifdef MY_A_REVERSE
                attrs |= MY_A_REVERSE;
#endif
#ifdef MY_A_DIM
                if (use_dim_and_bright) {
                    attrs |= MY_A_DIM;
                }
#endif
            }
            if (c == MY_PEN__DIM) {
#ifdef MY_A_DIM
                if (use_dim_and_bright) {
                    attrs |= MY_A_DIM;
                }
#endif
            }
            if (c == MY_PEN__STANDOUT) {
#ifdef MY_A_STANDOUT
                attrs |= MY_A_STANDOUT;
#else
#ifdef MY_A_REVERSE
                attrs |= MY_A_REVERSE;
#endif
#endif
            }
            if (c & MY_PEN___SPECIAL) {
                c = 0;
            }
#ifdef MY_A_REVERSE
            if ((c & MY_PEN_REVERSE) == MY_PEN_REVERSE) {
                attrs |= MY_A_REVERSE;
            }
#endif
#ifdef MY_A_BOLD
            if (((c & MY_PEN_BOLD) == MY_PEN_BOLD) && use_dim_and_bright) {
                attrs |= MY_A_BOLD;
            }
#endif
#ifdef MY_A_UNDERLINE
            if (((c & MY_PEN_UNDERLINE) == MY_PEN_UNDERLINE) && use_underline) {
                attrs |= MY_A_UNDERLINE;
            }
#endif
        }
    }
    return attrs;
}

#if USE_COLOR

/* FIXME: COLORS and COLOR_PAIRS used to be macros, but in modern
 * ncurses they are externs. */
#if 0

#ifndef COLORS
#define COLORS 8
#endif

#ifndef COLOR_PAIRS
#define COLOR_PAIRS ((COLORS) * (COLORS))
#endif

#endif /* 0 */

static int swap_rb = 0;

#define trans_color(i) (swap_rb \
    ? real_trans_color((((i) & 1) ? 4 : 0) | (((i) & 4) ? 1 : 0) | ((i) & ~(1 | 4))) \
    : real_trans_color(i))

#define real_trans_color(i) \
(((i) == 0) ? COLOR_BLACK \
: ((i) == 1) ? COLOR_BLUE \
: ((i) == 2) ? COLOR_GREEN \
: ((i) == 3) ? COLOR_CYAN \
: ((i) == 4) ? COLOR_RED \
: ((i) == 5) ? COLOR_MAGENTA \
: ((i) == 6) ? COLOR_YELLOW \
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

static short *old_pair_fg = NULL, *old_pair_bg = NULL;

#ifndef MY_INIT_PAIR_RET
#define MY_INIT_PAIR_RET
#endif

#define my_init_pair(x,y,z) init_pair(x,y,z) MY_INIT_PAIR_RET

#ifdef my_init_pair
#undef my_init_pair
#endif

static chtype my_color_pair(short, short *);

static int
my_init_pair(short pair, short f, short b)
{
    static short pairs[256][2];
    short pair2;

    my_color_pair(pair, &pair);
    if (pair == 0)
    {
        return ERR;
    }
    for (pair2 = 1; (pair2 < pair) && (pair2 < 256); pair2++)
    {
        if ((pairs[pair2][0] == f) && pairs[pair2][1] == b)
        {
#if 0
            fprintf(stderr, "!!! pair %hd == pair %hd\n", pair, pair2); fflush(stderr);
#endif
            break;
        }
    }
    if (pair < 256)
    {
        pairs[pair][0] = f;
        pairs[pair][1] = b;
    }
    return init_pair(pair, f, b) MY_INIT_PAIR_RET;
}

static int
my_usable_color_pair(int i)
{
    chtype pair;

    pair = COLOR_PAIR(i);
#ifdef A_BOLD
    if (A_BOLD && ((pair & (A_BOLD)) == A_BOLD))
    {
        return 0;
    }
#endif /* defined(A_BOLD) */
#ifdef A_REVERSE
    if (A_REVERSE && ((pair & (A_REVERSE)) == A_REVERSE))
    {
        return 0;
    }
#endif /* defined(A_REVERSE) */
#ifdef A_STANDOUT
    if (A_STANDOUT && ((pair & (A_STANDOUT)) == A_STANDOUT))
    {
        return 0;
    }
#endif /* defined(A_STANDOUT) */
#ifdef A_UNDERLINE
    if (A_UNDERLINE && ((pair & (A_UNDERLINE)) == A_UNDERLINE))
    {
        return 0;
    }
#endif /* defined(A_UNDERLINE) */
#ifdef A_ALTCHARSET
    if (A_ALTCHARSET && ((pair & (A_ALTCHARSET)) == A_ALTCHARSET))
    {
        return 0;
    }
#endif /* defined(A_ALTCHARSET) */
#ifdef A_BLINK
    if (A_BLINK && ((pair & (A_BLINK)) == A_BLINK))
    {
        return 0;
    }
#endif /* defined(A_BLINK) */
#ifdef A_DIM
    if (A_DIM && ((pair & (A_DIM)) == A_DIM))
    {
        return 0;
    }
#endif /* defined(A_DIM) */
#ifdef A_PROTECT
    if (A_PROTECT && ((pair & (A_PROTECT)) == A_PROTECT))
    {
        return 0;
    }
#endif /* defined(A_PROTECT) */
#ifdef A_INVIS
    if (A_INVIS && ((pair & (A_INVIS)) == A_INVIS))
    {
        return 0;
    }
#endif /* defined(A_INVIS) */
    return 1;
}

static int
my_color_pairs(void)
{
    int pairs;
    int i;

    pairs = 0;
    /* slcurses defines some color pairs and other attributes to be
     * equal; this only counts usable pairs (i.e. those not
     * overlapping with the basic attributes) */
    for (i = 0; i < terminal_color_pairs; i++)
    {
        if (! my_usable_color_pair(i))
        {
            continue;
        }
        pairs ++;
    }
    return pairs;
}

static chtype
my_color_pair(short i, short *pair_number)
{
    short j;

    for (j = 0; (j >= 0) && (j < terminal_color_pairs); j ++)
    {
        if (! my_usable_color_pair(j))
        {
            continue;
        }
        if (! i)
        {
            if (pair_number)
            {
                *pair_number = j;
            }
            return COLOR_PAIR(j);
        }
        if (i > 0)
        {
            i --;
        }
    }
    if (pair_number)
    {
        *pair_number = 0;
    }
    return 0;
}

#endif /* USE_COLOR */

static void
destroy_pen(void)
{
#if USE_COLOR
    int i;

    if (use_false_color)
    {
        return;
    }
#if USE_PALETTE
    if ((! use_fixedpal) && can_change_color() && (terminal_colors >= 16)) {
        for (i = 0; i < 16; i ++)
            init_color(i, old_pal[i][swap_rb ? 2 : 0], old_pal[i][1], old_pal[i][swap_rb ? 0 : 2]);
    }
#endif
    if (old_pair_fg && old_pair_bg)
    {
        for (i = 0; i < terminal_color_pairs; i ++)
            if (old_pair_fg[i] != old_pair_bg[i]) {
                init_pair(i, old_pair_fg[i], old_pair_bg[i]);
            }
        free((void *) old_pair_fg);
        free((void *) old_pair_bg);
        old_pair_fg = NULL;
        old_pair_bg = NULL;
    }
#endif
}

static void
init_pen(void)
{
    int i;

#if USE_COLOR
    if (terminal_colors == -1)
    {
        terminal_colors = COLORS;
    }
    if (terminal_color_pairs == -1)
    {
        terminal_color_pairs = COLOR_PAIRS;
    }
#endif
    for (i = 0; i < NPENS; i ++)
    {
        chtype attrs = 0;
        int p;

        p = pen_to_mono_pen(i);
        attrs = pen_to_attr(p);
        pen[i] = attrs;
    }
    if (use_false_color)
    {
        return;
    }
#if USE_COLOR
    {
        int pairs;
        int nextpair;
        my_uchar pair_allocated[32];

        pairs = my_color_pairs();
        memset((void *) pair_allocated, 0, sizeof(pair_allocated));
        old_pair_fg = (short *) malloc(terminal_color_pairs * sizeof(short));
        if (! old_pair_fg) {
            perror("malloc");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        memset((void *) old_pair_fg, 0, terminal_color_pairs * sizeof(short));
        old_pair_bg = (short *) malloc(terminal_color_pairs * sizeof(short));
        if (! old_pair_bg) {
            perror("malloc");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        memset((void *) old_pair_bg, 0, terminal_color_pairs * sizeof(short));
        for (i = 0; i < terminal_color_pairs; i ++)
        {
            old_pair_fg[i] = old_pair_bg[i] = COLOR_BLACK;
            pair_content(i, &(old_pair_fg[i]), &(old_pair_bg[i]));
        }
#ifdef MY_A_BOLD
#define trans_pen_bright ((PEN_BRIGHT == MY_A_BOLD) ? 8 : (((PEN_BRIGHT > 0) && (PEN_BRIGHT < 16)) ? PEN_BRIGHT : 8))
#define trans_attr_bright (((PEN_BRIGHT == MY_A_BOLD) || (PEN_BRIGHT > 127)) ? PEN_BRIGHT : MY_A_BOLD)
#else
#define trans_pen_bright (((PEN_BRIGHT > 0) && (PEN_BRIGHT < 16)) ? PEN_BRIGHT : 8)
#define trans_attr_bright ((PEN_BRIGHT > 127) ? PEN_BRIGHT : 0)
#endif
#if USE_PALETTE
        if ((! use_fixedpal) && can_change_color() && (terminal_colors >= 16) && (pairs >= 16))
        {
            short trans_dynamic_color[16];
            short trans_dynamic_color_reverse[16];

            /* attempt to use similar colors for the dynamic palette in case
             * setting the dynamic palette does not actually work */
            for (i = 0; i < 16; i ++)
            {
                trans_dynamic_color[i] = (short) ERR;
                trans_dynamic_color_reverse[i] = (short) ERR;
            }
            for (i = 0; i < 8; i ++)
            {
                if ((trans_color(i) >= 0)
                    &&
                    (trans_color(i) < 16)
                    &&
                    (trans_color(i) != (short) ERR)
                    &&
                    (i != (short) ERR)
                    &&
                    (trans_dynamic_color_reverse[trans_color(i)] == (short) ERR))
                {
                    trans_dynamic_color[i] = trans_color(i);
                    trans_dynamic_color_reverse[trans_color(i)] = i;
                    if (((trans_color(i) | trans_pen_bright) > 0)
                        &&
                        ((trans_color(i) | trans_pen_bright) < 16)
                        &&
                        ((trans_color(i) | trans_pen_bright) != (short) ERR)
                        &&
                        ((i | 8) != (short) ERR)
                        &&
                        (trans_dynamic_color_reverse[trans_color(i) | trans_pen_bright] == (short) ERR))
                    {
                        trans_dynamic_color[i | 8] = trans_color(i) | trans_pen_bright;
                        trans_dynamic_color_reverse[trans_color(i) | trans_pen_bright] = i | 8;
                    }
                }
            }
            for (i = 0; i < 16; i ++)
            {
                if ((trans_dynamic_color[i] == (short) ERR)
                    ||
                    (trans_dynamic_color_reverse[i] == (short) ERR))
                {
                    break;
                }
            }
            if (i != 16)
            {
                for (i = 0; i < 16; i ++)
                {
                    trans_dynamic_color[i] = i;
                    trans_dynamic_color_reverse[i] = i;
                }
            }
            for (i = 0; i < 16; i ++)
                color_content(i, old_pal[i], old_pal[i] + 1, old_pal[i] + 2);
            for (i = 0; i < 16; i ++) {
                init_color(trans_dynamic_color[i],
                           MILLE_TO_LINEAR_SCALE(PEN_PAL(i, swap_rb ? 2 : 0), terminal_color_scale),
                           MILLE_TO_LINEAR_SCALE(PEN_PAL(i, 1), terminal_color_scale),
                           MILLE_TO_LINEAR_SCALE(PEN_PAL(i, swap_rb ? 0 : 2), terminal_color_scale));
                if ((! i) || (my_init_pair(i, trans_dynamic_color[i], trans_dynamic_color[0]) == ERR))
                {
                    pen[i] = my_color_pair(0, NULL);
                }
                else
                {
                    pen[i] = my_color_pair(i, NULL);
                    pair_allocated[i / 8] =
                        ((unsigned) (my_uchar) pair_allocated[i / 8])
                        |
                        (1U << (i % 8));
                }
            }
            nextpair = 16;
            for (i = 16; i < 256; i ++)
            {
                if ((i % 16) == (i / 16)) continue;
                pen[i] = pen[i % 16];
#ifdef A_REVERSE
                pen[i] = pen[i / 16] ^ A_REVERSE;
                if (use_revbg) continue;
                if (((i / 16) > (i % 16)) && ! (pen[(i % 16) * 16 + (i / 16)] & A_REVERSE))
                {
                    pen[i] = pen[(i % 16) * 16 + (i / 16)] | A_REVERSE;
                }
                else if (i % 16)
#endif
                {
                    if ((nextpair < pairs) && ((i % 16) != (i / 16)))
                    {
                        if (my_init_pair(nextpair, trans_dynamic_color[i % 16], trans_dynamic_color[i / 16]) != ERR)
                        {
                            pen[i] = my_color_pair(nextpair, NULL);
                            pair_allocated[i / 8] =
                                ((unsigned) (my_uchar) pair_allocated[i / 8])
                                |
                                (1U << (i % 8));
                            nextpair++;
                        }
                    }
                }
            }
        }
        else
#endif
        {
            for (i = 0; i < 8; i ++)
            {
                if (i &&
                    (i < pairs) &&
                    (my_init_pair(i, trans_color(i), COLOR_BLACK) != ERR))
                {
                    pen[i] = my_color_pair(i, NULL);
                    pair_allocated[i / 8] =
                        ((unsigned) (my_uchar) pair_allocated[i / 8])
                        |
                        (1U << (i % 8));
                    pen[8 + i] = my_color_pair(i, NULL) |
                        (use_dim_and_bright ? trans_attr_bright : 0);
                    if (i == 7) {
                        pen[8] = pen[7] |
                            (use_dim_and_bright
                             ? PEN_DIM
                             : 0);
                    }
                }
                else
                {
                    pen[i] = my_color_pair(0, NULL);
                    if (i) {
                        pen[8 + i] = pen[i] |
                            (use_dim_and_bright
                             ? trans_attr_bright
                             : 0);
                    }
                }
            }
            nextpair = 8;
            for (i = 16; i < 256; i ++)
            {
                int fgansi, bgansi, fgansi8, bgansi8;

                fgansi = i % 16;
                bgansi = i / 16;
                fgansi8 = (fgansi == 8) ? 7 : (fgansi & 7);
                bgansi8 = (bgansi == 8) ? 7 : (bgansi & 7);
                if (fgansi8 == bgansi8) {
                    fgansi8 = bgansi8 ? 0 : 7;
                }
                if (fgansi == bgansi) continue;
                if (! use_dim_and_bright)
                {
                    if ((fgansi > 7) || (bgansi > 7))
                    {
                        pen[i] = pen[16 * bgansi8 + fgansi8];
                        continue;
                    }
                }
                else
                {
#ifndef A_REVERSE
                    if (bgansi > 7)
                    {
                        pen[i] = pen[16 * bgansi8 + fgansi];
                        continue;
                    }
#endif
                    if ((bgansi <= 7) && (fgansi > 7) && (fgansi8 != bgansi))
                    {
#ifdef A_REVERSE
                        if (! (pen[16 * bgansi + fgansi8] & A_REVERSE))
#endif
                        {
                            pen[i] = pen[16 * bgansi + ((fgansi == 8) ? 7 : fgansi8)] | ((fgansi == 8) ? PEN_DIM : trans_attr_bright);
                            continue;
                        }
                    }
#ifdef A_REVERSE
                    if ((fgansi <= 7) && (bgansi > 7) && (fgansi != bgansi8))
                    {
                        if (! (pen[16 * fgansi + bgansi8] & A_REVERSE))
                        {
                            pen[i] = pen[16 * fgansi + ((bgansi == 8) ? 7 : bgansi8)] | ((bgansi == 8) ? PEN_DIM : trans_attr_bright) | A_REVERSE;
                            continue;
                        }
                    }
                    if ((fgansi > 7) && (bgansi > 7))
                    {
                        pen[i] = pen[16 * bgansi + ((fgansi == 8) ? 7 : fgansi8)];
                        continue;
                    }
                    if ((fgansi8 == bgansi8) && (bgansi > 7) && (fgansi != bgansi))
                    {
                        if (! (pen[16 * fgansi + bgansi] & A_REVERSE))
                        {
                            pen[i] = pen[16 * fgansi + bgansi] | A_REVERSE;
                            continue;
                        }
                    }
#endif
                    if ((fgansi > 7) && (bgansi > 7))
                    {
                        pen[i] = pen[16 * bgansi8 + fgansi];
                        continue;
                    }
                }
                pen[i] = pen[fgansi];
#ifdef A_REVERSE
                pen[i] = pen[bgansi] | A_REVERSE;
                if (use_revbg) continue;
#endif
                if (fgansi && (fgansi != bgansi))
                {
                    if (nextpair < pairs)
                    {
#ifdef A_REVERSE
                        if ((bgansi > 7) && (my_init_pair(nextpair, trans_color(bgansi & 7), trans_color(fgansi & 7)) != ERR))
                        {
                            pen[i] = my_color_pair(nextpair, NULL) | A_REVERSE;
                            pair_allocated[i / 8] =
                                ((unsigned) (my_uchar) pair_allocated[i / 8])
                                |
                                (1U << (i % 8));
                            if (bgansi > 7)
                            {
                                pen[i] |= (use_dim_and_bright ? ((bgansi == 8) ? PEN_DIM : trans_attr_bright) : 0);
                            }
                            nextpair ++;
                            continue;
                        }
                        else
#endif
                        {
                            if (my_init_pair(nextpair, trans_color(fgansi & 7), trans_color(bgansi & 7)) != ERR)
                            {
                                pen[i] = my_color_pair(nextpair, NULL);
                                pair_allocated[i / 8] =
                                    ((unsigned) (my_uchar) pair_allocated[i / 8])
                                    |
                                    (1U << (i % 8));
                                if (fgansi > 7)
                                {
                                    pen[i] |= (use_dim_and_bright ? ((fgansi == 8) ? PEN_DIM : trans_attr_bright) : 0);
                                }
                                nextpair ++;
                                continue;
                            }
                        }
                    }
                }
            }
            if ((terminal_colors == 16)
                &&
                use_altcolor
                &&
                (nextpair < pairs))
            {
                int workable;

                workable = 1;
                for (i = 0; i < 8; i ++)
                {
                    if ((trans_color(i) < 0) || (trans_color(i) > 8))
                    {
                        workable = 0;
                        break;
                    }
                }
                if (workable)
                {
                    /* NOTE: we assume colors 8-15 are the bright versions
                     * of 0-7 in this case */
                    for (i = 0; i < 256; i ++)
                    {
                        int fgansi, bgansi;

                        fgansi = i % 16;
                        bgansi = i / 16;
                        if (fgansi == bgansi) continue;
                        if ((fgansi > 7) || (bgansi > 7))
                        {
                            if (bgansi == fgansi)
                            {
                                pen[i] = pen[16 * bgansi];
                                continue;
                            }
#ifdef A_REVERSE
                            if ((bgansi > fgansi)
                                &&
                                (! (pen[16 * fgansi + bgansi] & A_REVERSE)))
                            {
                                pen[i] = pen[16 * fgansi + bgansi] ^ A_REVERSE;
                                continue;
                            }
#endif
                            if ((nextpair < pairs)
                                &&
                                (my_init_pair(nextpair, trans_color(fgansi & 7) | (fgansi & 8), trans_color(bgansi & 7) | (bgansi & 8)) != ERR))
                            {
                                pen[i] = my_color_pair(nextpair, NULL);
                                pair_allocated[i / 8] =
                                    ((unsigned) (my_uchar) pair_allocated[i / 8])
                                    |
                                    (1U << (i % 8));
                                if (fgansi > 7)
                                {
                                    pen[i] |= (use_dim_and_bright ? ((fgansi == 8) ? PEN_DIM : trans_attr_bright) : 0);
#ifdef A_REVERSE
                                    if (bgansi == 0)
                                    {
                                        int j;

                                        for (j = 0; j < 16; j ++)
                                        {
                                            pen[16 * fgansi + j] = pen[i] | A_REVERSE;
                                        }
                                    }
#endif
                                }
                                nextpair ++;
                                continue;
                            }
                        }
                    }
                }
            }
            /* NOTE: we assume the default xterm-256color/xterm-88color
             * palette in these cases */
            if (((terminal_colors == 88)
                 ||
                 (terminal_colors == 256))
                &&
                (pairs >= 16))
            {
                nextpair = 1;
                for (i = 0; i < 256; i ++)
                {
                    int fg, bg;
                    int rgbscale;
                    int grayscale;
                    short fg_rgb[3], bg_rgb[3];
                    int fgansi, bgansi;

                    fgansi = i % 16;
                    bgansi = i / 16;
                    if (fgansi == bgansi) continue;
                    if (use_revbg && fgansi && bgansi)
                    {
                        pen[i] = pen[16 * bgansi];
                        continue;
                    }
#ifdef A_REVERSE
                    if ((pairs < 256) || use_revbg)
                    {
                        if (((fgansi < bgansi) || (use_revbg && bgansi)) && (! (pen[(16 * fgansi) + bgansi] & A_REVERSE)))
                        {
                            pen[i] = pen[(16 * fgansi) + bgansi] | A_REVERSE;
                            continue;
                        }
                    }
#endif
                    if (! i)
                    {
                        pen[i] = my_color_pair(0, NULL);
                        continue;
                    }
                    else if ((nextpair > pairs) || ((i >= 16) && use_revbg))
                    {
                        pen[i] = pen[fgansi];
#ifdef A_REVERSE
                        if (! (pen[bgansi] & A_REVERSE))
                        {
                            pen[i] = pen[bgansi] | A_REVERSE;
                        }
#endif
                        continue;
                    }
                    rgbscale = (terminal_colors == 256) ? 6 : 4;
                    grayscale = (terminal_colors == 256) ? 26 : 10;
                    fg_rgb[0] = mille_to_scale(PEN_PAL(fgansi, 0), rgbscale);
                    fg_rgb[1] = mille_to_scale(PEN_PAL(fgansi, 1), rgbscale);
                    fg_rgb[2] = mille_to_scale(PEN_PAL(fgansi, 2), rgbscale);
                    if (fg_rgb[0]
                        &&
                        (fg_rgb[0] < (rgbscale - 1))
                        &&
                        (fg_rgb[0]
                         ==
                         fg_rgb[1])
                        &&
                        (fg_rgb[0]
                         ==
                         fg_rgb[2])
                        &&
                        ((((long) fg_rgb[0]) * (grayscale - 1) / (rgbscale - 1)) != (long) mille_to_scale(PEN_PAL(fgansi, 0), grayscale)))
                    {
                        fg = 16 + rgbscale * rgbscale * rgbscale + mille_to_scale(299L * PEN_PAL(fgansi, 0) / 1000 + 587L * PEN_PAL(fgansi, 1) / 1000 + 114L * PEN_PAL(fgansi, 2) / 1000, grayscale) - 1;
                    }
                    else
                    {
                        fg = 16 + rgbscale * (rgbscale * fg_rgb[0] + fg_rgb[1]) + fg_rgb[2];
                    }
                    bg_rgb[0] = mille_to_scale(PEN_PAL(bgansi, 0), rgbscale);
                    bg_rgb[1] = mille_to_scale(PEN_PAL(bgansi, 1), rgbscale);
                    bg_rgb[2] = mille_to_scale(PEN_PAL(bgansi, 2), rgbscale);
                    if (bg_rgb[0]
                        &&
                        (bg_rgb[0] < (rgbscale - 1))
                        &&
                        (bg_rgb[0]
                         ==
                         bg_rgb[1])
                        &&
                        (bg_rgb[0]
                         ==
                         bg_rgb[2])
                        &&
                        ((((long) bg_rgb[0]) * (grayscale - 1) / (rgbscale - 1)) != (long) mille_to_scale(PEN_PAL(i/ 16, 0), grayscale)))
                    {
                        bg = 16 + rgbscale * rgbscale * rgbscale + mille_to_scale(299L * PEN_PAL(bgansi, 0) / 1000 + 587L * PEN_PAL(bgansi, 1) / 1000 + 114L * PEN_PAL(bgansi, 2) / 1000, grayscale) - 1;
                    }
                    else
                    {
                        bg = 16 + rgbscale * (rgbscale * bg_rgb[0] + bg_rgb[1]) + bg_rgb[2];
                    }
                    if (my_init_pair(nextpair, fg, bg) != ERR)
                    {
                        pen[i] = my_color_pair(nextpair, NULL);
                        pair_allocated[i / 8] =
                            ((unsigned) (my_uchar) pair_allocated[i / 8])
                            |
                            (1U << (i % 8));
                        nextpair ++;
                    }
                }
            }
        }
        /* HACK: throw away the black pen, use white instead */
        pen[0] = pen[7];
    }
#endif /* USE_COLOR */
}

int
pen_and_attr_to_pen(int p, chtype attrs)
{
#ifdef MY_A_REVERSE
    if ((attrs & MY_A_REVERSE) == MY_A_REVERSE)
    {
        if ((p / 16) == (p % 16))
        {
            p ^= MY_PEN_REVERSE;
        }
        else
        {
            p = (p / 16) + 16 * (p % 16);
        }
    }
#endif
    return p;
}

/* wrappers around some curses functions to allow raw CP437-mode and
 * snapshots; note that these wrappers support only a small subset of
 * the corresponding curses behavior */

FILE *snapshot = NULL;
FILE *snapshot_txt = NULL;
static int snapshot_x = 0;
static int snapshot_y = 0;
static int snapshot_pen = MY_PEN_NORMAL;
static int snapshot_active_pen = MY_PEN_NORMAL;
static int snapshot_use_color = 0;

/* start taking a snapshot */
void snapshot_start(void)
{
    char buf[128];
    char buf_txt[128];
    my_uint idx;

    if ((! snapshot)
        &&
        (! snapshot_txt))
    {
        /* try to find a free slot */
        for (idx = 0; idx <= 9999; idx ++)
        {
            sprintf(buf,
                    "snap%4.4u%s",
                    idx, HTM_SUFFIX);
            sprintf(buf_txt,
                    "snap%4.4u%s",
                    idx, TXT_SUFFIX);
            if (access(buf, F_OK) && access(buf_txt, F_OK))
            {
                break;
            }
        }
        snapshot = fopen(buf, "wb");
        snapshot_txt = fopen(buf_txt, "wb");
        snapshot_use_color = use_color;
    }
}

/* simulate a subset of curses attributes in HTML; note that this
 * generates presentational markup (<font color="...">, <u>, <b>,
 * etc.) which is considered deprecated in modern HTML; however there
 * is really no acceptable alternative since this markup needs to look
 * colorful even in older browsers */
static void
snapshot_choose_active_pen(int p)
{
    if (! snapshot)
    {
        return;
    }
    if (snapshot_active_pen != p)
    {
        if ((snapshot_active_pen % 16) != (snapshot_active_pen / 16))
        {
            if (snapshot_use_color && (snapshot_active_pen >= 0) && (snapshot_active_pen < NPENS))
            {
                fprintf(snapshot,
                        "</font>");
            }
        }
        else
        {
            if ((snapshot_active_pen == MY_PEN__STANDOUT)
                ||
                (snapshot_active_pen == MY_PEN__DIM)
                ||
                (snapshot_active_pen == MY_PEN__REVDIM))
            {
                fprintf(snapshot,
                        "</span>");
            }
            if (! (snapshot_active_pen & MY_PEN___SPECIAL))
            {
                if (snapshot_active_pen & MY_PEN_BOLD)
                {
                    fprintf(snapshot,
                            "</b>");
                }
                if (snapshot_active_pen & MY_PEN_UNDERLINE)
                {
                    fprintf(snapshot,
                            "</u>");
                }
                if (snapshot_active_pen & MY_PEN_REVERSE)
                {
                    fprintf(snapshot,
                            "</span>");
                }
            }
        }
        if ((p % 16) != (p / 16))
        {
            if (snapshot_use_color && (p >= 0) && (p < NPENS))
            {
                my_ulong r, g, b;
                int p1;

                p1 = (p / 16) ? (p / 16) : (p % 16);
                r = (255L * PEN_PAL(p1 % 16, 0)) / 1000;
                g = (255L * PEN_PAL(p1 % 16, 1)) / 1000;
                b = (255L * PEN_PAL(p1 % 16, 2)) / 1000;
                fprintf(snapshot,
                        "<font color=\"#%2.2lX%2.2lX%2.2lX\" class=\"",
                        r & 0xffUL, g & 0xffUL, b & 0xffUL);
                fprintf(snapshot,
                        "f%x",
                        p % 16);
                if (p / 16)
                {
                    fprintf(snapshot,
                            " b%x",
                            p / 16);
                }
                fprintf(snapshot,
                        "\">");
            }
        }
        else
        {
            if (p & MY_PEN___SPECIAL)
            {
                if (p == MY_PEN__STANDOUT)
                {
                    fprintf(snapshot,
                            "<span class=\"so\">");
                }
                else if (p == MY_PEN__REVDIM)
                {
                    fprintf(snapshot,
                            "<span class=\"revdim\">");
                }
                else if (p == MY_PEN__DIM)
                {
                    fprintf(snapshot,
                            "<span class=\"dim\">");
                }
            }
            else
            {
                if (p & MY_PEN_REVERSE)
                {
                    fprintf(snapshot,
                            "<span class=\"rev\">");
                }
                if (p & MY_PEN_UNDERLINE)
                {
                    fprintf(snapshot,
                            "<u>");
                }
                if (p & MY_PEN_BOLD)
                {
                    fprintf(snapshot,
                            "<b>");
                }
            }
        }
        snapshot_active_pen = p;
    }
    fflush(snapshot);
}

/* non-outputting version of snapshot_choose_active_pen */
static void
snapshot_choose_pen(int p)
{
    snapshot_pen = p;
}

static int last_valid_line = 0;
static int last_valid_col = -1;

int my_cols(void) {
    return MY_COLS;
}

int my_vlines(void) {
    return (reflect ? MY_COLS : (LINES * (use_bitmap ? 2 : 1)));
}

int my_vcols(void) {
    return (reflect ? (LINES * (use_bitmap ? 2 : 1)) : MY_COLS);
}

int my_rlines(void) {
    return LINES;
}

int my_rcols(void) {
    return COLS;
}

void
my_erase(void)
{
    if (snapshot || snapshot_txt)
    {
        const char *my_locale = "en";
        char *my_locale_dynamic = NULL;
        const char *web_locale;

#ifdef LC_CTYPE
        my_locale = locale_is_broken ? NULL : setlocale(LC_CTYPE, myman_locale);
#endif /* defined(LC_CTYPE) */
        if ((! my_locale) || (! *my_locale))
        {
            my_locale = "en";
        }
        my_locale_dynamic = strdup(my_locale);
        if (! my_locale_dynamic)
        {
            my_locale = "en";
        }
        else
        {
            int i;

            my_locale = my_locale_dynamic;
            for (i = 0; my_locale_dynamic[i]; i ++)
            {
                if (my_locale_dynamic[i] == '_')
                {
                    my_locale_dynamic[i] = '-';
                    continue;
                }
                if ((my_locale_dynamic[i] == '.')
                    ||
                    (my_locale_dynamic[i] == '\"')
                    ||
                    (my_locale_dynamic[i] == '@')
                    ||
                    (my_locale_dynamic[i] < 0x20)
                    ||
                    (my_locale_dynamic[i] > 0x7E))
                {
                    my_locale_dynamic[i] = '\0';
                    break;
                }
            }
            if ((! my_locale_dynamic[0])
                ||
                (! strcmp(my_locale_dynamic, "C"))
                ||
                (! strcmp(my_locale_dynamic, "POSIX")))
            {
                free((void *) my_locale_dynamic);
                my_locale_dynamic = NULL;
                my_locale = "en";
            }
        }
        web_locale = my_locale;
        if (! strcmp(web_locale, "English-United States"))
        {
            web_locale = "en-US";
        }
        snapshot_x = 0;
        snapshot_y = 0;
        snapshot_pen = MY_PEN_NORMAL;
        snapshot_active_pen = MY_PEN_NORMAL;
        if (snapshot)
        {
            int p;
            const char* font_family;

            font_family =
                (CJK_MODE
                 ?
                 "sazanami gothic, kochi gothic, ar pl sew sung, osaka, kai, biaukai, stkaiti, ms gothic, nsimsun, mingliu, fixedsys, courier, monospace"
                 :
                 "menlo, courier new, courier, monaco, fixedsys, lucida sans unicode, freemono, fixed, monospace");
            fprintf(
                snapshot,
                "%s%s%s%s%s%s%s%s<html xmlns=\"%s\" xml:lang=\"%s\" lang=\"%s\">%s%s%s<meta name=\"generator\" content=\"%s %s\" />%s<meta http-equiv=\"Content-type\" content=\"%s\" />%s<title>",
                ((use_acs && use_raw && ! use_raw_ucs)
                 ? "<\?xml version=\"1.0\" encoding=\"CP437\"\?>"
                 : "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>"),
                CRLF,
                "<!DOCTYPE html",
                CRLF,
                "     PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"",
                CRLF,
                "    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">",
                CRLF,
                "http://www.w3.org/1999/xhtml",
                web_locale, web_locale,
                CRLF,
                "<head>",
                CRLF,
                MYMAN, MYMANVERSION,
                CRLF,
                ((use_acs && use_raw && ! use_raw_ucs)
                 ? "text/html; charset=CP437"
                 : "text/html; charset=UTF-8"),
                CRLF);

            fprintf(
                snapshot,
                "MyMan Screenshot [%s %s]",
                MYMAN,
                MYMANVERSION);

            fprintf(
                snapshot,
                "</title>%s<style type=\"text/css\">%sbody{font-family:%s}%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",

                CRLF,
                CRLF,
                font_family,
                CRLF,
                "body{color:white;background:black}",
                CRLF,
                ".rev{color:black;background:white}",
                CRLF,
                ".rev u{color:black;background:blue}",
                CRLF,
                ".rev b{color:black;background:yellow}",
                CRLF,
                ".rev u b{color:black;background:lime}",
                CRLF,
                "u{color:blue;text-decoration:none}",
                CRLF,
                "u b{color:lime}",
                CRLF,
                "b{color:yellow}",
                CRLF,
                ".dim{color:gray}",
                CRLF,
                ".revdim{color:black;background:gray}",
                CRLF,
                ".so{color:black;background:magenta}",
                CRLF);

            for (p = 0; p < 16; p ++)
            {
                my_ulong r, g, b;

                r = (255L * PEN_PAL(p, 0)) / 1000;
                g = (255L * PEN_PAL(p, 1)) / 1000;
                b = (255L * PEN_PAL(p, 2)) / 1000;
                fprintf(snapshot,
                        ".f%x {color: #%2.2lX%2.2lX%2.2lX}%s",

                        p,
                        r & 0xffUL, g & 0xffUL, b & 0xffUL,
                        CRLF);
                fprintf(snapshot,
                        ".b%x {background: #%2.2lX%2.2lX%2.2lX}%s",

                        p,
                        r & 0xffUL, g & 0xffUL, b & 0xffUL,
                        CRLF);
            }

            fprintf(
                snapshot,
                "</style>%s</head>%s<body%s%s><pre><font face=\"%s\">%s",

                CRLF,
                CRLF,
                snapshot_use_color ? " text=\"white\"" : "",
                snapshot_use_color ? " bgcolor=\"black\"" : "",
                font_family,
                CRLF);
            fflush(snapshot);
        }
        if (snapshot_txt)
        {
            fputc_utf8(0xFEFF, snapshot_txt);
            /* Title */
            fprintf(snapshot_txt,
                    "MyMan Screenshot [%s %s]%s",
                    MYMAN,
                    MYMANVERSION,
                    CRLF);
            fflush(snapshot_txt);
        }
        if (my_locale)
        {
#ifdef LC_CTYPE
            setlocale(LC_CTYPE, my_locale);
#endif /* defined(LC_CTYPE) */
            if (my_locale_dynamic)
            {
                free((void *) my_locale_dynamic);
                my_locale_dynamic = NULL;
                my_locale = NULL;
            }
        }
    }

    if (location_is_suspect)
    {
        last_valid_line = 0;
        last_valid_col = -1;
        return;
    }
    last_valid_line = LINES - 1;
    last_valid_col = COLS - 1;
    erase();
}

int
my_clear(void)
{
    int ret;

    /*location_is_suspect = 0;*/
    ret = clear();
    return ret;
}

void
my_clearok(int ok)
{
    clearok(curscr, (ok ? TRUE : FALSE));
}

void
my_showpause(void)
{
    standout();
    mvprintw(my_rlines() / 2, ((my_rcols() - (int) strlen(PAUSE)) & ~(use_fullwidth ? 1 : 0)) / 2, PAUSE);
    standend();
}

int
my_refresh(void)
{
    if (snapshot)
    {
        snapshot_choose_active_pen(MY_PEN_NORMAL);
        fprintf(snapshot,
                "%s</font></pre></body></html>%s",
                CRLF,
                CRLF);
        fflush(snapshot);
        fclose(snapshot);
        snapshot = (FILE *) 0;
    }
    if (snapshot_txt)
    {
        fprintf(snapshot_txt,
                CRLF);
        fflush(snapshot_txt);
        fclose(snapshot_txt);
        snapshot_txt = (FILE *) 0;
    }
    if (location_is_suspect)
    {
        if (((last_valid_col + 1) < COLS)
            ||
            ((last_valid_line + 1) < LINES))
        {
            move((last_valid_line + (last_valid_col + 1) / COLS), (last_valid_col + 1) % COLS);
            clrtobot();
        }
        last_valid_col = COLS - 1;
        last_valid_line = LINES - 1;
    }
    return refresh();
}

void
my_move(int y, int x)
{
    if ((y < 0) || (x < 0) || (y > LINES) || (x > COLS))
    {
        return;
    }
    if ((snapshot || snapshot_txt)
        &&
        ((x != snapshot_x) || (y != snapshot_y)))
    {
        snapshot_choose_active_pen(MY_PEN_NORMAL);
        if ((snapshot || snapshot_txt)
            &&
            (y < snapshot_y))
        {
            if (snapshot)
            {
                fprintf(snapshot, "<!-- cuu%d -->", snapshot_y - y);
                fflush(snapshot);
            }
            snapshot_y = y;
        }
        if (snapshot && (x < snapshot_x) && (y == snapshot_y))
        {
            fprintf(snapshot, "<!-- cub%d -->", snapshot_x - x);
            fflush(snapshot);
        }
        while ((y > snapshot_y) || (x < snapshot_x))
        {
            snapshot_y ++;
            snapshot_x = 0;
            if (snapshot)
            {
                fprintf(snapshot,
                        CRLF);
                fflush(snapshot);
            }
            if (snapshot_txt)
            {
                fprintf(snapshot_txt,
                        CRLF);
                fflush(snapshot_txt);
            }
        }
        while (x > snapshot_x)
        {
            if (snapshot)
            {
                fprintf(snapshot,
                        " ");
                fflush(snapshot);
            }
            if (snapshot_txt)
            {
                fprintf(snapshot_txt,
                        " ");
                fflush(snapshot_txt);
            }
            snapshot_x ++;
        }
    }

    do
    {
        int cur_y, cur_x;

        getyx(stdscr, cur_y, cur_x);
        if (location_is_suspect)
        {
            if (last_valid_col == (COLS - 1))
            {
                last_valid_col = -1;
                last_valid_line ++;
            }
            while (y > last_valid_line)
            {
                move(last_valid_line, last_valid_col + 1);
                clrtoeol();
                last_valid_line ++;
                last_valid_col = -1;
            }
            while ((y == last_valid_line)
                   &&
                   (x > (last_valid_col + 1)))
            {
                move(last_valid_line, ++ last_valid_col);
                addch(' ');
            }
        }
        getyx(stdscr, cur_y, cur_x);
        if ((y != cur_y) || (x != cur_x))
        {
            move(y, x);
        }
    }
    while(0);
}

void my_clearattrs(void)
{
#if USE_ATTR || USE_COLOR
    standend();
#if HAVE_ATTRSET
    attrset(0);
#endif
#endif
}

#if USE_ATTR || USE_COLOR

#if DANGEROUS_ATTRS
static chtype my_attrs = 0;
#endif

chtype my_active_attrs = 0;

static int
my_real_attrset(chtype attrs)
{
    my_active_attrs = attrs;
#if DANGEROUS_ATTRS
    if (attrs)
    {
        int cur_x, cur_y;

        getyx(stdscr, cur_y, cur_x);
        /* classic BSD curses has an annoying bug which causes it to
         * hang if attributes are used in the last writable screen
         * cell */
        if ((cur_x >= (COLS - (CJK_MODE ? 1 : 0) - 2 * (cur_y == (LINES - 1)))))
        {
            return 1;
        }
    }
#endif
#if HAVE_ATTRSET
    attrset(attrs);
#else
    {
#ifdef MY_A_STANDOUT
        if (attrs & MY_A_STANDOUT) standout();
        else standend();
#endif
#if HAVE_SETATTR
        {
#ifdef MY_A_BLINK
#ifdef _BLINK
            if (attrs & MY_A_BLINK) setattr(_BLINK);
            else clrattr(_BLINK);
#endif
#endif
#ifdef MY_A_BOLD
#ifdef _BOLD
            if (attrs & MY_A_BOLD) setattr(_BOLD);
            else clrattr(_BOLD);
#endif
#endif
#ifdef MY_A_REVERSE
#ifdef _REVERSE
            if (attrs & MY_A_REVERSE) setattr(_REVERSE);
            else clrattr(_REVERSE);
#endif
#endif
#ifdef MY_A_UNDERLINE
#ifdef _UNDERLINE
            if (attrs & MY_A_UNDERLINE) setattr(_UNDERLINE);
            else clrattr(_UNDERLINE);
#endif
#endif
        }
#endif
    }
#endif
    return 1;
}

#endif /* USE_ATTR || USE_COLOR */

int
my_choose_pen(int p)
{
    chtype attrs;

    if (use_false_color)
    {
        if ((use_false_color & 2) && ((p / 16) != (p % 16)))
        {
            int rev;

            /* CGA-style three-color palette with XTerm-style
             * attribute mapping:
             * normal = white;
             * bold = cyan;
             * underline = magenta; and
             * background = black. Attribute combinations do
             * not work.
             */
            rev = 0;
            if (p / 16)
            {
                p /= 16;
                rev = 1;
            }
            if ((p == 7) && (use_false_color & 1)) p = 5;
            if (p == 8) p = 7;
            p &= 7;
            p = (p == 5) ? 2 : (((p >> 1) | (p & 1)) % 4);
            if (p == 1) p = MY_PEN_BOLD;
            else if (p == 2) p = MY_PEN_UNDERLINE;
            else p = MY_PEN_NORMAL;
            if (rev && (use_false_color & 4))
            {
                p |= MY_PEN_REVERSE;
            }
        }
        else
        {
            p = pen_to_mono_pen(p);
        }
    }
    snapshot_choose_pen(p);
    attrs = pen_to_attr(p);
#if USE_ATTR || USE_COLOR
    attrs ^= (snapshot || snapshot_txt) ?
#ifdef MY_A_REVERSE
      MY_A_REVERSE
#else
      0
#endif
      : 0;
#if DANGEROUS_ATTRS
    my_attrs = attrs;
#else
    my_real_attrset(attrs);
#endif
#endif /* USE_ATTR || USE_COLOR */
    return 1;
}

/* add cp437 byte inbyte to the HTML snapshot with pen p */
static void
snapshot_addch(short inbyte, int p)
{

#ifdef SNAPSHOT_ADDCH__NARROWC
#undef SNAPSHOT_ADDCH__NARROWC
#endif
#if USE_WIDEC_SUPPORT
#define SNAPSHOT_ADDCH__NARROWC(c) MY_NULL_ACS
#else
#define SNAPSHOT_ADDCH__NARROWC(c) (c)
#endif

    if (snapshot || snapshot_txt)
    {
        my_ulong codepoint;
        int wcw;

        wcw = 1;
        if (use_acs)
        {
            codepoint = ((use_raw && use_raw_ucs) ? uni_cp437 : uni_cp437_halfwidth)[inbyte];
            if (CJK_MODE && use_raw && use_raw_ucs)
            {
                wcw = 2;
            }
            else if (! use_raw)
            {
                switch (inbyte)
                {
                case 201:
                    if (altcharset_cp437[201] != altcharset_cp437[218])
                    {
                        codepoint = (altcharset_cp437[201] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[201])
                            ) ? uni_cp437_halfwidth[201] : (ascii_cp437[201] & MY_A_CHARTEXT);
                        break;
                    }
                case 218:
                    codepoint = (altcharset_cp437[218] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[218])
                        ) ? uni_cp437_halfwidth[218] : (ascii_cp437[218] & MY_A_CHARTEXT);
                    break;
                case 200:
                    if (altcharset_cp437[200] != altcharset_cp437[192])
                    {
                        codepoint = (altcharset_cp437[200] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[200])
                            ) ? uni_cp437_halfwidth[200] : (ascii_cp437[200] & MY_A_CHARTEXT);
                        break;
                    }
                case 192:
                    codepoint = (altcharset_cp437[192] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[192])
                        ) ? uni_cp437_halfwidth[192] : (ascii_cp437[192] & MY_A_CHARTEXT);
                    break;
                case 187:
                    if (altcharset_cp437[187] != altcharset_cp437[191])
                    {
                        codepoint = (altcharset_cp437[187] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[187])
                            ) ? uni_cp437_halfwidth[187] : (ascii_cp437[187] & MY_A_CHARTEXT);
                        break;
                    }
                case 191:
                    codepoint = (altcharset_cp437[191] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[191])
                        ) ? uni_cp437_halfwidth[191] : (ascii_cp437[191] & MY_A_CHARTEXT);
                    break;
                case 188:
                    if (altcharset_cp437[188] != altcharset_cp437[217])
                    {
                        codepoint = (altcharset_cp437[188] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[188])
                            ) ? uni_cp437_halfwidth[188] : (ascii_cp437[188] & MY_A_CHARTEXT);
                        break;
                    }
                case 217:
                    codepoint = (altcharset_cp437[217] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[217])
                        ) ? uni_cp437_halfwidth[217] : (ascii_cp437[217] & MY_A_CHARTEXT);
                    break;
                case 185:
                    if (altcharset_cp437[185] != altcharset_cp437[181])
                    {
                        codepoint = (altcharset_cp437[185] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[185])
                            ) ? uni_cp437_halfwidth[185] : (ascii_cp437[185] & MY_A_CHARTEXT);
                        break;
                    }
                case 181:
                    if (altcharset_cp437[181] != altcharset_cp437[182])
                    {
                        codepoint = (altcharset_cp437[181] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[181])
                            ) ? uni_cp437_halfwidth[181] : (ascii_cp437[181] & MY_A_CHARTEXT);
                        break;
                    }
                case 182:
                    if (altcharset_cp437[182] != altcharset_cp437[180])
                    {
                        codepoint = (altcharset_cp437[182] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[182])
                            ) ? uni_cp437_halfwidth[182] : (ascii_cp437[182] & MY_A_CHARTEXT);
                        break;
                    }
                case 180:
                    codepoint = (altcharset_cp437[180] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[180])
                        ) ? uni_cp437_halfwidth[180] : (ascii_cp437[180] & MY_A_CHARTEXT);
                    break;
                case 204:
                    if (altcharset_cp437[204] != altcharset_cp437[198])
                    {
                        codepoint = (altcharset_cp437[204] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[204])
                            ) ? uni_cp437_halfwidth[204] : (ascii_cp437[204] & MY_A_CHARTEXT);
                        break;
                    }
                case 198:
                    if (altcharset_cp437[198] != altcharset_cp437[199])
                    {
                        codepoint = (altcharset_cp437[198] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[198])
                            ) ? uni_cp437_halfwidth[198] : (ascii_cp437[198] & MY_A_CHARTEXT);
                        break;
                    }
                case 199:
                    if (altcharset_cp437[199] != altcharset_cp437[195])
                    {
                        codepoint = (altcharset_cp437[199] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[199])
                            ) ? uni_cp437_halfwidth[199] : (ascii_cp437[199] & MY_A_CHARTEXT);
                        break;
                    }
                case 195:
                    codepoint = (altcharset_cp437[195] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[195])
                        ) ? uni_cp437_halfwidth[195] : (ascii_cp437[195] & MY_A_CHARTEXT);
                    break;
                case 202:
                    if (altcharset_cp437[202] != altcharset_cp437[207])
                    {
                        codepoint = (altcharset_cp437[202] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[202])
                            ) ? uni_cp437_halfwidth[202] : (ascii_cp437[202] & MY_A_CHARTEXT);
                        break;
                    }
                case 207:
                    if (altcharset_cp437[207] != altcharset_cp437[208])
                    {
                        codepoint = (altcharset_cp437[207] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[207])
                            ) ? uni_cp437_halfwidth[207] : (ascii_cp437[207] & MY_A_CHARTEXT);
                        break;
                    }
                case 208:
                    if (altcharset_cp437[208] != altcharset_cp437[193])
                    {
                        codepoint = (altcharset_cp437[208] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[208])
                            ) ? uni_cp437_halfwidth[208] : (ascii_cp437[208] & MY_A_CHARTEXT);
                        break;
                    }
                case 193:
                    codepoint = (altcharset_cp437[193] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[193])
                        ) ? uni_cp437_halfwidth[193] : (ascii_cp437[193] & MY_A_CHARTEXT);
                    break;
                case 203:
                    if (altcharset_cp437[203] != altcharset_cp437[209])
                    {
                        codepoint = (altcharset_cp437[203] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[203])
                            ) ? uni_cp437_halfwidth[203] : (ascii_cp437[203] & MY_A_CHARTEXT);
                        break;
                    }
                case 209:
                    if (altcharset_cp437[209] != altcharset_cp437[210])
                    {
                        codepoint = (altcharset_cp437[209] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[209])
                            ) ? uni_cp437_halfwidth[209] : (ascii_cp437[209] & MY_A_CHARTEXT);
                        break;
                    }
                case 210:
                    if (altcharset_cp437[210] != altcharset_cp437[194])
                    {
                        codepoint = (altcharset_cp437[210] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[210])
                            ) ? uni_cp437_halfwidth[210] : (ascii_cp437[210] & MY_A_CHARTEXT);
                        break;
                    }
                case 194:
                    codepoint = (altcharset_cp437[194] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[194])
                        ) ? uni_cp437_halfwidth[194] : (ascii_cp437[194] & MY_A_CHARTEXT);
                    break;
                case 213:
                    codepoint = (altcharset_cp437[194] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[194])
                        ) ? uni_cp437_halfwidth[194] : (ascii_cp437[194] & MY_A_CHARTEXT);
                    break;
                case 214:
                    codepoint = (altcharset_cp437[195] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[195])
                        ) ? uni_cp437_halfwidth[195] : (ascii_cp437[195] & MY_A_CHARTEXT);
                    break;
                case 212:
                    codepoint = (altcharset_cp437[193] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[193])
                        ) ? uni_cp437_halfwidth[193] : (ascii_cp437[193] & MY_A_CHARTEXT);
                    break;
                case 211:
                    codepoint = (altcharset_cp437[195] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[195])
                        ) ? uni_cp437_halfwidth[195] : (ascii_cp437[195] & MY_A_CHARTEXT);
                    break;
                case 184:
                    codepoint = (altcharset_cp437[194] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[194])
                        ) ? uni_cp437_halfwidth[194] : (ascii_cp437[194] & MY_A_CHARTEXT);
                    break;
                case 183:
                    codepoint = (altcharset_cp437[180] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[180])
                        ) ? uni_cp437_halfwidth[180] : (ascii_cp437[180] & MY_A_CHARTEXT);
                    break;
                case 190:
                    codepoint = (altcharset_cp437[193] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[193])
                        ) ? uni_cp437_halfwidth[193] : (ascii_cp437[193] & MY_A_CHARTEXT);
                    break;
                case 189:
                    codepoint = (altcharset_cp437[180] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[180])
                        ) ? uni_cp437_halfwidth[180] : (ascii_cp437[180] & MY_A_CHARTEXT);
                    break;
                case 205:
                    if (altcharset_cp437[205] != altcharset_cp437[196])
                    {
                        codepoint = (altcharset_cp437[205] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[205])
                            ) ? uni_cp437_halfwidth[205] : (ascii_cp437[205] & MY_A_CHARTEXT);
                        break;
                    }
                case 196:
                    codepoint = (altcharset_cp437[196] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[196])
                        ) ? uni_cp437_halfwidth[196] : (ascii_cp437[196] & MY_A_CHARTEXT);
                    break;
                case 186:
                    if (altcharset_cp437[186] != altcharset_cp437[179])
                    {
                        codepoint = (altcharset_cp437[186] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[186])
                            ) ? uni_cp437_halfwidth[186] : (ascii_cp437[186] & MY_A_CHARTEXT);
                        break;
                    }
                case 179:
                    codepoint = (altcharset_cp437[179] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[179])
                        ) ? uni_cp437_halfwidth[179] : (ascii_cp437[179] & MY_A_CHARTEXT);
                    break;
                case 206:
                    if (altcharset_cp437[206] != altcharset_cp437[215])
                    {
                        codepoint = (altcharset_cp437[206] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[206])
                            ) ? uni_cp437_halfwidth[206] : (ascii_cp437[206] & MY_A_CHARTEXT);
                        break;
                    }
                case 215:
                    if (altcharset_cp437[215] != altcharset_cp437[216])
                    {
                        codepoint = (altcharset_cp437[215] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[215])
                            ) ? uni_cp437_halfwidth[215] : (ascii_cp437[215] & MY_A_CHARTEXT);
                        break;
                    }
                case 216:
                    if (altcharset_cp437[216] != altcharset_cp437[197])
                    {
                        codepoint = (altcharset_cp437[216] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[216])
                            ) ? uni_cp437_halfwidth[216] : (ascii_cp437[216] & MY_A_CHARTEXT);
                        break;
                    }
                case 197:
                    codepoint = (altcharset_cp437[197] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[197])
                        ) ? uni_cp437_halfwidth[197] : (ascii_cp437[197] & MY_A_CHARTEXT);
                    break;
                case 15:
                    if (altcharset_cp437[15] != altcharset_cp437[176])
                    {
                        codepoint = (altcharset_cp437[15] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[15])
                            ) ? uni_cp437_halfwidth[15] : (ascii_cp437[15] & MY_A_CHARTEXT);
                        break;
                    }
                case 176:
                    if (altcharset_cp437[176] != altcharset_cp437[177])
                    {
                        codepoint = (altcharset_cp437[176] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[176])
                            ) ? uni_cp437_halfwidth[176] : (ascii_cp437[176] & MY_A_CHARTEXT);
                        break;
                    }
                case 177:
                    if (altcharset_cp437[177] != altcharset_cp437[178])
                    {
                        codepoint = (altcharset_cp437[177] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[177])
                            ) ? uni_cp437_halfwidth[177] : (ascii_cp437[177] & MY_A_CHARTEXT);
                        break;
                    }
                case 178:
                    if (altcharset_cp437[178] != altcharset_cp437[10])
                    {
                        codepoint = (altcharset_cp437[178] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[178])
                            ) ? uni_cp437_halfwidth[178] : (ascii_cp437[178] & MY_A_CHARTEXT);
                        break;
                    }
                case 10:
                    if (altcharset_cp437[10] != altcharset_cp437[219])
                    {
                        codepoint = (altcharset_cp437[10] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[10])
                            ) ? uni_cp437_halfwidth[10] : (ascii_cp437[10] & MY_A_CHARTEXT);
                        break;
                    }
                case 219:
                    codepoint = (altcharset_cp437[219] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[219])
                        ) ? uni_cp437_halfwidth[219] : (ascii_cp437[219] & MY_A_CHARTEXT);
                    if ((codepoint == (my_ulong) (ascii_cp437[219] & MY_A_CHARTEXT)) &&
                        (ascii_cp437[219] & ~MY_A_CHARTEXT))
                    {
                        p = pen_and_attr_to_pen(p, ascii_cp437[219] & ~MY_A_CHARTEXT);
                    }
                    break;
                case 27:
                    if (altcharset_cp437[27] != altcharset_cp437[17])
                    {
                        codepoint = (altcharset_cp437[27] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[27])
                            ) ? uni_cp437_halfwidth[27] : (ascii_cp437[27] & MY_A_CHARTEXT);
                        break;
                    }
                case 17:
                    if (altcharset_cp437[17] != altcharset_cp437[174])
                    {
                        codepoint = (altcharset_cp437[17] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[17])
                            ) ? uni_cp437_halfwidth[17] : (ascii_cp437[17] & MY_A_CHARTEXT);
                        break;
                    }
                case 174:
                    if (altcharset_cp437[174] != altcharset_cp437[243])
                    {
                        codepoint = (altcharset_cp437[174] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[174])
                            ) ? uni_cp437_halfwidth[174] : (ascii_cp437[174] & MY_A_CHARTEXT);
                        break;
                    }
                case 243:
                    codepoint = (altcharset_cp437[243] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[243])
                        ) ? uni_cp437_halfwidth[243] : (ascii_cp437[243] & MY_A_CHARTEXT);
                    break;
                case 26:
                    if (altcharset_cp437[26] != altcharset_cp437[16])
                    {
                        codepoint = (altcharset_cp437[26] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[26])
                            ) ? uni_cp437_halfwidth[26] : (ascii_cp437[26] & MY_A_CHARTEXT);
                        break;
                    }
                case 16:
                    if (altcharset_cp437[16] != altcharset_cp437[175])
                    {
                        codepoint = (altcharset_cp437[16] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[16])
                            ) ? uni_cp437_halfwidth[16] : (ascii_cp437[16] & MY_A_CHARTEXT);
                        break;
                    }
                case 175:
                    if (altcharset_cp437[175] != altcharset_cp437[242])
                    {
                        codepoint = (altcharset_cp437[175] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[175])
                            ) ? uni_cp437_halfwidth[175] : (ascii_cp437[175] & MY_A_CHARTEXT);
                        break;
                    }
                case 242:
                    codepoint = (altcharset_cp437[242] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[242])
                        ) ? uni_cp437_halfwidth[242] : (ascii_cp437[242] & MY_A_CHARTEXT);
                    break;
                case 7:
                    if (altcharset_cp437[7] != altcharset_cp437[9])
                    {
                        codepoint = (altcharset_cp437[7] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[7])
                            ) ? uni_cp437_halfwidth[7] : (ascii_cp437[7] & MY_A_CHARTEXT);
                        break;
                    }
                case 9:
                    if (altcharset_cp437[9] != altcharset_cp437[254])
                    {
                        codepoint = (altcharset_cp437[9] !=
                                     SNAPSHOT_ADDCH__NARROWC(ascii_cp437[9])
                            ) ? uni_cp437_halfwidth[9] : (ascii_cp437[9] & MY_A_CHARTEXT);
                        break;
                    }
                case 4:
                case 25:
                case 31:
                case 24:
                case 30:
                case 248:
                case 241:
                case 227:
                case 156:
                case 249:
                case 250:
                case 254:
                    codepoint = (altcharset_cp437[inbyte] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[inbyte])
                        ) ? uni_cp437_halfwidth[inbyte] : (ascii_cp437[inbyte] & MY_A_CHARTEXT);
                    break;
                case 8:
                    codepoint = (altcharset_cp437[8] !=
                                 SNAPSHOT_ADDCH__NARROWC(ascii_cp437[8])
                        ) ? uni_cp437_halfwidth[8] : (ascii_cp437[8] & MY_A_CHARTEXT);
                    if ((codepoint == (my_ulong) (ascii_cp437[8] & MY_A_CHARTEXT)) &&
                        (ascii_cp437[8] & ~MY_A_CHARTEXT))
                    {
                        p = pen_and_attr_to_pen(p, ascii_cp437[8] & ~MY_A_CHARTEXT);
                    }
                    break;
                default:
                    inbyte = (int) (my_uchar)
                        (ascii_cp437[inbyte] & MY_A_CHARTEXT);
                    codepoint = inbyte;
                }
            }
        }
        else
        {
            if (ascii_cp437[inbyte] & ~MY_A_CHARTEXT)
            {
                p = pen_and_attr_to_pen(p, ascii_cp437[inbyte] & ~MY_A_CHARTEXT);
            }
            inbyte = (int) (my_uchar)
                (ascii_cp437[inbyte] & MY_A_CHARTEXT);
            codepoint = inbyte;
        }
        if (p != snapshot_pen)
        {
            int old_pen;

            old_pen = snapshot_pen;
            snapshot_choose_pen(p);
            snapshot_addch(inbyte, p);
            snapshot_choose_pen(old_pen);
            return;
        }
        if (snapshot)
        {
            snapshot_choose_active_pen(snapshot_pen);
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
            fflush(snapshot);
        }
        if (snapshot_txt)
        {
            if (USE_ATTR)
            {
                int mono_pen;

                mono_pen = pen_to_mono_pen(snapshot_active_pen);
                if ((mono_pen != snapshot_active_pen)
                    &&
                    ((mono_pen == MY_PEN__DIM)
                     ||
                     (! (mono_pen & (MY_PEN_REVERSE | MY_PEN___SPECIAL))))
                    &&
                    (inbyte == ' '))
                {
                    mono_pen = MY_PEN_NORMAL;
                }
                if (mono_pen & MY_PEN_BOLD)
                {
                    if (mono_pen & MY_PEN_UNDERLINE)
                    {
                        fputs("_\b", snapshot_txt);
                    }
                    fputc_utf8(codepoint, snapshot_txt);
                    fputc('\b', snapshot_txt);
                }
                if (mono_pen & MY_PEN_UNDERLINE)
                {
                    fputs("_\b", snapshot_txt);
                }
            }
            fputc_utf8(codepoint, snapshot_txt);
            fflush(snapshot_txt);
        }
        snapshot_x += wcw;
    }
}

/* non-blocking version of getch(); return a single character if it is
 * available, ERR otherwise */
static int
my_getch(void)
{
    int k = ERR;
#if ! HAVE_NODELAY
    {
        int avail = 1;

#ifdef FIONREAD
        ioctl(fileno(stdin), FIONREAD, &avail);
#endif /* defined(FIONREAD) */
        if (! avail)
        {
            return k;
        }
    }
#endif
    k = getch();
    if (use_debug_input)
    {
        static int kprev = ERR;

        if (k != ERR) {
            if ((k >= ' ') && (k <= '~')) {
                fprintf(
                    stderr,
                    "%s%c\n",
                    (k == '\\') ? "\\" : "",
                    k);
            } else {
                fprintf(stderr, "\\x%02X\n", k);
            }
        } else {
            fprintf(stderr, ".");
        }
        fflush(stderr);
        if (k == '\r') {
            k = '\n';
            kprev = '\r';
        } else if ((kprev == '\r') && (k == '\n')) {
            k = ERR;
            kprev = '\n';
        } else if (k != ERR) {
            kprev = k;
        }
    }
    return k;
}

int
my_inner_addch(chtype c, int p)
{
    int ret;
    chtype effective_c;
    int effective_pen;

    effective_c = c;

    effective_pen = pen_and_attr_to_pen(p, c);
    if (effective_pen != p)
    {
        chtype translated_attrs;

        my_choose_pen(effective_pen);
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
        my_real_attrset(my_attrs);
#endif
#endif /* USE_ATTR || USE_COLOR */
        translated_attrs = pen_to_attr(p) ^ pen_to_attr(effective_pen);
        effective_c &= ~translated_attrs;
    }

    ret = addch(effective_c);

    if (effective_pen != p)
    {
        my_choose_pen(p);
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
        my_real_attrset(my_attrs);
#endif
#endif /* USE_ATTR || USE_COLOR */
    }

    return ret;
}

int my_ucs_fallback(my_ulong *my_ucsp)
{
    my_ulong my_ucs;

    my_ucs = *my_ucsp;
    /* U+256F BOX DRAWINGS LIGHT ARC UP AND LEFT -> U+2518 BOX DRAWINGS LIGHT UP AND LEFT */
    if (my_ucs == 0x256f)
    {
        my_ucs = 0x2518;
    }
    /* U+256E BOX DRAWINGS LIGHT ARC DOWN AND LEFT -> U+2510 BOX DRAWINGS LIGHT DOWN AND LEFT */
    if (my_ucs == 0x256e)
    {
        my_ucs = 0x2510;
    }
    /* U+256D BOX DRAWINGS LIGHT ARC DOWN AND RIGHT -> U+250C BOX DRAWINGS LIGHT DOWN AND RIGHT */
    if (my_ucs == 0x256d)
    {
        my_ucs = 0x250c;
    }
    /* U+2570 BOX DRAWINGS LIGHT ARC UP AND RIGHT -> U+2514 BOX DRAWINGS LIGHT UP AND RIGHT */
    if (my_ucs == 0x2570)
    {
        my_ucs = 0x2514;
    }
    /* U+30FB KATAKANA MIDDLE DOT -> 0x00B7 MIDDLE DOT */
    if (my_ucs == 0x30fb)
    {
        my_ucs = 0x00b7;
    }
    /* U+00B7 MIDDLE DOT -> 0xFF65 HALFWIDTH KATAKANA MIDDLE DOT */
    else if (my_ucs == 0x00b7)
    {
        my_ucs = 0xff65;
    }
    /* U+FF65 HALFWIDTH KATAKANA MIDDLE DOT -> 0xFF0E FULLWIDTH FULL STOP */
    else if (my_ucs == 0xff65)
    {
        my_ucs = 0xff0e;
    }
    /* U+301C WAVE DASH -> 0xFF5E FULLWIDTH TILDE */
    else if (my_ucs == 0x301c)
    {
        my_ucs = 0xff5e;
    }
    if (my_ucs != *my_ucsp)
    {
        *my_ucsp = my_ucs;
        return 1;
    }
    return 0;
}

static int
my_addstr_cp437(char *buf, int max_wcw)
{
    if (! use_raw_cp437)
    {
        char buf2[16];
        size_t i, sz;
        int ret = !ERR;

        sz = strlen(buf);
        for (i = 0; i < sz; i ++)
        {
            my_ulong u;
            short inbyte;
            int ret2;
            int my_wcw;
            int fallback;
            int old_x, old_y;

            getyx(stdscr, old_y, old_x);
            inbyte = (int) (my_uchar) buf[i];
          my_addstr_cp437_inner:
            fallback = fallback_cp437[inbyte];
            if (((chtype) fallback) == ascii_cp437[inbyte])
            {
                fallback = inbyte;
            }
            u = uni_cp437[inbyte];
          my_addstr_cp437_ucs_inner:
            my_wcw = 1;
            memset((void *) buf2, '\0', sizeof(buf2));
#ifdef LC_CTYPE
            if (! locale_is_utf8())
            {
                wchar_t wcbuf[2], wcbuf2[2];
                size_t cvret, cvret2;

                wcbuf[0] = ucs_to_wchar(u);
                if (! wcbuf[0])
                {
                    if (my_ucs_fallback(&u))
                    {
                        goto my_addstr_cp437_ucs_inner;
                    }
                    if (fallback != inbyte)
                    {
                        inbyte = fallback;
                        goto my_addstr_cp437_inner;
                    }
                    return ERR;
                }
                else
                {
                    const char *my_locale;

                    wcbuf[1] = '\0';
                    my_locale = locale_is_broken ? NULL : setlocale(LC_CTYPE, myman_locale);
                    cvret = wcstombs(buf2, wcbuf, sizeof(buf2));
                    buf2[sizeof(buf2) - 1] = '\0';
                    cvret2 = mbstowcs(wcbuf2, buf2, sizeof(wcbuf2) / sizeof(*wcbuf2));
                    if (my_locale)
                    {
                        setlocale(LC_CTYPE, my_locale);
                    }
                    wcbuf2[sizeof(wcbuf2)/sizeof(*wcbuf2) - 1] = 0;
                    if ((cvret == (size_t) -1) ||
                        (cvret2 == (size_t) -1) ||
                        (wcbuf2[0] != wcbuf[0]))
                    {
                        if (my_ucs_fallback(&u))
                        {
                            goto my_addstr_cp437_ucs_inner;
                        }
                        if (fallback != inbyte)
                        {
                            inbyte = fallback;
                            goto my_addstr_cp437_inner;
                        }
                        return ERR;
                    }
                    else
                    {
                        my_wcw = my_wcwidth(wcbuf[0]);
                        if (my_wcw < 1)
                        {
                            if (my_ucs_fallback(&u))
                            {
                                goto my_addstr_cp437_ucs_inner;
                            }
                            if (fallback != inbyte)
                            {
                                inbyte = fallback;
                                goto my_addstr_cp437_inner;
                            }
                            return ERR;
                        }
                        if ((my_wcw == 1) && (strlen(buf2) == 2))
                        {
                            my_wcw = 2;
                            location_is_suspect = 1;
                        }
                    }
                }
            }
            else
#endif /* defined(LC_CTYPE) */
            {
                if (u <= 0x7f)
                {
                    buf2[0] = u;
                }
                else if (u <= 0x7ff)
                {
                    buf2[0] = 0xc0 | (u >> 6);
                    buf2[1] = 0x80 | (u & 0x3f);
                }
                else if (u <= 0xffff)
                {
                    buf2[0] = 0xe0 | (u >> 12);
                    buf2[1] = 0x80 | ((u >> 6) & 0x3f);
                    buf2[2] = 0x80 | (u & 0x3f);
                }
                else if (u <= 0x10ffff)
                {
                    buf2[0] = 0xf0 | (u >> 18);
                    buf2[1] = 0x80 | ((u >> 12) & 0x3f);
                    buf2[2] = 0x80 | ((u >> 6) & 0x3f);
                    buf2[3] = 0x80 | (u & 0x3f);
                }
                else
                {
                    ret = ERR;
                    continue;
                }
                my_wcw = my_wcwidth(ucs_to_wchar(u));
            }
            if ((my_wcw < 1) || (my_wcw > max_wcw))
            {
                if (my_ucs_fallback(&u))
                {
                    goto my_addstr_cp437_ucs_inner;
                }
                if (fallback != inbyte)
                {
                    inbyte = fallback;
                    goto my_addstr_cp437_inner;
                }
                return ERR;
            }
            {
#ifdef LC_CTYPE
                const char *my_locale;

                my_locale = locale_is_broken ? NULL : setlocale(LC_CTYPE, myman_locale);
#endif
                ret2 = addstr(buf2);
#ifdef LC_CTYPE
                if (my_locale)
                {
                    setlocale(LC_CTYPE, my_locale);
                }
#endif
            }
            if (ret2 == ERR)
            {
                if (my_ucs_fallback(&u))
                {
                    move(old_y, old_x);
                    goto my_addstr_cp437_ucs_inner;
                }
                if (fallback != inbyte)
                {
                    move(old_y, old_x);
                    inbyte = fallback;
                    goto my_addstr_cp437_inner;
                }
            }
            if (ret2 != ERR)
            {
                int new_y, new_x;

                getyx(stdscr, new_y, new_x);
                if (((new_x % COLS) != ((old_x + my_wcw) % COLS)) ||
                    ((new_y != old_x) && (new_y != (old_y + 1))))
                {
                    location_is_suspect = 1;
                }
                if (location_is_suspect)
                {
                    leaveok(stdscr, FALSE);
                    move(0, 0);
                    refresh();
                    leaveok(stdscr, TRUE);
                    move(old_y, old_x + my_wcw);
                }
            }
            if (ret != ERR)
            {
                ret = ret2;
            }
        }
        return ret;
    }
    return addstr(buf);
}

/* add CP437 byte b with pen c */
int
my_addch(my_ulong b, int p)
{
    int ret = 0;
    chtype c = '\?';
    int old_y, old_x;
    int new_y, new_x;

    if ((fold_upper_case_to_lower_case == 1) && (b >= 'A') && (b <= 'Z'))
    {
        b = (b - 'A') + 'a';
    }
    else if ((fold_lower_case_to_upper_case == 1) && (b >= 'a') && (b <= 'z'))
    {
        b = (b - 'a') + 'A';
    }
    if (use_bgspace && use_color)
    {
        if (b == (my_ulong) 219)
        {
            if (((p / 16) != (p % 16)) ? (p % 16) : (p & MY_PEN_REVERSE))
            {
                p = ((p / 16) != (p % 16)) ? ((p % 16) * 16) : MY_PEN_REVERSE;
            }
            else
            {
                p = ((p / 16) != (p % 16)) ? 7 : MY_PEN_NORMAL;
            }
            b = ' ';
        }
        else if (b == (my_ulong) ' ')
        {
            if (((p / 16) != (p % 16)) ? (p / 16) : (p & MY_PEN_REVERSE))
            {
                if ((p / 16) != (p % 16)) p = (p / 16) * 16;
            }
            else
            {
                p = ((p / 16) != (p % 16)) ? 7 : MY_PEN_NORMAL;
            }
        }
    }
    if (use_fgblock && use_color)
    {
        if (b == (my_ulong) 219)
        {
            if (((p / 16) != (p % 16)) ? (p % 16) : (p & MY_PEN_REVERSE))
            {
                if (((p / 16) != (p % 16)) && (p % 16))
                {
                    p = p % 16;
                }
                else
                {
                    p = ((p / 16) != (p % 16)) ? 7 : MY_PEN_NORMAL;
                    b = ' ';
                }
            }
        }
        else if (b == (my_ulong) ' ')
        {
            if (((p / 16) != (p % 16)) ? (p / 16) : (p & MY_PEN_REVERSE))
            {
                p = ((p / 16) != (p % 16)) ? (p / 16) : (p ^ MY_PEN_REVERSE);
                b = (my_ulong) 219;
                if (p == MY_PEN___SPECIAL) p = MY_PEN_NORMAL;
            }
            else
            {
                p = ((p / 16) != (p % 16)) ? 7 : MY_PEN_NORMAL;
            }
        }
    }
    while (use_single && (b >= 179) && (b <= 218) && (((unsigned) (my_uchar) udlr[b]) & 0xaa))
    {
        my_ulong b_single;

        b_single = fallback_cp437[(int) b];
        if ((b_single >= 179) && (b_single <= 218))
        {
            b = b_single;
        }
        else
        {
            break;
        }
    }
    if (! b) b = ' ';
    getyx(stdscr, old_y, old_x);
    if ((old_y == last_valid_line)
        &&
        (old_x == (last_valid_col + 1)))
    {
        last_valid_col += CJK_MODE ? 2 : 1;
    }
    if (b == ' ')
    {
        if ((p / 16) == (p % 16))
        {
            if ((p == MY_PEN__DIM)
                ||
                (! (p & (MY_PEN_REVERSE | MY_PEN___SPECIAL))))
            {
                p = MY_PEN_NORMAL;
            }
        }
        else
        {
            p = ((p / 16) ? 0 : 7) + 16 * (p / 16);
        }
    }
    my_choose_pen(p);
    snapshot_addch(b, p);
    if (CJK_MODE && ! (use_acs && use_raw && use_raw_ucs))
    {
        my_uchar rhs;

        rhs = cp437_fullwidth_rhs[b];
        if ((int) (my_uchar) rhs)
        {
            snapshot_addch(rhs, p);
        }
    }
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
    my_real_attrset(my_attrs);
#endif
#endif /* USE_ATTR || USE_COLOR */
    do
    {
        if (use_acs && use_raw && ! use_raw_ucs)
        {
            char buf[2];
            buf[0] = (char) (my_uchar) (b & 0xFF);
            buf[1] = '\0';
            ret = my_addstr_cp437(buf, CJK_MODE ? 2 : 1);
            getyx(stdscr, new_y, new_x);
            if ((old_x != new_x) || (old_y != new_y))
            {
                if (CJK_MODE && ((new_x % COLS) == ((old_x + 1) % COLS)))
                {
                    my_uchar rhs;

                    rhs = cp437_fullwidth_rhs[b];
                    if ((int) (my_uchar) rhs)
                    {
                        buf[0] = (char) (my_uchar) (0xFFU & (unsigned) rhs);
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
                        if (my_attrs) my_real_attrset(0);
#endif
#endif /* USE_ATTR || USE_COLOR */
                        move(old_y, old_x + 1);
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
                        my_real_attrset(my_attrs);
#endif
#endif /* USE_ATTR || USE_COLOR */
                        my_addstr_cp437(buf, 1);
                        move(old_y, old_x + 2);
                    }
                }
                if (CJK_MODE)
                {
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
                    if (my_attrs) my_real_attrset(0);
#endif
#endif /* USE_ATTR || USE_COLOR */
                    move(old_y, old_x + 2);
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
                    my_real_attrset(my_attrs);
#endif
#endif /* USE_ATTR || USE_COLOR */
                }
                break;
            }
        }
        if (b <= 0xFF)
        {
            if (use_acs)
            {
#if USE_WIDEC_SUPPORT
                if (use_raw && use_raw_ucs)
                {
                    my_ulong my_ucs;
                    wchar_t my_wch;
                    int my_wcw;
                    int b_mapped;

                    b_mapped = b;
                  my_addch_wide_inner:
                    my_ucs = uni_cp437[b_mapped];
                  my_addch_wide_ucs_inner:
                    my_wch = ucs_to_wchar(my_ucs);
                  my_addch_wide_wch_inner:
                    my_wcw = my_wch ? my_wcwidth(my_wch) : 0;
                    if ((my_wcw <= 0) || (my_wcw > ((CJK_MODE) ? 2 : 1)))
                    {
                        int b_fallback;

                        if (my_ucs_fallback(&my_ucs))
                        {
                            goto my_addch_wide_ucs_inner;
                        }
                        b_fallback = fallback_cp437[b_mapped];
                        if ((b_fallback != b_mapped) &&
                            ((b_fallback < ' ') || (b_fallback > '~')))
                        {
                            b_mapped = b_fallback;
                            goto my_addch_wide_inner;
                        }
                    }
                    else
                    {
                        ret = addnwstr(&my_wch, 1);
                        getyx(stdscr, new_y, new_x);
                        if ((old_x != new_x) || (old_y != new_y))
                        {
                            if (CJK_MODE && (my_wcw == 1))
                            {
                                my_uchar rhs;
                                wchar_t wrhs;
                                int wcwrhs;
                                int rhs_mapped;

                                rhs = cp437_fullwidth_rhs[b_mapped];
                                rhs_mapped = (int) (my_uchar) rhs;
                              my_addch_wide_rhs_inner:
                                if (! rhs_mapped)
                                {
                                    rhs_mapped = ' ';
                                }
                                wrhs = ucs_to_wchar(uni_cp437[rhs_mapped]);
                                wcwrhs = wrhs ? my_wcwidth(wrhs) : 0;
                                if ((my_wcw + wcwrhs) == 2)
                                {
                                    my_wcw += wcwrhs;
                                    addnwstr(&wrhs, 1);
                                }
                                else
                                {
                                    int rhs_fallback;

                                    rhs_fallback = fallback_cp437[rhs_mapped];
                                    if (! rhs_fallback)
                                    {
                                        rhs_fallback = ' ';
                                    }
                                    if (rhs_fallback != rhs_mapped)
                                    {
                                        rhs_mapped = rhs_fallback;
                                        goto my_addch_wide_rhs_inner;
                                    }
                                    addch(' ');
                                    my_wcw = 2;
                                }
                            }
                            else if (CJK_MODE && (my_wcw == 2)
                                     &&
                                     (((old_x + 1) % COLS) == (new_x % COLS)))
                            {
                                location_is_suspect = 1;
                                leaveok(stdscr, FALSE);
                                move(0, 0);
                                refresh();
                                leaveok(stdscr, TRUE);
                                move((old_y + ((old_x + 2) / COLS)) % LINES, (old_x + 2) % COLS);
                            }
                            break;
                        }
                        else
                        {
                            my_wch = 0;
                            goto my_addch_wide_wch_inner;
                        }
                    }
                }
                if (altcharset_cp437[b])
                {
                    wchar_t my_wchbuf[CCHARW_MAX];
                    attr_t my_acs_attrs;
                    short my_color_pair, cch_color_pair;
                    attr_t my_current_attrs;
                    int my_len;
                    int wide_ok;

#if USE_CCHAR_FOR_ALTCHARSET
                    my_len =
                        getcchar(
                            MY_WACS_PTR altcharset_cp437[b],
                            NULL,
                            &my_acs_attrs,
                            &cch_color_pair,
                            NULL);
                    wide_ok = my_len &&
                        (getcchar(
                            MY_WACS_PTR altcharset_cp437[b],
                            my_wchbuf,
                            &my_acs_attrs,
                            &cch_color_pair,
                            NULL) != ERR) &&
                        (my_wcswidth(my_wchbuf, my_len) == 1);
#else
                    my_len = 1;
                    my_acs_attrs = 0;
#ifdef WA_ALTCHARSET
                    my_acs_attrs |= WA_ALTCHARSET;
#endif
                    my_wchbuf[0] = altcharset_cp437[b];
                    my_wchbuf[1] = 0;
                    wide_ok = my_wchbuf[0];
#endif
                    if (wide_ok)
                    {
#if USE_3ARGS_FOR_ATTR_SET
                        attr_get(
                            & my_current_attrs,
                            & my_color_pair,
                            NULL);
                        attr_set(
                            my_current_attrs | my_acs_attrs,
                            my_color_pair,
                            NULL);
#else
                        my_current_attrs = attr_get();
#if ! USE_CCHAR_FOR_ALTCHARSET
                        my_color_pair = PAIR_NUMBER(my_current_attrs);
#endif
                        attr_set(my_current_attrs | my_acs_attrs);
#endif
                        ret = addnwstr(
                            my_wchbuf,
                            my_len);
                        getyx(stdscr, new_y, new_x);
                        if (CJK_MODE && ((old_x != new_x) || (old_y != new_y)))
                        {
                            my_uchar rhs;
                        
                            rhs = cp437_fullwidth_rhs[b];
                            if ((int) (my_uchar) rhs)
                            {
                                if (altcharset_cp437[(int) (my_uchar) rhs])
                                {
#if USE_CCHAR_FOR_ALTCHARSET
                                    my_len =
                                        getcchar(
                                            MY_WACS_PTR altcharset_cp437[(int) (my_uchar) rhs],
                                            NULL,
                                            &my_acs_attrs,
                                            &cch_color_pair,
                                            NULL);
                                    wide_ok = my_len &&
                                        (getcchar(
                                            MY_WACS_PTR altcharset_cp437[(int) (my_uchar) rhs],
                                            my_wchbuf,
                                            &my_acs_attrs,
                                            &cch_color_pair,
                                            NULL) != ERR) &&
                                        (my_wcswidth(my_wchbuf, my_len) == 1);
#else
                                    my_len = 1;
                                    my_acs_attrs = 0;
#ifdef WA_ALTCHARSET
                                    my_acs_attrs |= WA_ALTCHARSET;
#endif
                                    my_wchbuf[0] = altcharset_cp437[(int) (my_uchar) rhs];
                                    my_wchbuf[1] = 0;
                                    wide_ok = my_wchbuf[0];
#endif
                                    if (wide_ok)
                                    {
#if USE_3ARGS_FOR_ATTR_SET
                                        attr_set(
                                            my_current_attrs | my_acs_attrs,
                                            my_color_pair,
                                            NULL);
#else
                                        attr_set(my_current_attrs | my_acs_attrs);
#endif
                                        addnwstr(
                                            my_wchbuf,
                                            my_len);
                                    }
                                }
                                else
                                {
                                    my_inner_addch(ascii_cp437[(int) (my_uchar) rhs], p);
                                }
                            }
                        }
#ifdef USE_3ARGS_FOR_ATTR_SET
                        attr_set(
                            my_current_attrs,
                            my_color_pair,
                            NULL);
#else
                        attr_set(my_current_attrs);
#endif
                        if ((old_x != new_x) || (old_y != new_y))
                        {
                            break;
                        }
                    }
                }
#else /* ! USE_WIDEC_SUPPORT */
                if (use_raw && use_raw_ucs)
                {
                    int b_mapped;
                    my_ulong ucs;

                    b_mapped = b;
                  my_addch_narrow_inner:
                    ucs = uni_cp437[b_mapped];
                  my_addch_narrow_ucs_inner:
                    c = (chtype) ucs;
                    ret = my_inner_addch(c, p);
                    getyx(stdscr, new_y, new_x);
                    if ((old_x == new_x) && (old_y == new_y))
                    {
                        int b_fallback;

                        if (my_ucs_fallback(&ucs))
                        {
                            goto my_addch_narrow_ucs_inner;
                        }
                        b_fallback = fallback_cp437[b_mapped];
                        if ((b_fallback != b_mapped) &&
                            ((b_fallback < ' ') || (b_fallback > '~')))
                        {
                            b_mapped = b_fallback;
                            goto my_addch_narrow_inner;
                        }
                    }
                    else
                    {
                        if (CJK_MODE)
                        {
                            wchar_t my_wch;

                            my_wch = ucs_to_wchar(ucs);
                            if (my_wch && (my_wcwidth(my_wch) < 2))
                            {
                                my_uchar rhs, rhs_mapped;

                                rhs = cp437_fullwidth_rhs[b_mapped];
                                rhs_mapped = rhs;
                              my_addch_narrow_rhs_inner:
                                if ((int) (my_uchar) rhs_mapped)
                                {
                                    ucs = uni_cp437[(int) (my_uchar) rhs_mapped];
                                  my_addch_narrow_rhs_ucs_inner:
                                    c = (chtype) ucs;
                                    my_wch = ucs_to_wchar(c);
                                    getyx(stdscr, old_y, old_x);
                                    if (my_wch && (my_wcwidth(my_wch) < 2))
                                    {
                                        my_inner_addch(c, p);
                                        getyx(stdscr, new_y, new_x);
                                        if ((old_x != new_x) || (old_y != new_y))
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            int rhs_fallback;

                                            if (my_ucs_fallback(&ucs))
                                            {
                                                goto my_addch_narrow_rhs_ucs_inner;
                                            }
                                            rhs_fallback = fallback_cp437[rhs_mapped];
                                            if ((rhs_fallback != rhs_mapped) &&
                                                ((rhs_fallback < ' ') || (rhs_fallback > '~')))
                                            {
                                                rhs_mapped = rhs_fallback;
                                                goto my_addch_narrow_rhs_inner;
                                            }
                                        }
                                    }
                                    c = altcharset_cp437[(int) (my_uchar) rhs];
                                    my_inner_addch(c, p);
                                    getyx(stdscr, new_y, new_x);
                                    if ((old_x != new_x) || (old_y != new_y))
                                    {
                                        break;
                                    }
                                    my_inner_addch(ascii_cp437[(int) (my_uchar) rhs], p);
                                }
                            }
                        }
                        break;
                    }
                }
                c = altcharset_cp437[b];
                ret = my_inner_addch(c, p);
                getyx(stdscr, new_y, new_x);
                if ((old_x != new_x) || (old_y != new_y))
                {
                    if (CJK_MODE && ((new_x % COLS) != ((old_x + 2) % COLS)))
                    {
                        my_uchar rhs;
                    
                        rhs = (my_uchar) (unsigned) (chtype) cp437_fullwidth_rhs[b];
                        if ((int) (my_uchar) rhs)
                        {
                            c = altcharset_cp437[(int) (my_uchar) rhs];
                            my_inner_addch(c, p);
                        }
                    }
                    break;
                }
#endif /* ! USE_WIDEC_SUPPORT */
            }
        }
        c = ascii_cp437[b];
        getyx(stdscr, old_y, old_x);
        ret = my_inner_addch(c, p);
        getyx(stdscr, new_y, new_x);
        if (CJK_MODE && ((new_x % COLS) != ((old_x + 2) % COLS)))
        {
            my_uchar rhs;

            rhs = cp437_fullwidth_rhs[b];
            if ((int) (my_uchar) rhs)
            {
                c = ascii_cp437[(int) (my_uchar) rhs];
                my_inner_addch(c, p);
            }
        }
    }
    while (0);
#if USE_ATTR || USE_COLOR
#if DANGEROUS_ATTRS
    if (my_attrs) my_real_attrset(0);
#endif
#endif /* USE_ATTR || USE_COLOR */
    return ret;
}

/* add CP437 string s with pen c */
int
my_addstr(const char *s, int c)
{
    size_t i;
    int ret = 0;
    int y, x;

    getyx(stdscr, y, x);
    for (i = 0; s[i]; i ++)
    {
        my_ulong b;

        b = (my_ulong) (my_uchar) s[i];
        move(y, x + i * (CJK_MODE ? 2 : 1));
        ret = my_addch(b, c);
        if (ret == ERR)
        {
            break;
        }
    }
    return ret;
}

#ifndef MY_CURSES_ARGS
#ifdef XCURSES
#define MY_CURSES_ARGS 1
#endif
#endif

#ifndef MY_CURSES_ARGS
#ifdef INITSCR_WITH_HINTS_AND_ARGS
#define MY_CURSES_ARGS 1
#endif
#endif

#ifndef MY_CURSES_ARGS
#define MY_CURSES_ARGS 0
#endif

#ifndef MY_CURSES_USAGE
#ifdef INITSCR_WITH_HINTS_AND_ARGS
#define MY_CURSES_USAGE " %s"
#define MY_CURSES_USAGE_PARAMETERS(stream) \
    , initscrUsage(stream)
#endif
#endif

#ifndef MY_CURSES_USAGE
#ifdef XCURSES
#define MY_CURSES_USAGE                         \
    " [ -- toolkit-options]"
#define MY_CURSES_USAGE_PARAMETERS(stream)
#endif
#endif

#ifndef MY_CURSES_USAGE
#if MY_CURSES_ARGS
#define MY_CURSES_USAGE                         \
" [ -- output-driver-options]"
#define MY_CURSES_USAGE_PARAMETERS(stream)
#endif
#endif

#ifndef MY_CURSES_USAGE
#define MY_CURSES_USAGE "%s"
#define MY_CURSES_USAGE_PARAMETERS(stream)      \
    , ""
#endif

#ifndef MY_CURSES_USAGE_PARAMETERS
#define MY_CURSES_USAGE_PARAMETERS(stream)
#endif

#ifndef MY_CURSES_HELP
#ifdef XCURSES
#define MY_CURSES_HELP(stream, progname)                                \
    "Toolkit options:" "\n"                                             \
    "For a list of standard toolit options, see the X(7) manual entry." "\n" \
    "For a list of X Resources understood by PDCurses for X11, see:" "\n" \
    "    http://pdcurses.sourceforge.net/doc/PDCurses.txt" "\n"         \
    "or doc/x11.txt in the PDCurses source directory." "\n"             \
    "The top-level widget name is: %s" "\n"                             \
    , strrchr((progname), '/') ? (strrchr((progname), '/') + 1) : (progname)
#endif
#endif

#ifndef MY_CURSES_HELP
#ifdef INITSCR_WITH_HINTS_AND_ARGS
#define MY_CURSES_HELP(stream, progname)        \
    "%s", initscrHelp((stream), (progname))
#endif
#endif

#ifndef MY_CURSES_HELP
#define MY_CURSES_HELP(stream, progname) "%s", ""
#endif

const char * pager_notice = 0;
const char * pager_remaining = 0;
/* left then right in the pager is equivalent to space */
int pager_arrow_magic = 0;

int
pager_big(void)
{
    return ((((COLS / tile_w) * (LINES / pager_tile_h)) >= 80) && (tile_w >= 4) && ((tile_h >= 3) || ((tile_h) * 2 == tile_w)));
}

int
pager_lines(void)
{
    return pager_big() ? (LINES / pager_tile_h) : LINES;
}

int
pager_cols(void)
{
    return pager_big() ? (MY_COLS / tile_w) : MY_COLS;
}

#ifndef PAGER_PEN_STANDOUT
#define PAGER_PEN_STANDOUT (use_color ? PAUSE_COLOR : MY_PEN_REVERSE)
#endif

void
pager_move(int y, int x)
{
    int big;

    big = pager_big();
    my_move((big ? ((y) * pager_tile_h) : y),
            ((big ? ((x) * tile_w) : x) * (use_fullwidth ? 2 : 1)));
}

void
pager_getyx(int *y, int *x)
{
    int ly, lx;

    getyx(stdscr, ly, lx);
    if (pager_big())
    {
        ly /= pager_tile_h;
        lx /= tile_w;
    }
    lx /= use_fullwidth ? 2 : 1;
    *y = ly;
    *x = lx;
}

static void
pager_addstr(const char *s, int c)
{
    const char *pager_addstr__s = (s);

    while (*pager_addstr__s)
    {
        pager_addch((my_ulong) (my_uchar) *pager_addstr__s, c);
        pager_addstr__s ++;
    }
}

void
my_usleep(long usecs)
{
    usleep(usecs);
}

double
doubletime(void)
{
    struct timeval tval;

    tval.tv_sec = 0;
    tval.tv_usec = 0;
    if (myman_gettimeofday(&tval, NULL))
    {
        return -1.0L;
    }
    return 1.0L * tval.tv_sec + 1e-6L * tval.tv_usec;
}

static void
pager(void)
{
    int c = ERR;
    int k = ERR;

    my_choose_pen(MY_PEN_NORMAL);
    my_erase();
    if (use_color)
    {
        my_choose_pen(TEXT_COLOR);
    }
    if (! pager_remaining)
    {
        if (pager_notice)
        {
            pager_remaining = pager_notice;
        }
    }
    while (pager_remaining && (! quit_requested) && (! reinit_requested))
    {
        const char *pager;
        int y, x;
        int plines, pcols;

        plines = pager_lines();
        pcols = pager_cols();
        pager_move(0, 0);
        if (use_color)
        {
            my_choose_pen(TEXT_COLOR);
        }
        pager = pager_remaining;
        while (*pager)
        {
            c = *pager;
            pager ++;
            pager_getyx(&y, &x);
            if (y && ((y + 1) >= plines) && (c != '\n') && (c != ' '))
            {
                pager --;
                c = '\n';
            }
            if (c == ' ')
            {
                int wlen;

                for (wlen = 0; (pager[wlen] != ' ') && (pager[wlen] != '\n') && (pager[wlen] != '\0'); wlen ++)
                {
                    if ((x + wlen + 1) >= pcols)
                    {
                        c = '\n';
                        break;
                    }
                }
                if ((x + 1) >= pcols)
                {
                    c = '\n';
                }
                if (c == '\n')
                {
                    while (*pager == ' ')
                    {
                        pager ++;
                    }
                    if (*pager == '\n')
                    {
                        pager ++;
                    }
                }
            }
            else if ((c != '\n')
                     &&
                     (! (((c >= 'A') && (c <= 'Z'))
                         ||
                         ((c >= 'a') && (c <= 'z'))
                         ||
                         ((c >= '0') && (c <= '9'))
                         ||
                         (c == '(')
                         ||
                         (c == '`')
                         ||
                         (c == '\'')
                         ||
                         (c == '"')
                         ||
                         (c == '{')
                         ||
                         (c == '[')
                         ||
                         (c == '<'))))
            {
                int wlen;

                for (wlen = 0;
                     ((pager[wlen] >= 'A') && (pager[wlen] <= 'Z'))
                         ||
                         ((pager[wlen] >= 'a') && (pager[wlen] <= 'z'))
                         ||
                         ((pager[wlen] >= '0') && (pager[wlen] <= '9'))
                         ||
                         (pager[wlen] == '>')
                         ||
                         (pager[wlen] == ']')
                         ||
                         (pager[wlen] == '}')
                         ||
                         (pager[wlen] == '%')
                         ||
                         (pager[wlen] == '\'')
                         ||
                         (pager[wlen] == '"')
                         ||
                         (pager[wlen] == ')')
                         ||
                         (pager[wlen] == '.')
                         ||
                         (pager[wlen] == ',')
                         ||
                         (pager[wlen] == '!')
                         ||
                         (pager[wlen] == '\?')
                         ||
                         (pager[wlen] == ':')
                         ||
                         (pager[wlen] == ';');
                     wlen ++)
                {
                    if ((x + wlen + 1) >= pcols)
                    {
                        c = '\n';
                        break;
                    }
                }
                if ((x + 1) >= pcols)
                {
                    c = '\n';
                }
                if (c == '\n')
                {
                    pager_addch((my_ulong) (my_uchar) *(pager - 1),
                                use_color
                                ?
                                TEXT_COLOR
                                :
                                MY_PEN_NORMAL
                        );
                    x ++;
                    while (*pager == ' ')
                    {
                        pager ++;
                    }
                    if (*pager == '\n')
                    {
                        pager ++;
                    }
                }
            }
            else if (((x + 2) == pcols)
                     &&
                     (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
                     &&
                     (((pager[0] >= 'A') && (pager[0] <= 'Z')) || ((pager[0] >= 'a') && (pager[0] <= 'z')))
                     &&
                     (((pager[1] >= 'A') && (pager[1] <= 'Z')) || ((pager[1] >= 'a') && (pager[1] <= 'z'))))
            {
                /* FIXME: this hyphenation rule is really not even
                 * adequate for English. Fortunately, we almost
                 * never actually use it. */
                pager_addch((my_ulong) (my_uchar) c,
                            use_color
                            ?
                            TEXT_COLOR
                            :
                            MY_PEN_NORMAL
                    );
                x ++;
                pager_addch((my_ulong) (my_uchar) '-',
                            use_color
                            ?
                            TEXT_COLOR
                            :
                            MY_PEN_NORMAL
                    );
                x ++;
                c = '\n';
            }
            if (c != '\n')
            {
                pager_getyx(&y, &x);
                pager_addch((my_ulong) (my_uchar) c,
                            use_color
                            ?
                            TEXT_COLOR
                            :
                            MY_PEN_NORMAL
                    );
                pager_getyx(&y, &x);
                while ((x == 0) && (pcols > 1) && (*pager == ' '))
                {
                    pager ++;
                }
                if (x == 0)
                {
                    y --;
                    x = pcols;
                    c = '\n';
                }
                if (c == '\n')
                {
                    while (*pager == ' ')
                    {
                        pager ++;
                    }
                    if (*pager == '\n')
                    {
                        pager ++;
                    }
                }
            }
            if (c == '\n')
            {
                while ((x ++) < pcols)
                {
                    pager_addch((my_ulong) (my_uchar) ' ',
                                use_color
                                ?
                                TEXT_COLOR
                                :
                                MY_PEN_NORMAL
                        );
                }
                y ++;
                x = 0;
                pager_move(y, x);
                if ((y + 1) >= plines)
                {
                    while (*pager == '\n')
                    {
                        pager ++;
                    }
                    if (! *pager)
                    {
                        break;
                    }
                    if (plines > 1)
                    {
                        pager_move(plines - 1, 0);
                        pager_addstr(MOREMESSAGE, PAGER_PEN_STANDOUT);
                        pager_getyx(&y, &x);
                        while (((x ++) < pcols) && (y < plines))
                        {
                            pager_addch((my_ulong) (my_uchar) ' ', PAGER_PEN_STANDOUT);
                        }
                    }
                    my_refresh();
                    do
                    {
                        while ((k = my_getch()) == ERR)
                        {
                            my_refresh();
                            if (got_sigwinch) break;
                            my_usleep(100000UL);
                        }
                        if (IS_LEFT_ARROW(k) || (k == '<') || (k == ','))
                        {
                            pager_arrow_magic = 1;
                            continue;
                        }
                        else
                        {
                            if ((pager_arrow_magic == 1) && (IS_RIGHT_ARROW(k) || (k == '>') || (k == '.')))
                            {
                                k = 27;
                            }
                            pager_arrow_magic = 0;
                        }
                        break;
                    }
                    while (1);
                    my_choose_pen(MY_PEN_NORMAL);
                    my_erase();
                    if (use_color)
                    {
                        my_choose_pen(TEXT_COLOR);
                    }
                    y = 0;
                    x = 0;
                    pager_move(y, x);
#ifdef KEY_RESIZE
                    if (k == KEY_RESIZE)
                    {
                        reinit_requested = 1;
                        pager = pager_remaining;
                        break;
                    }
                    else
#endif
                        if ((k == '@') || (got_sigwinch && (k == ERR)))
                        {
                            if (got_sigwinch)
                            {
                                ever_got_sigwinch = 1;
                                use_env(FALSE);
                            }
                            got_sigwinch = 0;
                            reinit_requested = 1;
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == 'r') || (k == 'R') || (k == MYMANCTRL('L')) || (k == MYMANCTRL('R')))
                        {
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            break;
                        }
                        else if (k == 27)
                        {
                            pager_arrow_magic = 0;
                            pager_notice = 0;
                            pager_remaining = 0;
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == 'q') || (k == 'Q') || (k == MYMANCTRL('C')))
                        {
                            quit_requested = 1;
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == 'a') || (k == 'A'))
                        {
                            use_acs = ! use_acs;
                            use_acs_p = 1;
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == 'c') || (k == 'C'))
                        {
                            use_color = ! use_color;
                            use_color_p = 1;
                            if (use_color)
                                init_pen();
                            else
                                destroy_pen();
                            my_choose_pen(MY_PEN_NORMAL);
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            continue;
                        }
                        else if ((k == 'b') || (k == 'B'))
                        {
                            use_dim_and_bright =
                                ! use_dim_and_bright;
                            use_dim_and_bright_p = 1;
                            if (use_color)
                            {
                                destroy_pen();
                                init_pen();
                            }
                            my_choose_pen(MY_PEN_NORMAL);
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            continue;
#if USE_ATTR
                        } else if ((k == 'u') || (k == 'U')) {
                            use_underline = ! use_underline;
                            if (use_color)
                            {
                                destroy_pen();
                                init_pen();
                            }
                            my_choose_pen(MY_PEN_NORMAL);
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            continue;
#endif
                        }
                        else if ((k == 'x') || (k == 'X'))
                        {
                            use_raw = ! use_raw;
                            init_trans(use_bullet_for_dots);
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == 'e') || (k == 'E'))
                        {
                            use_raw_ucs = ! use_raw_ucs;
                            init_trans(use_bullet_for_dots);
                            my_clear();
#if HAVE_CURS_SET
                            if (curs_set(0) -0 == ERR)
                            {
                                leaveok(stdscr, FALSE);
                            }
#endif
                            clearok(curscr, TRUE);
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == 't') || (k == 'T'))
                        {
                            snapshot_start();
                            break;
                        }
                        else if (IS_UP_ARROW(k) && (pager_remaining != pager_notice))
                        {
                            if (pager_remaining != pager_notice)
                            {
                                pager_remaining --;
                                while ((pager_remaining != pager_notice)
                                       &&
                                       ((*(pager_remaining - 1)) != '\n'))
                                {
                                    pager_remaining --;
                                }
                            }
                            pager = pager_remaining;
                            break;
                        }
                        else if (IS_DOWN_ARROW(k) || (k == '\r') || (k == '\n'))
                        {
                            while ((pager_remaining != pager)
                                   &&
                                   (*pager_remaining)
                                   &&
                                   ((*pager_remaining) != '\n'))
                            {
                                pager_remaining ++;
                            }
                            if (*pager_remaining == '\n')
                            {
                                pager_remaining ++;
                            }
                            pager = pager_remaining;
                            break;
                        }
                        else if (IS_LEFT_ARROW(k) || IS_RIGHT_ARROW(k))
                        {
                            pager = pager_remaining;
                            break;
                        }
                        else if ((k == MYMANCTRL('@')) && (k != ERR))
                        {
                            /* NUL - idle keepalive (iTerm, maybe others?) */
                            pager = pager_remaining;
                            break;
                        }
                        else if (k == MYMANCTRL('S'))
                        {
                            xoff_received = 1;
                            pager = pager_remaining;
                            break;
                        }
                        else if (k == MYMANCTRL('Q'))
                        {
                            xoff_received = 0;
                            pager = pager_remaining;
                            break;
                        }
                        else if (k == ' ')
                        {
                            pager_remaining = pager;
                            continue;
                        }
                        else if ((k == 's') || (k == 'S'))
                        {
                            use_sound = ! use_sound;
                            pager = pager_remaining;
                            break;
                        }
                        else if (k != ERR)
                        {
#if USE_BEEP
                            if (use_sound) beep();
#endif
                            pager = pager_remaining;
                            break;
                        }
                }
            }
            pager_move(y, x);
        }
        if (pager && (! (*pager)))
        {
            if (plines > 1)
            {
                pager_getyx(&y, &x);
                while ((y + 1) < plines)
                {
                    if (! x)
                    {
                        pager_move(y, x);
                        pager_addch((my_ulong) (my_uchar) '~',
                                    use_color
                                    ?
                                    TEXT_COLOR
                                    :
                                    MY_PEN_NORMAL
                            );
                        x ++;
                        while ((x ++) < pcols)
                        {
                            pager_addch((my_ulong) (my_uchar) ' ',
                                        use_color
                                        ?
                                        TEXT_COLOR
                                        :
                                        MY_PEN_NORMAL
                                );
                        }
                    }
                    x = 0;
                    y ++;
                }
                pager_move(y, 0);
                pager_addstr(DONEMESSAGE, PAGER_PEN_STANDOUT);
                pager_getyx(&y, &x);
                while (((x ++) < pcols) && (y < plines))
                {
                    pager_addch((my_ulong) (my_uchar) ' ', PAGER_PEN_STANDOUT);
                }
            }
            my_refresh();
            while ((k = my_getch()) == ERR)
            {
                my_refresh();
                if (got_sigwinch) break;
                my_usleep(100000UL);
            }
            if (IS_LEFT_ARROW(k) || (k == '<') || (k == ','))
            {
                pager_arrow_magic = 1;
            }
            else
            {
                if ((pager_arrow_magic == 1) && (IS_RIGHT_ARROW(k) || (k == '>') || (k == '.')))
                {
                    k = 27;
                }
                pager_arrow_magic = 0;
            }
#ifdef KEY_RESIZE
            if (k == KEY_RESIZE)
            {
                reinit_requested = 1;
            }
            else
#endif
            {
                if (k == 27)
                {
                    pager_arrow_magic = 0;
                    pager_notice = 0;
                    pager_remaining = 0;
                }
                else if ((k == 'q') || (k == 'Q') || (k == MYMANCTRL('C')))
                {
                    quit_requested = 1;
                }
                else if ((k == 'a') || (k == 'A'))
                {
                    use_acs = ! use_acs;
                    use_acs_p = 1;
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
                }
                else if ((k == 'c') || (k == 'C'))
                {
                    use_color = ! use_color;
                    use_color_p = 1;
                    if (use_color)
                        init_pen();
                    else
                        destroy_pen();
                    my_choose_pen(MY_PEN_NORMAL);
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
                }
                else if ((k == 'b') || (k == 'B'))
                {
                    use_dim_and_bright =
                        ! use_dim_and_bright;
                    use_dim_and_bright_p = 1;
                    if (use_color)
                    {
                        destroy_pen();
                        init_pen();
                    }
                    my_choose_pen(MY_PEN_NORMAL);
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
#if USE_ATTR
                } else if ((k == 'u') || (k == 'U')) {
                    use_underline = ! use_underline;
                    if (use_color)
                    {
                        destroy_pen();
                        init_pen();
                    }
                    my_choose_pen(MY_PEN_NORMAL);
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
#endif
                }
                else if ((k == 'x') || (k == 'X'))
                {
                    use_raw = ! use_raw;
                    init_trans(use_bullet_for_dots);
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
                }
                else if ((k == 'e') || (k == 'E'))
                {
                    use_raw_ucs = ! use_raw_ucs;
                    init_trans(use_bullet_for_dots);
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
                }
                else if ((k == 't') || (k == 'T'))
                {
                    snapshot_start();
                }
                else if (IS_UP_ARROW(k) && (pager_remaining != pager_notice))
                {
                    if (pager_remaining != pager_notice)
                    {
                        pager_remaining --;
                        while ((pager_remaining != pager_notice)
                               &&
                               ((*(pager_remaining - 1)) != '\n'))
                        {
                            pager_remaining --;
                        }
                    }
                }
                else if (IS_DOWN_ARROW(k) || (k == '\r') || (k == '\n'))
                {
                    while ((*pager_remaining)
                           &&
                           ((*pager_remaining) != '\n'))
                    {
                        pager_remaining ++;
                    }
                    if (*pager_remaining == '\n')
                    {
                        pager_remaining ++;
                    }
                }
                else if (IS_LEFT_ARROW(k) || IS_RIGHT_ARROW(k))
                {
                }
                else if ((k == 'r') || (k == 'R') || (k == MYMANCTRL('L')) || (k == MYMANCTRL('R')))
                {
                    my_clear();
#if HAVE_CURS_SET
                    if (curs_set(0) -0 == ERR)
                    {
                        leaveok(stdscr, FALSE);
                    }
#endif
                    clearok(curscr, TRUE);
                }
                else if ((k == '@') || (got_sigwinch && (k == ERR)))
                {
                    if (got_sigwinch)
                    {
                        ever_got_sigwinch = 1;
                        use_env(FALSE);
                    }
                    got_sigwinch = 0;
                    reinit_requested = 1;
                }
                else if ((k == MYMANCTRL('@')) && (k != ERR))
                {
                    /* NUL - idle keepalive (iTerm, maybe others?) */
                }
                else if (k == MYMANCTRL('S'))
                {
                    xoff_received = 1;
                }
                else if (k == MYMANCTRL('Q'))
                {
                    xoff_received = 0;
                }
                else if (k == ' ')
                {
                    pager_arrow_magic = 0;
                    pager_notice = 0;
                    pager_remaining = 0;
                }
                else if ((k == 's') || (k == 'S'))
                {
                    use_sound = ! use_sound;
                }
                else if (k != ERR)
                {
#if USE_BEEP
                    if (use_sound) beep();
#endif
                }
            }
        }
        my_choose_pen(MY_PEN_NORMAL);
        my_erase();
        if (use_color)
        {
            my_choose_pen(TEXT_COLOR);
        }
        my_move(0, 0);
    }
}

int key_buffer = ERR;
int key_buffer_ERR = ERR;

double td = 0.0L;
#if USE_SDL_MIXER
static int sdl_audio_open = 0;
#endif

#ifdef ALLEGROCURSES
#define handle_sfx(n) \
do { if (myman_sfx & myman_sfx_##n) \
{ \
    static MIDI *n##_music = 0; \
    myman_sfx &= ~myman_sfx_##n; \
    if ((use_sound && ! myman_demo) && ! n##_music) \
    { \
        n##_music = load_midi("sfx/"#n".mid"); \
    } \
    if ((use_sound && ! myman_demo) && n##_music) \
    { \
        stop_midi(); \
        play_midi(n##_music, 0); \
    } \
} } while (0)
#else
#if USE_SDL_MIXER
#define handle_sfx(n) \
do { if ((myman_sfx & myman_sfx_##n) && sdl_audio_open) \
{ \
    static Mix_Music *n##_music = 0; \
    myman_sfx &= ~myman_sfx_##n; \
    if ((use_sound && ! myman_demo) && ! n##_music) \
    { \
        n##_music = Mix_LoadMUS("sfx/"#n".xm"); \
    } \
    if ((use_sound && ! myman_demo) && ! n##_music) \
    { \
        n##_music = Mix_LoadMUS("sfx/"#n".mid"); \
    } \
    if ((use_sound && ! myman_demo) && n##_music) \
    { \
        if (! Mix_PlayingMusic()) Mix_PlayMusic(n##_music, 1); \
    } \
} } while (0)
#else
#if USE_BEEP
#define handle_sfx(n) do { if (myman_sfx & myman_sfx_##n) { myman_sfx &= ~myman_sfx_##n; if ((myman_sfx_##n & ~myman_sfx_nobeep_mask) && use_sound && ! myman_demo) beep(); } } while (0)
#else
#define handle_sfx(n) do { if (myman_sfx & myman_sfx_##n) { myman_sfx &= ~myman_sfx_##n; } } while (0)
#endif
#endif
#endif

#ifdef __DESMET__
#undef handle_sfx
#define handle_sfx(n)
#endif

void
gamesfx(void)
{
    handle_sfx(credit);
    handle_sfx(dot);
    handle_sfx(dying);
    handle_sfx(ghost);
    handle_sfx(intermission);
    handle_sfx(pellet);
    handle_sfx(siren0_down);
    handle_sfx(siren0_up);
    handle_sfx(siren1_down);
    handle_sfx(siren1_up);
    handle_sfx(siren2_down);
    handle_sfx(siren2_up);
    handle_sfx(start);
    handle_sfx(fruit);
    handle_sfx(life);
    handle_sfx(level);
    handle_sfx(bonus);
    if (myman_sfx)
    {
        myman_sfx = 0UL;
#if USE_BEEP
        if (use_sound && ! myman_demo) beep();
#endif
    }
}

int
gameinput(void)
{
    int k;
    int hero_can_move_left = 0;
    int hero_can_move_right = 0;
    int hero_can_move_up = 0;
    int hero_can_move_down = 0;
    my_uchar m1, m2;
    int xtile, ytile, x_off, y_off;

    x_off = sprite_register_x[HERO] % gfx_w;
    y_off = sprite_register_y[HERO] % gfx_h;
    xtile = XTILE(sprite_register_x[HERO]);
    ytile = YTILE(sprite_register_y[HERO]);
    while (1)
    {
        double td_pre, td_post;
        int qx, qy;

        td_pre = doubletime();
        k = my_getch();
        td_post = doubletime();
        /* a very slow keypress is likely a sign of unmapping, suspending, or some similar mess */

        /* TODO: treat job control signals similarly */
        if ((td_post - td_pre) >= 1.0)
        {
            ignore_delay = 1;
            frameskip = 0;
        }
        hero_can_move_down = hero_can_move_up = hero_can_move_right = hero_can_move_left = 1;
        for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
        {
            for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
            {
                m1 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx - NOTRIGHT(x_off))];
                m2 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                hero_can_move_left = hero_can_move_left &&
                    (ISOPEN((unsigned) m1)
                     ||
                     ISZAPLEFT((unsigned) m2));
                m1 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx + NOTLEFT(x_off))];
                m2 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                hero_can_move_right = hero_can_move_right &&
                    (ISOPEN((unsigned) m1)
                     ||
                     ISZAPRIGHT((unsigned) m2));
                m1 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy - NOTBOTTOM(y_off)) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                m2 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                hero_can_move_up = hero_can_move_up &&
                    (ISOPEN((unsigned) m1)
                     ||
                     ISZAPUP((unsigned) m2));
                m1 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy + NOTTOP(y_off)) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                m2 = (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                hero_can_move_down = hero_can_move_down &&
                    (ISOPEN((unsigned) m1)
                     ||
                     ISZAPDOWN((unsigned) m2));
            }
        }
#ifdef KEY_RESIZE
        if (k == KEY_RESIZE)
        {
            k = '@';
        }
#endif
        if ((k == 'q') || (k == 'Q') || (k == MYMANCTRL('C')) || quit_requested)
        {
            quit_requested = 0;
            return 0;
        }
        else if ((k == MYMANCTRL('@')) && (k != ERR))
        {
            /* NUL - idle keepalive (iTerm, maybe others?) */
            return 1;
        }
        else if (k == MYMANCTRL('S'))
        {
            xoff_received = 1;
            return 1;
        }
        else if (k == MYMANCTRL('Q'))
        {
            xoff_received = 0;
            return 1;
        }
        else if (k == '!')
        {
            gameinfo();
            return 0;
        }
        else if ((k == '\?') || (k == MYMANCTRL('H')))
        {
            gamehelp();
            return 0;
        } else if ((k == '@') || (got_sigwinch && (k == ERR)))
        {
            if (got_sigwinch)
            {
                ever_got_sigwinch = 1;
                use_env(FALSE);
            }
            got_sigwinch = 0;
            reinit_requested = 1;
            return 0;
        } else if ((k == 'r') || (k == 'R') || (k == MYMANCTRL('L')) || (k == MYMANCTRL('R'))) {
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            wrefresh(stdscr);
            ignore_delay = 1;
            frameskip = 0;
            return 1;
        } else if ((k == 'i') || (k == 'I'))
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
        } else if ((k == 'c') || (k == 'C')) {
            use_color = ! use_color;
            use_color_p = 1;
            if (use_color)
                init_pen();
            else
                destroy_pen();
            my_choose_pen(MY_PEN_NORMAL);
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
        } else if ((k == 'b') || (k == 'B')) {
            use_dim_and_bright =
                ! use_dim_and_bright;
            use_dim_and_bright_p = 1;
            if (use_color)
            {
                destroy_pen();
                init_pen();
            }
            my_choose_pen(MY_PEN_NORMAL);
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
#if USE_ATTR
        } else if ((k == 'u') || (k == 'U')) {
            use_underline = ! use_underline;
            if (use_color)
            {
                destroy_pen();
                init_pen();
            }
            my_choose_pen(MY_PEN_NORMAL);
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
#endif
        } else if ((k == 's') || (k == 'S')) {
            use_sound = ! use_sound;
            return 1;
        } else if ((k == 'o') || (k == 'O') || (k == '0'))
        {
            use_bullet_for_dots = ! use_bullet_for_dots;
            use_bullet_for_dots_p = 1;
            init_trans(use_bullet_for_dots);
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
        } else if ((k == 'a') || (k == 'A'))
        {
            use_acs = ! use_acs;
            use_acs_p = 1;
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
        } else if ((k == 'x') || (k == 'X'))
        {
            use_raw = ! use_raw;
            init_trans(use_bullet_for_dots);
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
        } else if ((k == '/') || (k == '\\'))
        {
            reflect = ! reflect;
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            if (IS_LEFT_ARROW(key_buffer)) key_buffer = KEY_UP;
            else if (IS_UP_ARROW(key_buffer)) key_buffer = KEY_LEFT;
            else if (IS_RIGHT_ARROW(key_buffer)) key_buffer = KEY_DOWN;
            else if (IS_DOWN_ARROW(key_buffer)) key_buffer = KEY_RIGHT;
            return 1;
        } else if ((k == 'e') || (k == 'E'))
        {
            use_raw_ucs = ! use_raw_ucs;
            init_trans(use_bullet_for_dots);
            my_clear();
#if HAVE_CURS_SET
            if (curs_set(0) -0 == ERR)
            {
                leaveok(stdscr, FALSE);
            }
#endif
            clearok(curscr, TRUE);
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            return 1;
        } else if ((k == 't') || (k == 'T'))
        {
            snapshot_start();
            return 1;
        } else if ((k == 'p') || (k == 'P') || (k == 27)) {
            if (paused)
            {
                DIRTY_ALL();
            }
            paused = ! paused;
            ignore_delay = 1;
            frameskip = 0;
            continue;
        } else if ((k == ',') || (k == '<')) {
            if (reflect ? (IS_LEFT_ARROW(key_buffer) || IS_RIGHT_ARROW(key_buffer)) : (IS_UP_ARROW(key_buffer) || IS_DOWN_ARROW(key_buffer)))
            {
                if (reflect ? ((hero_dir != MYMAN_LEFT) && (hero_dir != MYMAN_RIGHT)) : ((hero_dir != MYMAN_UP) && (hero_dir != MYMAN_DOWN)))
                {
                    key_buffer = ERR;
                }
            }
        } else if ((k == '.') || (k == '>')) {
            if (reflect ? (IS_UP_ARROW(key_buffer) || IS_DOWN_ARROW(key_buffer)) : (IS_LEFT_ARROW(key_buffer) || IS_RIGHT_ARROW(key_buffer)))
            {
                if (reflect ? ((hero_dir != MYMAN_UP) && (hero_dir != MYMAN_DOWN)) : ((hero_dir != MYMAN_LEFT) && (hero_dir != MYMAN_RIGHT)))
                {
                    key_buffer = ERR;
                }
            }
        } else if ((k == 'w') || (k == 'W')) {
            dots = total_dots[maze_level] - 1;
            continue;
        } else if ((k == 'd') || (k == 'D')) {
            debug = ! debug;
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            continue;
        } else if ((reflect ? IS_UP_ARROW(((k == ERR) ? key_buffer : k)) : IS_LEFT_ARROW(((k == ERR) ? key_buffer : k)))
                   && hero_can_move_left)
        {
            if (! (winning || dying || (dead && ! ghost_eaten_timer)))
            {
                hero_dir = MYMAN_LEFT;
                sprite_register[HERO] = SPRITE_HERO + 4;
            }
        } else if ((reflect ? IS_DOWN_ARROW(((k == ERR) ? key_buffer : k)) : IS_RIGHT_ARROW(((k == ERR) ? key_buffer : k)))
                   && hero_can_move_right)
        {
            if (! (winning || dying || (dead && ! ghost_eaten_timer)))
            {
                hero_dir = MYMAN_RIGHT;
                sprite_register[HERO] = SPRITE_HERO + 12;
            }
        } else if ((reflect ? IS_LEFT_ARROW(((k == ERR) ? key_buffer : k)) : IS_UP_ARROW(((k == ERR) ? key_buffer : k)))
                   && hero_can_move_up)
        {
            if (! (winning || dying || (dead && ! ghost_eaten_timer)))
            {
                hero_dir = MYMAN_UP;
                sprite_register[HERO] = SPRITE_HERO;
            }
        } else if ((reflect ? IS_RIGHT_ARROW(((k == ERR) ? key_buffer : k)) : IS_DOWN_ARROW(((k == ERR) ? key_buffer : k)))
                   && hero_can_move_down)
        {
            if (! (winning || dying || (dead && ! ghost_eaten_timer)))
            {
                hero_dir = MYMAN_DOWN;
                sprite_register[HERO] = SPRITE_HERO + 16;
            }
        }
        if (k == ERR)
        {
            k = key_buffer;
        }
        else if (! ignore_delay)
        {
            if (paused)
            {
                DIRTY_ALL();
                ignore_delay = 1;
                frameskip = 0;
            }
            paused = 0;
            key_buffer = k;
            continue;
        }
        break;
    }
    return (k == ERR) ? -1 : -2;
}

#if MY_CURSES_ARGS
static int myman_argc = 0;
static char** myman_argv = NULL;
#endif

static void
myman(void)
{
#ifdef SLANG_VERSION
#if SLANG_VERSION >= 20000
    if ((! myman_getenv("SLANG_USE_UTF8")) ||
        (! *(myman_getenv("SLANG_USE_UTF8"))))
    {
        SLutf8_enable(-1);
        SLtt_utf8_enable(1);
        SLsmg_utf8_enable(1);
        SLinterp_utf8_enable(1);
    }
    else
    {
        SLutf8_enable(atoi(myman_getenv("SLANG_USE_UTF8")));
    }
#endif
#endif
    do
    {
#if USE_SDL_MIXER
        SDL_Init(SDL_INIT_EVERYTHING);
        if ((! sdl_audio_open) &&
            (! Mix_OpenAudio(44100, AUDIO_S16, 1, 4096)))
        {
            sdl_audio_open = 1;
        }
#endif
        if (! myman_lines) myman_lines = (reflect ? (maze_maxwidth * gfx_w) : (maze_maxheight * gfx_h)) + (3 * tile_h + sprite_h);
        if (! myman_columns) myman_columns = (reflect ? (maze_maxheight * gfx_h) : (maze_maxwidth * gfx_w)) * (use_fullwidth ? 2 : 1);
#ifdef GTKCURSES
        if (((! myman_getenv("GTKCURSES_ICON")) || ! *(myman_getenv("GTKCURSES_ICON"))) && MYMANICONPNG && *MYMANICONPNG)
        {
            myman_setenv("GTKCURSES_ICON", MYMANICONPNG);
        }
#endif
#ifdef SLANG_VERSION
        /* ths works around the horribly broken TERMCAP values
         * produced by truncation to accomodate classic curses' (tiny)
         * fixed-length buffers without overrun checks. */
        if ((! myman_getenv("SLANG_USE_TERMCAP")) ||
            (! *(myman_getenv("SLANG_USE_TERMCAP"))) ||
            (! atoi(myman_getenv("SLANG_USE_TERMCAP"))))
        {
            SLtt_Try_Termcap = 0;
        }
#endif
#ifdef INITSCR_WITH_HINTS
        {
            const char *myman_title_fmt = "MyMan [%s %s]";
            char *myman_title_str = NULL;

            myman_title_str = (char *) malloc(strlen(myman_title_fmt) + strlen(MYMAN) + strlen(MYMANVERSION) + 1);

            if (myman_title_str != NULL)
            {
                sprintf(myman_title_str, myman_title_fmt, MYMAN, MYMANVERSION);
            }
            if (! initscrWithHints(
                    myman_lines,
                    myman_columns,
                    (myman_title_str != NULL) ? myman_title_str : MYMAN,
                    MYMAN))
            {
                perror("initscrWithHints");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            if (myman_title_str != NULL)
            {
                free((void *) myman_title_str);
            }
        }
#else /* ! defined(INITSCR_WITH_HINTS) */
#ifdef INITSCR_WITH_HINTS_AND_ARGS
        if (! initscrWithHintsAndArgs(
                myman_lines,
                myman_columns,
                "MyMan [" MYMAN " " MYMANVERSION "]",
                MYMAN,
                myman_argc,
                myman_argv))
        {
            perror("initscrWithHintsAndArgs");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
#else /* ! defined(INITSCR_WITH_HINTS_AND_ARGS) */
#if defined(SLCURSES) || defined(__PDCURSES__)
        if (! reinit_requested)
#endif
        {
#ifdef XCURSES
            if (! Xinitscr(myman_argc, myman_argv))
            {
                perror("Xinitscr");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
#else /* ! defined(XCURSES) */
            if (! initscr())
            {
                perror("initscr");
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
#endif /* ! defined(XCURSES) */
#ifdef __PDCURSES__
#ifdef PDC_BUILD
#if PDC_BUILD >= 2400
            PDC_set_title("MyMan [" MYMAN " " MYMANVERSION "]");
#endif
#endif /* defined(PDC_BUILD) */

#if USE_TERM_H
            if (fold_upper_case_to_lower_case == -1)
            {
                fold_upper_case_to_lower_case = tigetflag("OTLC");
            }
            if (fold_lower_case_to_upper_case == -1)
            {
                fold_lower_case_to_upper_case = tigetflag("OTUC");
            }
#endif
            if (use_transparency)
            {
                use_default_colors();
            }
            raw_output(TRUE);
#else /* ! defined(__PDCURSES__) */
#ifdef NCURSES_VERSION
            if (use_transparency)
            {
                use_default_colors();
            }
#endif
#endif /* ! defined(__PDCURSES__) */
        }
#endif /* ! defined(INITSCR_WITH_HINTS_AND_ARGS) */
#endif /* ! defined(INITSCR_WITH_HINTS) */
        my_clear();
        cbreak();
        noecho();
        nonl();
#if HAVE_NODELAY
        nodelay(stdscr, TRUE);
#endif
        intrflush(stdscr, FALSE);
        my_choose_pen(MY_PEN_NORMAL);
        leaveok(stdscr, TRUE);
#if HAVE_CURS_SET
        if (curs_set(0) -0 == ERR)
        {
            leaveok(stdscr, FALSE);
        }
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
        if (! use_bullet_for_dots_p)
        {
            use_bullet_for_dots = use_basic_acs && SWAPDOTS;
        }
        if (! use_dim_and_bright_p)
        {
            use_dim_and_bright = USE_DIM_AND_BRIGHT;
        }
        if (! use_acs_p)
        {
            use_acs = USE_ACS;
        }
        init_trans(use_bullet_for_dots);
#if USE_COLOR
#if COLORIZE
        if (! use_color_p)
        {
            use_color = has_colors();
            use_color_p = 1;
        }
#endif
#ifdef SLCURSES
        if (! reinit_requested)
#endif
        {
            start_color();
        }
#endif
        if (! use_color_p)
        {
            use_color = use_false_color;
            use_color_p = 1;
        }
        if (use_color)
            init_pen();
#if USE_SIGWINCH
        old_sigwinch_handler = signal(SIGWINCH, sigwinch_handler);
#endif
        reinit_requested = 0;
        pager();
        old_lines = 0;
        old_cols = 0;
        old_score = 0;
        old_showlives = 0;
        old_level = 0;
        while (! reinit_requested)
        {
            if (! gamecycle(LINES, COLS))
            {
                break;
            }
        }
#if USE_SIGWINCH
        if (old_sigwinch_handler) signal(SIGWINCH, old_sigwinch_handler);
        else signal(SIGWINCH, SIG_DFL);
#endif
        my_choose_pen(MY_PEN_NORMAL);
#if HAVE_CURS_SET
        curs_set(1); /* slcurses doesn't do this in endwin() */
#endif
        my_clear();
        if (use_color)
        {
            standout();
            mvprintw(LINES ? 1 : 0, 0, " ");
            standend();
            refresh();
            destroy_pen();
            mvprintw(LINES ? 1 : 0, 0, " ");
            addch('\n');
        }
        refresh();
        echo();
        endwin();
        if (reinit_requested)
        {
            refresh();
            if (use_resize)
            {
#ifdef SLCURSES
                SLtt_get_screen_size();
                SLsmg_reinit_smg();
                delwin(stdscr);
                stdscr = newwin(0, 0, 0, 0);
#endif
#if USE_IOCTL
                if (use_ioctl)
                {
#ifdef TIOCGWINSZ
                    struct winsize wsz;
#endif
#ifdef TIOCGSIZE
                    struct ttysize tsz;
#endif
                    int widthmul;

                    widthmul = 1;
                    if (ever_got_sigwinch)
                    {
                        const char *termType;

                        termType = myman_getenv("TERM");
                        if (termType && MYMAN__HASFLAG(termType, "dwl"))
                        {
                            widthmul = 2;
                        }
                    }
#ifdef TIOCGWINSZ
                    if (! ioctl(fileno(stdout), TIOCGWINSZ, &wsz))
                    {
                        myman_lines = wsz.ws_row;
                        myman_columns = wsz.ws_col / widthmul;
                    }
                    else
#endif
#ifdef TIOCGSIZE
                        if (! ioctl(fileno(stdout), TIOCGSIZE, &tsz))
                        {
                            myman_lines = tsz.ts_lines;
                            myman_columns = tsz.ts_cols / widthmul;
                        }
                        else
#endif
                        {
                            myman_lines = LINES;
                            myman_columns = COLS;
                        }
                }
#endif /* USE_IOCTL */
                if (! myman_lines) myman_lines = LINES;
                if (! myman_columns) myman_columns = COLS;
#ifdef KEY_RESIZE
#ifdef __PDCURSES__
                resize_term(0, 0);
#else
                resizeterm(myman_lines ? myman_lines : LINES, myman_columns ? myman_columns : COLS);
#endif
#else
                {
                    static char buf[32];

                    sprintf(buf, "%d", myman_lines);
                    myman_setenv("LINES", buf);
                    sprintf(buf, "%d", myman_columns);
                    myman_setenv("COLUMNS", buf);
                }
#endif
            }
        }
    } while (reinit_requested);
    fprintf(stderr, "%s: scored %d points\n",
            progname, score);
#ifdef XCURSES
    XCursesExit();
#endif
#if USE_ICONV
    if (cd_to_wchar != (iconv_t) -1)
    {
        iconv_close(cd_to_wchar);
        cd_to_wchar = (iconv_t) -1;
    }
    if (cd_to_uni != (iconv_t) -1)
    {
        iconv_close(cd_to_uni);
        cd_to_uni = (iconv_t) -1;
    }
#endif
}

void
usage(const char *mazefile, const char *spritefile, const char *tilefile)
{
    printf("Usage: %s [-h] [options]", progname);
    printf(MY_CURSES_USAGE
           MY_CURSES_USAGE_PARAMETERS(stdout));
    puts("");
    puts("-h \tdisplay this help and exit");
    puts("-b \tenable sounds");
    puts("-q \tdisable sounds");
    puts("-c \tenable color support");
    puts("-n \tdisable color support");
    puts("-B \tuse dim and bright attributes for missing colors");
    puts("-N \tdon't use dim and bold attributes for extra colors");
    puts("-o \tuse 'o' for power pellets and bullet for dots");
    puts("-p \tuse bullet for power pellets and '.' for dots");
#if MYMANDELAY
    puts("-d NUM \tdelay NUM microseconds/refresh");
#else
    puts("-d NUM \tdelay NUM microseconds/refresh (must recompile first)");
#endif
    puts("-D NAME \tdefine environment variable NAME with value 1");
    puts("-D NAME=VALUE \tdefine environment variable NAME with value VALUE");
    puts("-D {N1[=V1],N2[=V2][,...]} \tdefine multiple environment variables");
    puts("-g NUM \tplay against NUM monsters");
    puts("-l NUM \tstart with NUM lives");
#if USE_ATTR
    puts("-u \tuse the underline attribute for maze walls");
#else
    puts("-u \tuse the underline attribute for maze walls (must recompile first)");
#endif
    puts("-U \tdon't use the underline attribute for maze walls");
    puts("-r \tuse raw tile characters (CP437 or UCS/Unicode character graphics)");
    puts("-R \tuse altcharset translations (VT100-style graphics)");
    puts("-e \tuse UCS/Unicode for internal representation of raw tile characters");
    puts("-E \tuse CP437 for internal representation of raw tile characters");
    puts("-a \tuse ASCII for altcharset translation");
    puts("-A \tuse your terminal's altcharset translations");
    puts("-i \tscroll vertically by redrawing the screen");
    puts("-I \tuse your terminal's vertical scrolling capabilities");
    puts("-1 \tuse default UCS/Unicode mapping");
    puts("-2 \tuse CJK (fullwidth) UCS/Unicode mapping");
    puts("-m FILE \tuse the maze in FILE");
    puts("-s FILE \tuse sprites from FILE");
    puts("-t FILE \tuse tiles from FILE");
    puts("-M \twrite the maze to stdout and exit");
    puts("-S \twrite sprites to stdout and exit");
    puts("-T \twrite tiles to stdout and exit");
    puts("-Q \twrite output in text format (default: C)");
    puts("-f FILE \tredirect stdout to FILE (append)");
    puts("-F FILE \tredirect stdout to FILE (truncate)");
    puts("-x \treflect maze diagonally, exchanging the upper right and lower left corners");
    puts("-X \tdo not reflect maze");
#ifdef MY_CURSES_HELP
    printf(MY_CURSES_HELP(stdout, progname));
#endif
    printf("Defaults:");
    printf(" %s", use_raw ? "-r" : "-R");
    printf(" %s", use_raw_ucs ? "-e" : "-E");
    if (use_acs_p)
    {
        printf(" %s", use_acs ? "-A" : "-a");
    }
    printf(" %s", use_idlok ? "-I" : "-i");
    printf(" %s", use_sound ? "-b" : "-q");
    printf(" %s", use_color ? "-c" : "-n");
    if (use_dim_and_bright_p)
    {
        printf(" %s", use_dim_and_bright ? "-B" : "-N");
    }
    if (use_bullet_for_dots_p)
    {
        printf(" %s", use_bullet_for_dots ? "-o" : "-p");
    }
    printf(" %s", use_underline ? "-u" : "-U");
    printf(" %s", use_fullwidth ? "-2" : "-1");
    printf(" %s", reflect ? "-x" : "-X");
    printf(" -d %lu -l %d -m \"",
           mymandelay ? mymandelay : 0,
           lives);
    if (mazefile)
        mymanescape(mazefile, strlen(mazefile));
    else {
        printf("(");
        mymanescape(maze_s, strlen(maze_s));
        printf(")");
    }
    printf("\" -s \"");
    if (spritefile)
        mymanescape(spritefile, strlen(spritefile));
    else {
        printf("(");
        mymanescape(sprite_s, strlen(sprite_s));
        printf(")");
    }
    printf("\" -t \"");
    if (tilefile)
        mymanescape(tilefile, strlen(tilefile));
    else {
        printf("(");
        mymanescape(tile_s, strlen(tile_s));
        printf(")");
    }
    printf("\"\n");
}

static void myman_summary(FILE *stream, const char *progname)
{
    fprintf(stream, "Usage: %s [-h] [options]", progname);
    fprintf(stream, MY_CURSES_USAGE
            MY_CURSES_USAGE_PARAMETERS(stream));
    fprintf(stream, "\n");
}

static void
parse_myman_args(int argc, char **argv)
{
    int i, n;
    int
        dump_maze = 0,
        dump_sprite = 0,
        dump_tile = 0,
        dump_text = 0;
    const char *tilefile = TILEFILE;
    const char *spritefile = SPRITEFILE;
    const char *mazefile = MAZEFILE;
    int option_index;
    const char *defvariant = MYMANVARIANT;
    const char *defsize = MYMANSIZE;
    my_ulong uli;

    while ((i = getopt_long(argc, argv, short_options,
                            long_options, &option_index))
           != -1)
        switch (i) {
        case 'V':
            printf("%s-%s (%s) %s\n%s\n",
                   MYMANVARIANT,
                   MYMANSIZE,
                   MYMAN,
                   MYMANVERSION,
                   MYMANCOPYRIGHT);
            fflush(stdout), fflush(stderr), exit(EXIT_SUCCESS);
        case 'v':
            defvariant = optarg;
            break;
        case 'z':
            defsize = optarg;
            break;
        case 'b':
            use_sound = 1;
            break;
        case 'q':
            use_sound = 0;
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
            use_acs_p = 1;
            break;
        case 'A':
            use_acs = 1;
            use_acs_p = 1;
            break;
        case 'x':
            reflect = 1;
            break;
        case 'X':
            reflect = 0;
            break;
        case 'd':
        {
            char garbage;

            if (sscanf(optarg, "%lu%c", &uli, &garbage) != 1)
            {
                fprintf(stderr,
                        "%s: argument to -d must be an my_ulong integer.\n",
                        progname);
                fflush(stderr), exit(EXIT_FAILURE);
            }
            mymandelay = uli;
            mindelay = mymandelay / 2;
            break;
        }
        case 'D':
        {
            if (myman_super_setenv(optarg))
            {
                perror("setenv");
                fflush(stderr), exit(EXIT_FAILURE);
            }
            break;
        }
        case 'g':
        {
            const char *tmp_ghosts_endp = NULL;

            maze_GHOSTS = strtollist(optarg, &tmp_ghosts_endp, &maze_GHOSTS_len);
            if (! maze_GHOSTS)
            {
                perror("-g");
                fflush(stderr), exit(EXIT_FAILURE);
            }
            else if (tmp_ghosts_endp && *tmp_ghosts_endp)
            {
                fprintf(stderr,
                        "%s: -g: garbage after argument: %s\n",
                        progname,
                        tmp_ghosts_endp);
                fflush(stderr), exit(EXIT_FAILURE);
            }
            ghosts_p = 1;
            break;
        }
        case 'l':
        {
            char garbage;

            if (sscanf(optarg, "%lu%c", &uli, &garbage) != 1)
            {
                fprintf(stderr,
                        "%s: argument to -l must be an unsigned integer.\n",
                        progname);
                fflush(stderr), exit(EXIT_FAILURE);
            }
            lives = (int) uli;
            break;
        }
        case 'h':
            usage(mazefile, spritefile, tilefile);
            fflush(stdout), fflush(stderr), exit(EXIT_SUCCESS);
            break;
        case 'k':
            if (1) {
                int iik;

                for (iik = 0; myman_keys_strs[iik] != NULL; ++ iik)
                {
                    printf("%s", myman_keys_strs[iik]);
                }
            }
            fflush(stdout), fflush(stderr), exit(EXIT_SUCCESS);
            break;
        case 'L':
            if (1) {
                int iiln;

                for (iiln = 0; myman_legal_notice_strs[iiln] != NULL; ++ iiln)
                {
                    printf("%s", myman_legal_notice_strs[iiln]);
                }
            }
            fflush(stdout), fflush(stderr), exit(EXIT_SUCCESS);
            break;
        case 'u':
            use_underline = 1;
            break;
        case 'U':
            use_underline = 0;
            break;
        case 'M':
            dump_maze = 1;
            nogame = 1;
            break;
        case 'Q':
            dump_text = 1;
            break;
        case 'S':
            dump_sprite = 1;
            nogame = 1;
            break;
        case 'T':
            dump_tile = 1;
            nogame = 1;
            break;
        case 'm':
#ifdef BUILTIN_MAZE
            if ((*optarg == '(') &&
                (strlen(optarg) == (strlen(maze_s) + 2)) &&
                (optarg[strlen(optarg) - 1] == ')') &&
                ! strncmp(optarg + 1, maze_s, strlen(maze_s))) {
                mazefile = 0;
                break;
            }
#endif
            mazefile = optarg;
            break;
        case 'n':
            use_color_p = 1;
            use_color = 0;
            break;
        case 'o':
            use_bullet_for_dots = 1;
            use_bullet_for_dots_p = 1;
            break;
        case 'p':
            use_bullet_for_dots = 0;
            use_bullet_for_dots_p = 1;
            break;
        case '2':
            use_fullwidth = 1;
            break;
        case '1':
            use_fullwidth = 0;
            break;
        case 'c':
            use_color_p = 1;
            use_color = 1;
            break;
        case 'B':
            use_dim_and_bright = 1;
            use_dim_and_bright_p = 1;
            break;
        case 'N':
            use_dim_and_bright = 0;
            use_dim_and_bright_p = 1;
            break;
        case 't':
#ifdef BUILTIN_TILE
            if ((*optarg == '(') &&
                (strlen(optarg) == (strlen(tile_s) + 2)) &&
                (optarg[strlen(optarg) - 1] == ')') &&
                ! strncmp(optarg + 1, tile_s, strlen(tile_s))) {
                tilefile = 0;
                break;
            }
#endif
            tilefile = optarg;
            break;
        case 's':
#ifdef BUILTIN_SPRITE
            if ((*optarg == '(') &&
                (strlen(optarg) == (strlen(sprite_s) + 2)) &&
                (optarg[strlen(optarg) - 1] == ')') &&
                ! strncmp(optarg + 1, sprite_s, strlen(sprite_s))) {
                spritefile = 0;
                break;
            }
#endif
            spritefile = optarg;
            break;
        case 'f':
            if (freopen(optarg, "a", stdout) == NULL)
            {
                perror(optarg);
                fflush(stderr), exit(EXIT_FAILURE);
            }
            break;
        case 'F':
            if (freopen(optarg, "w", stdout) == NULL)
            {
                perror(optarg);
                fflush(stderr), exit(EXIT_FAILURE);
            }
            break;
        default:
            myman_summary(stderr, progname);
            fflush(stderr), exit(MYMAN_EXIT_USAGE);
        }
    use_fixedpal = 0;
    if (myman_getenv("MYMAN_FIXEDPAL") && *(myman_getenv("MYMAN_FIXEDPAL")) && atoi(myman_getenv("MYMAN_FIXEDPAL")))
    {
        use_fixedpal = 1;
    }
    fold_lower_case_to_upper_case = -1;
    if (myman_getenv("MYMAN_UC") && *(myman_getenv("MYMAN_UC")))
    {
        fold_lower_case_to_upper_case = !!atoi(myman_getenv("MYMAN_UC"));
    }
    fold_upper_case_to_lower_case = -1;
    if (myman_getenv("MYMAN_LC") && *(myman_getenv("MYMAN_LC")))
    {
        fold_upper_case_to_lower_case = !!atoi(myman_getenv("MYMAN_LC"));
    }
    if (myman_getenv("MYMAN_XCASE") && *(myman_getenv("MYMAN_XCASE")))
    {
        if (fold_upper_case_to_lower_case == -1)
        {
            fold_upper_case_to_lower_case = !!atoi(myman_getenv("MYMAN_XCASE"));
        }
        if (fold_lower_case_to_upper_case == -1)
        {
            fold_lower_case_to_upper_case = !!atoi(myman_getenv("MYMAN_XCASE"));
        }
    }
#if USE_COLOR
    terminal_color_scale = 1000;
    if (myman_getenv("MYMAN_COLOR_SCALE") && *(myman_getenv("MYMAN_COLOR_SCALE")) && (atoi(myman_getenv("MYMAN_COLOR_SCALE")) > 0))
    {
        terminal_color_scale = atoi(myman_getenv("MYMAN_COLOR_SCALE"));
    }
    terminal_colors = -1;
    if (myman_getenv("MYMAN_COLORS") && *(myman_getenv("MYMAN_COLORS")) && (atoi(myman_getenv("MYMAN_COLORS")) >= 0))
    {
        terminal_colors = atoi(myman_getenv("MYMAN_COLORS"));
    }
    terminal_color_pairs = -1;
    if (myman_getenv("MYMAN_COLOR_PAIRS") && *(myman_getenv("MYMAN_COLOR_PAIRS")) && (atoi(myman_getenv("MYMAN_COLOR_PAIRS")) >= 0))
    {
        terminal_color_pairs = atoi(myman_getenv("MYMAN_COLOR_PAIRS"));
    }
#endif
    use_debug_input = 0;
    if (myman_getenv("MYMAN_DEBUG_INPUT") && *(myman_getenv("MYMAN_DEBUG_INPUT")) && atoi(myman_getenv("MYMAN_DEBUG_INPUT")))
    {
        use_debug_input = 1;
    }
    if (myman_getenv("MYMAN_DEBUG") && *(myman_getenv("MYMAN_DEBUG")) && strcmp(myman_getenv("MYMAN_DEBUG"), "0"))
    {
        debug = atoi(myman_getenv("MYMAN_DEBUG"));
        debug = debug ? debug : 1;
    }
    if (myman_getenv("MYMAN_REVBLOCK") && *(myman_getenv("MYMAN_REVBLOCK")))
    {
        use_revblock = atoi(myman_getenv("MYMAN_REVBLOCK"));
    }
    if (myman_getenv("MYMAN_BGSPACE") && *(myman_getenv("MYMAN_BGSPACE")))
    {
        use_bgspace = atoi(myman_getenv("MYMAN_BGSPACE"));
    }
    if (myman_getenv("MYMAN_FGBLOCK") && *(myman_getenv("MYMAN_FGBLOCK")))
    {
        use_fgblock = atoi(myman_getenv("MYMAN_FGBLOCK"));
    }
    if (myman_getenv("MYMAN_DIRECT") && *(myman_getenv("MYMAN_DIRECT")))
    {
        use_direct = atoi(myman_getenv("MYMAN_DIRECT"));
    }
    if (myman_getenv("MYMAN_ACS") && *(myman_getenv("MYMAN_ACS")))
    {
        use_basic_acs = atoi(myman_getenv("MYMAN_ACS"));
    }
    else
    {
        use_basic_acs = 1;
    }
    if (myman_getenv("MYMAN_EXTRA_ACS") && *(myman_getenv("MYMAN_EXTRA_ACS")))
    {
        use_extra_acs = atoi(myman_getenv("MYMAN_EXTRA_ACS"));
    }
    else
    {
        use_extra_acs = use_basic_acs && locale_is_utf8();
    }
    if (myman_getenv("MYMAN_ROUNDED") && *(myman_getenv("MYMAN_ROUNDED")))
    {
        use_rounded = atoi(myman_getenv("MYMAN_ROUNDED"));
    }
    else
    {
        use_rounded = 0;
    }
    if (myman_getenv("MYMAN_DOUBLE") && *(myman_getenv("MYMAN_DOUBLE")))
    {
        use_double = atoi(myman_getenv("MYMAN_DOUBLE"));
    }
    else
    {
        use_double = ! use_fullwidth;
    }
    if (myman_getenv("MYMAN_SINGLE") && *(myman_getenv("MYMAN_SINGLE")))
    {
        use_single = atoi(myman_getenv("MYMAN_SINGLE"));
    }
    else
    {
        use_single = 0;
    }
    if (myman_getenv("MYMAN_BITMAP") && *(myman_getenv("MYMAN_BITMAP")))
    {
        use_bitmap = atoi(myman_getenv("MYMAN_BITMAP"));
    }
    else
    {
        use_bitmap = 0;
    }
#if USE_COLOR
    if (myman_getenv("MYMAN_SWAP_RB") && *(myman_getenv("MYMAN_SWAP_RB")))
    {
        swap_rb = atoi(myman_getenv("MYMAN_SWAP_RB"));
    }
#endif
    if (myman_getenv("MYMAN_FALSE_COLOR") && *(myman_getenv("MYMAN_FALSE_COLOR")))
    {
        use_false_color = atoi(myman_getenv("MYMAN_FALSE_COLOR"));
    }
    if (myman_getenv("MYMAN_VGA_PALETTE") && *(myman_getenv("MYMAN_VGA_PALETTE")))
    {
        use_vga_palette = atoi(myman_getenv("MYMAN_VGA_PALETTE"));
    }
    if (myman_getenv("MYMAN_ANSI_PALETTE") && *(myman_getenv("MYMAN_ANSI_PALETTE")))
    {
        use_ansi_palette = atoi(myman_getenv("MYMAN_ANSI_PALETTE"));
    }
    if (myman_getenv("MYMAN_REVBG") && *(myman_getenv("MYMAN_REVBG")))
    {
        use_revbg = atoi(myman_getenv("MYMAN_REVBG"));
    }
    if (myman_getenv("MYMAN_ALTCOLOR") && *(myman_getenv("MYMAN_ALTCOLOR")))
    {
        use_altcolor = atoi(myman_getenv("MYMAN_ALTCOLOR"));
    }
    if (myman_getenv("MYMAN_WCWIDTH") && *(myman_getenv("MYMAN_WCWIDTH")))
    {
        terminal_wcwidth = atoi(myman_getenv("MYMAN_WCWIDTH"));
    }
    if (myman_getenv("MYMAN_IOCTL") && *(myman_getenv("MYMAN_IOCTL")))
    {
        use_ioctl = atoi(myman_getenv("MYMAN_IOCTL"));
    }
    if (myman_getenv("MYMAN_RESIZE") && *(myman_getenv("MYMAN_RESIZE")))
    {
        use_resize = atoi(myman_getenv("MYMAN_RESIZE"));
    }
    if (myman_getenv("MYMAN_TRANSPARENT") && *(myman_getenv("MYMAN_TRANSPARENT")))
    {
        use_transparency = atoi(myman_getenv("MYMAN_TRANSPARENT"));
    }
    if (myman_getenv("MYMAN_LEGAL_NOTICE") && *(myman_getenv("MYMAN_LEGAL_NOTICE")))
    {
        if ((! mystrcasecmp(myman_getenv("MYMAN_LEGAL_NOTICE"), "I acknowledge the MyMan LEGAL NOTICE"))
            ||
            (! mystrcasecmp(myman_getenv("MYMAN_LEGAL_NOTICE"), "I-acknowledge-the-MyMan-LEGAL-NOTICE"))
            ||
            (! mystrcasecmp(myman_getenv("MYMAN_LEGAL_NOTICE"), "I_acknowledge_the_MyMan_LEGAL_NOTICE")))
        {
            pager_notice = 0;
        }
    }
#ifdef XCURSES
    argv[optind - 1] = (char *) progname;
    argc -= optind;
    argc ++;
    argv += optind;
    argv --;
#else
    if (optind < argc)
    {
        myman_summary(stderr, progname);
        fflush(stderr), exit(MYMAN_EXIT_USAGE);
    }
#endif
    if (strcmp(defvariant, MYMANVARIANT))
    {
        fprintf(stderr,
                "%s: game variant `%s' not included in this binary\n",
                progname,
                defvariant);
        exit(MYMAN_EXIT_USAGE);
    }
    if (strcmp(defsize, MYMANSIZE))
    {
        fprintf(stderr,
                "%s: game size `%s' not included in this binary\n",
                progname,
                defsize);
        exit(MYMAN_EXIT_USAGE);
    }

#if ! USE_ATTR
    if (use_underline)
    {
        fprintf(stderr,
                "%s: compile with -DUSE_ATTR=1 to enable the -u option.\n",
                progname);
        fflush(stderr), exit(EXIT_FAILURE);
    }
#endif
    if ((tilefile && readfont(tilefile, &tile_w, &tile_h, tile_d, tile_u, &tile_f, tile_c, &tile_a)) ||
        (spritefile && readfont(spritefile, &sprite_w, &sprite_h, sprite_d, sprite_u, &sprite_f, sprite_c, &sprite_a)))
        exit(EXIT_FAILURE);

    if (tile_a)
        if (parse_tile_a(tilefile ? tilefile : tile_s,
                            tile_a))
        {
            exit(EXIT_FAILURE);
        }

    if (sprite_a)
        if (parse_sprite_a(spritefile ? spritefile : sprite_s,
                              sprite_a))
        {
            exit(EXIT_FAILURE);
        }

    gfx_reflect = reflect && ! REFLECT_LARGE;

#if ! MYMANDELAY
    if (mymandelay)
    {
        fprintf(stderr,
                "%s: compile with -DMYMANDELAY=1 to enable the -d option.\n",
                progname);
        fflush(stderr), exit(EXIT_FAILURE);
    }
#else
    mymandelay = mymandelay / MYMANFIFTH;
    mindelay = mymandelay / 2;
#endif

#ifdef BUILTIN_MAZE
    if (! mazefile)
    {
        mazes = (char **) malloc(maze_n * sizeof(*mazes));
        if (! mazes)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset((void *) mazes, 0, maze_n * sizeof(*mazes));
        maze_colors = (char **) malloc(maze_n * sizeof(*maze_colors));
        if (! maze_colors)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset((void *) maze_colors, 0, maze_n * sizeof(*maze_colors));
        for (n = 0; n < maze_n; ++n)
        {
            mazes[n] = (char *) malloc(maze_h[n] * (maze_w[n] + 1) * sizeof(**mazes));
            if (! mazes[n])
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            memcpy((void *) mazes[n],
                   (void *) maze_d[n],
                   maze_h[n] * (maze_w[n] + 1) * sizeof(**mazes));
            maze_colors[n] = (char *) malloc(maze_h[n] * (maze_w[n] + 1) * sizeof(**maze_colors));
            if (! maze_colors[n])
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            memcpy((void *) maze_colors[n],
                   (void *) maze_c[n],
                   maze_h[n] * (maze_w[n] + 1) * sizeof(**maze_colors));
        }
    }
#endif /* defined(BUILTIN_MAZE) */
    if (mazefile && readmaze(mazefile, &maze_n, &maze_w, &maze_h, &mazes, &maze_f, &maze_colors, &maze_a))
    {
        exit(EXIT_FAILURE);
    }
    dirheros = (int *) malloc(maze_n * sizeof(*dirheros));
    if (! dirheros)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    dirheros[0] = DIRHERO;
    maze_maxwidth = maze_w[0];
    maze_maxheight = maze_h[0];
    for (n = 1; n < maze_n; ++ n)
    {
        dirheros[n] = DIRHERO;
        maze_maxwidth = MAX(maze_w[n], maze_maxwidth);
        maze_maxheight = MAX(maze_h[n], maze_maxheight);
    }
    if (maze_a)
        if (parse_maze_a(mazefile ? mazefile : maze_s,
                            maze_a))
        {
            exit(EXIT_FAILURE);
        }

    msglen = MAX(MAX(strlen(msg_PLAYER1), strlen(msg_PLAYER2)), MAX(strlen(msg_READY), strlen(msg_GAMEOVER)));
    total_dots = (int *) malloc(maze_n * sizeof(*total_dots));
    if (! total_dots)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) total_dots, 0, maze_n * sizeof(*total_dots));
    pellets = (int *) malloc(maze_n * sizeof(*pellets));
    if (! pellets)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) pellets, 0, maze_n * sizeof(*pellets));
    dirty_cell = (my_uchar *) malloc(maze_maxheight * ((maze_maxwidth + 1 + 7) >> 3) * sizeof(*dirty_cell));
    if (! dirty_cell)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) dirty_cell, 0, maze_maxheight * ((maze_maxwidth + 1 + 7) >> 3) * sizeof(*dirty_cell));
    home_dir = (my_uchar *) malloc(MAXGHOSTS * maze_maxheight * (maze_maxwidth + 1) * sizeof(*home_dir));
    if (! home_dir)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) home_dir, 0, MAXGHOSTS * maze_maxheight * (maze_maxwidth + 1) * sizeof(*home_dir));
    blank_mazes = (char **) malloc(maze_n * sizeof(*blank_mazes));
    if (! blank_mazes)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) blank_mazes, 0, maze_n * sizeof(*blank_mazes));
    blank_maze_colors = (char **) malloc(maze_n * sizeof(*blank_maze_colors));
    if (! blank_maze_colors)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) blank_maze_colors, 0, maze_n * sizeof(*blank_maze_colors));
    inside_walls = (my_ushort **) malloc(maze_n * sizeof(*inside_walls));
    if (! inside_walls)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset((void *) inside_walls, 0, maze_n * sizeof(*inside_walls));
    for (n = 0; n < maze_n; ++ n)
    {
        blank_mazes[n] = (char *) malloc(maze_h[n] * (maze_w[n] + 1) * sizeof(**blank_mazes));
        if (! blank_mazes[n])
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset((void *) blank_mazes[n], 0, maze_h[n] * (maze_w[n] + 1) * sizeof(**blank_mazes));
        blank_maze_colors[n] = (char *) malloc(maze_h[n] * (maze_w[n] + 1) * sizeof(**blank_maze_colors));
        if (! blank_maze_colors[n])
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset((void *) blank_maze_colors[n], 0, maze_h[n] * (maze_w[n] + 1) * sizeof(**blank_maze_colors));
        inside_walls[n] = (my_ushort *) malloc(maze_h[n] * (maze_w[n] + 1) * sizeof(**inside_walls));
        if (! inside_walls[n])
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset((void *) inside_walls[n], 0, maze_h[n] * (maze_w[n] + 1) * sizeof(**inside_walls));
        memcpy((void *)blank_mazes[n],
               (void *)mazes[n],
               (maze_w[n] + 1) * maze_h[n] * sizeof(my_uchar));
        memcpy((void *)blank_maze_colors[n],
               (void *)maze_colors[n],
               (maze_w[n] + 1) * maze_h[n] * sizeof(my_uchar));
    }

    CLEAN_ALL();
#if USE_ISATTY
    {
        int fd;

#if ! HAVE_SAFE_FILENO
        if (argc == 0 || (argv[0][0] != '/'))
        {
            fd = 2;
        }
        else
#endif
        {
            fd = fileno(stderr);
        }
        paint_walls(isatty(fd));
    }
#else
    paint_walls(0);
#endif
    gamereset();

    if (dump_maze)
        writemaze(dump_text, mazefile ? mazefile : maze_s);

    if (dump_sprite)
        writefont(dump_text, spritefile ? spritefile : sprite_s,
                  "sprite", sprite_w, sprite_h, sprite_d, sprite_u, sprite_f, sprite_c, sprite_a);
    if (dump_tile)
        writefont(dump_text, tilefile ? tilefile : tile_s,
                  "tile", tile_w, tile_h, tile_d, tile_u, tile_f, tile_c, tile_a);
}

#if defined(__APPLE__) || defined(macintosh)

static char **my_mac_argv = NULL;

static void my_mac_cleanargs(void)
{
    if (my_mac_argv)
    {
        int i;

        for (i = 0; my_mac_argv[i]; ++i)
        {
            free((void *) my_mac_argv[i]);
            my_mac_argv[i] = NULL;
        }
        free((void *) my_mac_argv);
        my_mac_argv = NULL;
    }
}

void my_mac_fixargs(int *argcp, char ***argvp, const char* progname)
{
    static int has_run = 0;

    if (has_run)
    {
        if (my_mac_argv
            &&
            ((*argvp) == my_mac_argv)
            &&
            my_mac_argv[0]
            &&
            (! *(my_mac_argv[0])))
        {
            free((void *) my_mac_argv[0]);
            my_mac_argv[0] = strdup(progname);
            if (!my_mac_argv[0])
            {
                perror("strdup");
                fflush(stderr), exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        int argc;
        char **argv;

        has_run = 1;
        atexit(my_mac_cleanargs);
        argc = *argcp;
        argv = *argvp;
#if defined(MACCURSES) || defined(macintosh)
        {
            int have_command_line;

            have_command_line = 0;
#ifdef HAVE_COMMAND_LINE
            have_command_line = HAVE_COMMAND_LINE;
#endif
            if ((argc == 0)
                ||
                ((argc == 1) && (! have_command_line)))
            {
                Handle args, envs;

                /* Allow overriding argv[0 ... argc - 1] from resource
                 * 'STR#' (129). */
                args = GetResource(MYMAN_FOUR_CHAR_CODE("STR#"), 129);
                if (args && *args)
                {
                    short nargc;

                    HLock(args);
                    nargc = *(short *)(void *)*args;
                    if (nargc > 0)
                    {
                        my_mac_argv = (char **)malloc((nargc + 2) * sizeof(char*));
                        if (! my_mac_argv)
                        {
                            perror("malloc");
                            fflush(stderr), exit(EXIT_FAILURE);
                        }
                        else
                        {
                            int i;
                            size_t noff;

                            my_mac_argv[nargc] = NULL;
                            noff = 2;
                            for (i = 0; i < nargc; ++i)
                            {
                                char *narg;
                                unsigned len;

                                len = (unsigned)(my_uchar)(((char*)(void*)*args)[noff]);
                                narg = (char*)malloc(len + 1);
                                if (! narg)
                                {
                                    perror("malloc");
                                    fflush(stderr), exit(EXIT_FAILURE);
                                }
                                else
                                {
                                    narg[len] = '\0';
                                    memcpy((void *)narg, ((char*)(void*)*args) + noff + 1, len);
                                }
                                noff += len + 1;
                                my_mac_argv[i] = narg;
                            }
                            argv = my_mac_argv;
                            argc = nargc;
                        }
                    }
                    HUnlock(args);
                    ReleaseResource(args);
                }
                /* Allow augmenting the environment from resource 'STR#' (129). */
                envs = GetResource(MYMAN_FOUR_CHAR_CODE("STR#"), 128);
                if (envs && *envs)
                {
                    short nenvc;

                    HLock(envs);
                    nenvc = *(short *)(void *)*envs;
                    if (nenvc > 0)
                    {
                        int i;
                        size_t noff;

                        noff = 2;
                        for (i = 0; i < nenvc; ++i)
                        {
                            char *nenv;
                            unsigned len;

                            len = (unsigned)(my_uchar)(((char*)(void*)*envs)[noff]);
                            nenv = (char*)malloc(len + 1);
                            if (! nenv)
                            {
                                perror("malloc");
                                fflush(stderr), exit(EXIT_FAILURE);
                            }
                            else
                            {
                                const char* value;

                                value = "1";
                                nenv[len] = '\0';
                                memcpy((void *)nenv, ((char*)(void*)*envs) + noff + 1, len);
                                if (strchr(nenv, '='))
                                {
                                    *(strchr(nenv, '=')) = '\0';
                                    value = nenv + strlen(nenv) + 1;
                                }
                                myman_setenv(nenv, value);
                                free((void *)nenv);
                            }
                            noff += len + 1;
                        }
                    }
                    HUnlock(envs);
                    ReleaseResource(envs);
                }
            }
        }
#endif /* defined(MACCURSES) || defined(macintosh) */
        *argcp = argc;
        *argvp = argv;
    }
}

#endif /* defined(__APPLE__) || defined(macintosh) */

int
main(int argc, char *argv[]
#ifndef MAIN_NO_ENVP
     , char *envp[]
#endif
    )
{
    my_ulong *uni_cp437_rounded = NULL;
    int i;
    long c = 0;

#ifndef MAIN_NO_ENVP
    if (envp)
    {
        /* we should care */
    }
#endif
    progname = ((argc > 0) && argv[0]) ? argv[0] : "";
    if (1) {
        int iiln;
        size_t iisz;
        size_t lnsz;

        iisz = 1;
        for (iiln = 0; myman_legal_notice_strs[iiln] != NULL; ++ iiln)
        {
            iisz += strlen(myman_legal_notice_strs[iiln]);
        }
        tmp_notice = (char *) malloc(iisz);
        if (tmp_notice)
        {
            pager_notice = tmp_notice;
            iisz = 0;
            for (iiln = 0; myman_legal_notice_strs[iiln] != NULL; ++ iiln)
            {
                lnsz = strlen(myman_legal_notice_strs[iiln]);
                memcpy((void *) (tmp_notice + iisz), myman_legal_notice_strs[iiln], lnsz);
                iisz += lnsz;
            }
            tmp_notice[iisz] = '\0';
        }
    }
#ifdef __CARBON__
    /* when launched as a CFM application under Mac OS X, there is no
     * argv[0], so we jump through a few hoops to figure out what it
     * should have been. */
    if ((argc == 0)
        ||
        ((argc == 2) && (! strncmp(argv[1], "-psn_", strlen("-psn_")))))
    {
        ProcessSerialNumber psn;

        if (noErr == MacGetCurrentProcess(&psn))
        {
            FSRef location;
#if ! (defined(__LP64__) && __LP64__)
            FSSpec processAppSpec;
#endif
            ProcessInfoRec pir;

            memset((void *) &pir, 0, sizeof(pir));
            pir.processInfoLength = sizeof(pir);
            pir.processName = NULL;
#if defined(__LP64__) && __LP64__
            pir.processAppRef = &location;
#else
            pir.processAppSpec = &processAppSpec;
#endif
            if (noErr == GetProcessInformation(&psn, &pir))
            {
#if ! (defined(__LP64__) && __LP64__)
                if (FSpMakeFSRef
                    &&
                    (noErr == FSpMakeFSRef(&processAppSpec, &location)))
#endif
                {
                    static UInt8 path[256];

                    if (FSRefMakePath
                        &&
                        (noErr == FSRefMakePath(&location, path, sizeof(path) - 1)))
                    {
                        progname = (char *) path;
                    }
                }
            }
        }
    }
    /* when launched as a native application under Mac OS X, there may
     * be a bogus process serial number parameter. */
    if ((argc == 2) && (! strncmp(argv[1], "-psn_", strlen("-psn_"))))
    {
        argc = 0;
    }
#endif /* defined(__CARBON__) */
    progname = (progname && *progname) ? progname : MYMAN;
#if defined(__APPLE__) || defined(macintosh)
    my_mac_fixargs(&argc, &argv, progname);
#endif
    myman_expand_args(&argc, &argv, progname);
    td = 0.0L;
    for (i = 0; i < SPRITE_REGISTERS; i ++) {
        sprite_register_used[i] = 0;
        sprite_register_frame[i] = 0;
        sprite_register_color[i] = 0x7;
    }
    for (i = 0; i < 256; i ++) {
#ifndef BUILTIN_TILE
        tile_c[i] = 0x7;
#endif
#ifndef BUILTIN_SPRITE
        sprite_c[i] = 0x7;
#endif
    }
    parse_myman_args(argc, argv);

    for (i = 0; i < 256; i ++)
    {
        int c_mapped;

        c = (my_ulong) (my_uchar) cp437_sprite[i];
        c_mapped = c;
        while (c_mapped
               &&
               (! tile_u[c_mapped])
               &&
               (((int) (unsigned) fallback_cp437[c_mapped]) != c)
               &&
               (((int) (unsigned) fallback_cp437[c_mapped]) != c_mapped))
        {
            c_mapped = (my_ulong) (my_uchar) fallback_cp437[c_mapped];
            cp437_sprite[i] = (my_uchar) c_mapped;
        }
    }

    if (nogame)
        fflush(stdout), fflush(stderr), exit(EXIT_SUCCESS);

#ifdef LC_CTYPE
    locale_is_broken = 0;
    myman_locale = myman_getenv("MYMAN_LOCALE");
    myman_locale = myman_locale ? myman_locale : "";
    if (! setlocale(LC_CTYPE, myman_locale))
    {
        locale_is_broken = 1;
        fprintf(
            stderr,
            "warning: setlocale(LC_CTYPE, \"");
        mymanfescape(stderr, myman_locale, strlen(myman_locale));
        fprintf(
            stderr,
            "\") failed\n");
        fflush(stderr);
    }
#endif /* defined(LC_CTYPE) */

    if (use_fullwidth)
    {
        uni_cp437 = uni_cp437_fullwidth;
    }
    if (use_rounded || (use_double == use_fullwidth))
    {
        uni_cp437_rounded = (my_ulong *) malloc(256 * sizeof(my_ulong));
        if (! uni_cp437_rounded)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memcpy((void *) uni_cp437_rounded, (void *) uni_cp437, 256 * sizeof(my_ulong));
        if (use_double && use_fullwidth)
        {
            memcpy(
                (void *) (uni_cp437_rounded + 179),
                (void *) (uni_cp437_halfwidth + 179),
                (218 - 179) * sizeof(*uni_cp437_rounded));
        }
        else if ((! use_double) && (! use_fullwidth))
        {
            memcpy(
                (void *) (uni_cp437_rounded + 179),
                (void *) (uni_cp437_fullwidth + 179),
                (218 - 179) * sizeof(*uni_cp437_rounded));
        }
        if (use_rounded)
        {
            uni_cp437_rounded[217] = 0x256fU;
            uni_cp437_rounded[191] = 0x256eU;
            uni_cp437_rounded[218] = 0x256dU;
            uni_cp437_rounded[192] = 0x2570U;
        }
        uni_cp437 = uni_cp437_rounded;
    }
#if MY_CURSES_ARGS
    myman_argc = argc;
    myman_argv = argv;
#endif
    myman();
    if (uni_cp437_rounded != NULL)
    {
        free((void *) uni_cp437_rounded);
    }
    if (tmp_notice)
    {
        free((void *) tmp_notice);
        tmp_notice = 0;
    }
#ifdef __TINYC__
#ifndef main
    exit(EXIT_SUCCESS);
#endif
#endif
    return EXIT_SUCCESS;
}
#ifdef END_OF_MAIN
END_OF_MAIN()
#endif

#if defined(__WATCOMC__) && defined(__WINDOWS__) && ! defined(__WINDOWS_386__)
/* This horrible hack works around a calling convention
 * mismatch. Fortunately _fpmath(...) never seems to get called. */
const char _fpmath[] = { 0xCB };
#endif

#ifdef __VBCC__
/* This horrible hack is needed for VBCC (vc) to get a reasonable
 * stack size */
#ifndef MYMAN_VBCC_STACK_SIZE
#define MYMAN_VBCC_STACK_SIZE 65536
#endif
size_t __stack = MYMAN_VBCC_STACK_SIZE;
#endif
