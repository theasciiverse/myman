/*
 * newtcurs.h - newt driver for the MyMan video game
 * Copyright 2007-2008, Benjamin C. Wiley Sittler <bsittler@gmail.com>
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

/* feature guessing */
#ifndef MYMAN_GUESS_H_INCLUDED
#include "guess.h"
#endif

#ifndef MYMAN_NEWTCURS_H_INCLUDED
#define MYMAN_NEWTCURS_H_INCLUDED 1

#include <newt.h>
#include <slang.h>

/* work-arounds for newt */

#ifndef NEWTCURSES_OLD_NEWT
#ifndef NEWT_COLORSET_CUSTOM
#define NEWTCURSES_OLD_NEWT 1
#endif
#endif

#ifndef NEWTCURSES_OLD_NEWT
#define NEWTCURSES_OLD_NEWT 0
#endif

#ifndef NEWT_COLORSET_CUSTOM
#define NEWT_COLORSET_CUSTOM(x) (30 + (x))
#endif

#if NEWTCURSES_OLD_NEWT
#ifndef newtLabelSetColors
#define newtLabelSetColors(co,cs)
#endif
#endif

#undef usleep
#define usleep newtDelay

#ifndef USE_ATTR
#define USE_ATTR 1
#endif

#ifndef USE_UNDERLINE
#define USE_UNDERLINE 1
#endif

#ifndef SWAPDOTS
#define SWAPDOTS 1
#endif

#ifndef USE_ACS
#define USE_ACS 1
#endif

#ifndef USE_RAW_UCS
#define USE_RAW_UCS 1
#endif

#ifndef USE_SIGWINCH
#define USE_SIGWINCH 0
#endif

#ifndef USE_IOCTL
#define USE_IOCTL 0
#endif

#ifndef USE_WCWIDTH
#define USE_WCWIDTH 1
#endif

#ifndef USE_ICONV
#define USE_ICONV 0
#endif

#ifndef OK
#define OK (0)
#endif

#ifndef ERR
#define ERR (-1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (! FALSE)
#endif

static int newtcurses_ready = 0;
static int newtcurses_initialized = 0;

typedef unsigned long int chtype;

typedef chtype attr_t;

static long newtcurses_x = 0, newtcurses_y = 0, newtcurses_w = 0, newtcurses_h = 0;
static long newtcurses_maxw = 0, newtcurses_maxh = 0;

static attr_t newtcurses_attr = 0;

static newtComponent newtcurses_mainform = NULL;
static newtComponent *newtcurses_standouts = NULL;
static const char *newtcurses_use_color = NULL;
static const char *newtcurses_use_attrs = NULL;
static const char *newtcurses_use_fixedpal = NULL;
static const char *newtcurses_use_roottext = NULL;
static const char *newtcurses_use_both = NULL;
static const char *newtcurses_use_reinit = NULL;
static struct newtColors newtcurses_palette;
static chtype newtcurses_getch_buffer[] = { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR };

#define clear erase

#define cbreak()

#define noecho()

#define nonl()

#define nodelay(stdscr, x)

#define intrflush(stdscr, x)

#define immedok(stdscr, x)

#define idlok(stdscr, x)

#define clearok(stdscr, x)

#define leaveok(stdscr, x)

#define getyx(stdscr, y, x) ((y) = newtcurses_y), ((x) = newtcurses_x)

#define wrefresh(stdscr) refresh()

#define beep() addch('\a')

#define use_env(x)

#define echo()

static int curs_set(int visibility)
{
    if (newtcurses_ready)
    {
        if (visibility)
        {
            newtCursorOn();
        }
        else
        {
            newtCursorOff();
        }
    }
    return OK;
}

#define keypad(stdscr, x)

#define LINES (newtcurses_h)

#define COLS (newtcurses_w)

#define KEY_UP NEWT_KEY_UP

#define KEY_DOWN NEWT_KEY_DOWN

#define KEY_LEFT NEWT_KEY_LEFT

#define KEY_RIGHT NEWT_KEY_RIGHT

#define KEY_RESIZE NEWT_KEY_RESIZE

#define A_CHARTEXT ((chtype) 0x001fffffL)

#define NEWTCURSES__A_COLOR ((chtype) 0x0fe00000L)

#define A_UNDERLINE ((chtype) 0x10000000L)

#define A_BOLD ((chtype) 0x20000000L)

#define A_REVERSE ((chtype) 0x40000000L)

#define A_DIM ((chtype) 0x80000000L)

#define A_STANDOUT A_REVERSE

#define _PAIR_SHIFT 21

#ifdef COLOR_PAIR
#undef COLOR_PAIR
#endif
#define COLOR_PAIR(p) (((chtype) p) << _PAIR_SHIFT)

#ifdef PAIR_NUMBER
#undef PAIR_NUMBER
#endif
#define PAIR_NUMBER(a) (((a) & NEWTCURSES__A_COLOR) >> _PAIR_SHIFT)

#ifdef COLORS
#undef COLORS
#endif
#define COLORS (newtcurses_use_fixedpal ? 8 : 16)

#ifdef COLOR_PAIRS
#undef COLOR_PAIRS
#endif
#define COLOR_PAIRS 128

#ifdef COLOR_BLACK
#undef COLOR_BLACK
#endif
#define COLOR_BLACK 0

#ifdef COLOR_BLUE
#undef COLOR_BLUE
#endif
#define COLOR_BLUE 4

#ifdef COLOR_GREEN
#undef COLOR_GREEN
#endif
#define COLOR_GREEN 2

#ifdef COLOR_CYAN
#undef COLOR_CYAN
#endif
#define COLOR_CYAN (COLOR_BLUE | COLOR_GREEN)

#ifdef COLOR_RED
#undef COLOR_RED
#endif
#define COLOR_RED 1

#ifdef COLOR_MAGENTA
#undef COLOR_MAGENTA
#endif
#define COLOR_MAGENTA (COLOR_RED | COLOR_BLUE)

#ifdef COLOR_YELLOW
#undef COLOR_YELLOW
#endif
#define COLOR_YELLOW (COLOR_RED | COLOR_GREEN)

#ifdef COLOR_WHITE
#undef COLOR_WHITE
#endif
#define COLOR_WHITE (COLOR_RED | COLOR_GREEN | COLOR_BLUE)

#ifndef FOREGROUND_BLUE
#define FOREGROUND_BLUE 1
#endif

#ifndef FOREGROUND_GREEN
#define FOREGROUND_GREEN 2
#endif

#ifndef FOREGROUND_RED
#define FOREGROUND_RED 4
#endif

#ifndef FOREGROUND_INTENSITY
#define FOREGROUND_INTENSITY 8
#endif

#ifndef BACKGROUND_BLUE
#define BACKGROUND_BLUE 16
#endif

#ifndef BACKGROUND_GREEN
#define BACKGROUND_GREEN 32
#endif

#ifndef BACKGROUND_RED
#define BACKGROUND_RED 64
#endif

#ifndef BACKGROUND_INTENSITY
#define BACKGROUND_INTENSITY 128
#endif

#define PEN_BRIGHT 8

#define ACS_LRCORNER 0x2518

#define ACS_URCORNER 0x2510

#define ACS_ULCORNER 0x250c

#define ACS_LLCORNER 0x2514

#define ACS_PLUS 0x253c

#define ACS_HLINE 0x2500

#define ACS_LTEE 0x251c

#define ACS_RTEE 0x2524

#define ACS_BTEE 0x2534

#define ACS_TTEE 0x252c

#define ACS_VLINE 0x2502

#define ACS_BULLET 0x00b7

#define INITSCR_WITH_HINTS

static char *newtcurses_color_name[16] = {
    "black",
    "red",
    "green",
    "brown",
    "blue",
    "magenta",
    "cyan",
    "lightgray",
    "gray",
    "brightred",
    "brightgreen",
    "yellow",
    "brightblue",
    "brightmagenta",
    "brightcyan",
    "white"
};

#define NEWTCURSES_SAFE_FG(fg,bg)                                       \
    ((((fg) & COLOR_WHITE) == ((bg) & COLOR_WHITE)) ?                   \
     (((bg) & COLOR_WHITE) == COLOR_BLACK) ?                            \
     COLOR_WHITE :                                                      \
     COLOR_BLACK :                                                      \
     (fg))

#define NEWTCURSES_UNSAFE_COLORSET(fg,bg)                               \
    (((fg) == COLOR_WHITE) && ((bg) == COLOR_BLACK) ?                   \
     NEWT_COLORSET_SHADOW :                                             \
     NEWT_COLORSET_CUSTOM(55) / 56 * 56 +                               \
     ((((fg) & PEN_BRIGHT) ? 1 : 0) +                                   \
      (((bg) & PEN_BRIGHT) ? 2 : 0)) * 56 +                             \
     ((bg) & COLOR_WHITE) * 7 + 6 -                                     \
     (((fg) & COLOR_WHITE) -                                            \
      (((fg) & COLOR_WHITE) > ((bg) & COLOR_WHITE))))

#define NEWTCURSES_COLORSET(fg,bg)                                      \
    NEWTCURSES_UNSAFE_COLORSET(NEWTCURSES_SAFE_FG((fg),(bg)),(bg))

#ifdef has_colors
#undef has_colors
#endif
#define has_colors() (newtcurses_use_color != NULL)

static int newtcurses_colormap[16] = {
    /* ---- */ NEWT_COLORSET_ROOTTEXT,
    /* ---u */ NEWT_COLORSET_EMPTYSCALE,
    /* --d- */ NEWT_COLORSET_ENTRY,
    /* --du */ NEWT_COLORSET_EMPTYSCALE,
    /* -b-- */ NEWT_COLORSET_SELLISTBOX,
    /* -b-u */ NEWT_COLORSET_EMPTYSCALE,
    /* -bd- */ NEWT_COLORSET_SELLISTBOX,
    /* -bdu */ NEWT_COLORSET_EMPTYSCALE,
    /* r--- */ NEWT_COLORSET_ACTBUTTON,
    /* r--u */ NEWT_COLORSET_FULLSCALE,
    /* r-d- */ NEWT_COLORSET_ACTCHECKBOX,
    /* r-du */ NEWT_COLORSET_FULLSCALE,
    /* rb-- */ NEWT_COLORSET_ACTSELLISTBOX,
    /* rb-u */ NEWT_COLORSET_FULLSCALE,
    /* rbd- */ NEWT_COLORSET_ACTSELLISTBOX,
    /* rbdu */ NEWT_COLORSET_FULLSCALE
};

static chtype newtcurses_pairs[COLOR_PAIRS];

#ifndef NEED_LOCALE_IS_UTF8
#define NEED_LOCALE_IS_UTF8 1
#endif
static int locale_is_utf8(void);

#if HAVE_WCHAR_H
#include <wchar.h>
#else
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#endif

/* FIXME: this is a horrible hack! */
#ifndef wcwidth
#if ! HAVE_WCWIDTH
#define wcwidth(wc) (((((long) (wc)) >= 0x0020L) && (((long) (wc)) < 0x3000L)) ? 1 : ((((long) (wc)) >= 0x3000L) && (((long) (wc)) <= 0x10fffdL)) ? 2 : -1)
#endif /* ! HAVE_WCWIDTH */
#endif /* ! defined(wcwidth) */

static int newtcurses_wcwidth(int ch)
{
    int ret;

    ret = wcwidth(ch);
    if (ret < 1)
    {
        switch (ch)
        {
        case ACS_LRCORNER:
        case ACS_URCORNER:
        case ACS_ULCORNER:
        case ACS_LLCORNER:
        case ACS_PLUS:
        case ACS_HLINE:
        case ACS_LTEE:
        case ACS_RTEE:
        case ACS_BTEE:
        case ACS_TTEE:
        case ACS_VLINE:
        case ACS_BULLET:
            ret = 1;
        }
    }
    return ret;
}

static int can_change_color(void)
{
    return has_colors() && (newtcurses_use_fixedpal ? 0 : 1);
}

static int init_color(short i, short r, short g, short b)
{
    return ERR;
}

static int color_content(short i, short *r, short *g, short *b)
{
    return ERR;
}

static int start_color(void)
{
    return OK;
}

static int init_pair(short i, short fg, short bg)
{
    if (! has_colors()) return ERR;
    if ((i < 0) || (i > COLOR_PAIRS)) return ERR;
    newtcurses_pairs[i] = 0;
    if (fg & COLOR_RED) newtcurses_pairs[i] |= FOREGROUND_RED;
    if (fg & COLOR_GREEN) newtcurses_pairs[i] |= FOREGROUND_GREEN;
    if (fg & COLOR_BLUE) newtcurses_pairs[i] |= FOREGROUND_BLUE;
    if (fg & PEN_BRIGHT) newtcurses_pairs[i] |= FOREGROUND_INTENSITY;
    if (bg & COLOR_RED) newtcurses_pairs[i] |= BACKGROUND_RED;
    if (bg & COLOR_GREEN) newtcurses_pairs[i] |= BACKGROUND_GREEN;
    if (bg & COLOR_BLUE) newtcurses_pairs[i] |= BACKGROUND_BLUE;
    if (bg & PEN_BRIGHT) newtcurses_pairs[i] |= BACKGROUND_INTENSITY;
    return OK;
}

static int pair_content(short i, short *fg, short *bg)
{
    if (! has_colors()) return ERR;
    if ((i < 0) || (i > COLOR_PAIRS)) return ERR;
    *fg = 0;
    if (newtcurses_pairs[i] & FOREGROUND_RED) *fg |= COLOR_RED;
    if (newtcurses_pairs[i] & FOREGROUND_GREEN) *fg |= COLOR_GREEN;
    if (newtcurses_pairs[i] & FOREGROUND_BLUE) *fg |= COLOR_BLUE;
    if (newtcurses_pairs[i] & FOREGROUND_INTENSITY) *fg |= PEN_BRIGHT;
    *bg = 0;
    if (newtcurses_pairs[i] & BACKGROUND_RED) *bg |= COLOR_RED;
    if (newtcurses_pairs[i] & BACKGROUND_GREEN) *bg |= COLOR_GREEN;
    if (newtcurses_pairs[i] & BACKGROUND_BLUE) *bg |= COLOR_BLUE;
    if (newtcurses_pairs[i] & BACKGROUND_INTENSITY) *bg |= PEN_BRIGHT;
    return OK;
}

static void newtcurses_ungetch(chtype ch);

static void newtcurses_suspend(void *d)
{
    newtSuspend();
    raise(SIGTSTP);
    newtResume();
    newtcurses_ungetch(KEY_RESIZE);
}

static void newtcurses_ungetch(chtype ch)
{
    int i;

    for (i = 0; i < (sizeof(newtcurses_getch_buffer)/sizeof(*newtcurses_getch_buffer)); i ++)
    {
        if (newtcurses_getch_buffer[i] == ERR)
        {
            newtcurses_getch_buffer[i] = ch;
            return;
        }
    }
    newtBell();
}

static void newtcurses_help(newtComponent co, void *tag)
{
    newtcurses_ungetch((chtype) (unsigned long) (unsigned char) 'h');
}

static void endwin(void);

static void newtcurses_cleanup(void)
{
    endwin();
    if (newtcurses_initialized)
    {
        newtFinished();
        newtcurses_initialized = 0;
    }
}

static int initscrWithHints(int h, int w, const char *title, const char *shortname)
{
    int i, j;

    if ((! myman_getenv("SLANG_USE_TERMCAP")) ||
        (! *(myman_getenv("SLANG_USE_TERMCAP"))) ||
        (! atoi(myman_getenv("SLANG_USE_TERMCAP"))))
    {
        /* ths avoids a bad interaction between slang and newt when
         * TERMCAP is set (freeing memory not allocated using malloc:
         * boom!); it also works around the horribly broken TERMCAP
         * values produced by truncation to accomodate classic curses'
         * (tiny) fixed-length buffers without overrun checks. */
        SLtt_Try_Termcap = 0;
    }
    newtcurses_use_attrs = myman_getenv("NEWTCURSES_ATTRS");
    if ((! newtcurses_use_attrs) || (! *newtcurses_use_attrs))
    {
        newtcurses_use_attrs = NEWTCURSES_OLD_NEWT ? "0" : "1";
    }
    if (atoi(newtcurses_use_attrs))
    {
        newtcurses_use_attrs = "1";
    }
    else
    {
        newtcurses_use_attrs = NULL;
    }
    newtcurses_use_reinit = myman_getenv("NEWTCURSES_REINIT");
    if ((! newtcurses_use_reinit) || (! *newtcurses_use_reinit))
    {
        newtcurses_use_reinit = SLtt_Try_Termcap ? "0" : "1";
    }
    if (atoi(newtcurses_use_reinit))
    {
        newtcurses_use_reinit = "1";
    }
    else
    {
        newtcurses_use_reinit = NULL;
    }
    newtcurses_use_fixedpal = myman_getenv("NEWTCURSES_FIXEDPAL");
    if ((! newtcurses_use_fixedpal) || (! *newtcurses_use_fixedpal))
    {
        newtcurses_use_fixedpal = "0";
    }
    if (atoi(newtcurses_use_fixedpal))
    {
        newtcurses_use_fixedpal = "1";
    }
    else
    {
        newtcurses_use_fixedpal = NULL;
    }
    newtcurses_use_roottext = myman_getenv("NEWTCURSES_ROOTTEXT");
    if ((! newtcurses_use_roottext) || (! *newtcurses_use_roottext))
    {
        newtcurses_use_roottext = NEWTCURSES_OLD_NEWT ? "1" : "0";
    }
    if (atoi(newtcurses_use_roottext))
    {
        newtcurses_use_roottext = "1";
    }
    else
    {
        newtcurses_use_roottext = NULL;
    }
    newtcurses_use_both = myman_getenv("NEWTCURSES_BOTH");
    if ((! newtcurses_use_both) || (! *newtcurses_use_both))
    {
        newtcurses_use_both = NEWTCURSES_OLD_NEWT ? "1" : "0";
    }
    if (atoi(newtcurses_use_both))
    {
        newtcurses_use_both = "1";
    }
    else
    {
        newtcurses_use_both = NULL;
    }
    newtcurses_use_color = myman_getenv("NEWTCURSES_COLOR");
    if ((! newtcurses_use_color) || (! *newtcurses_use_color))
    {
        newtcurses_use_color = newtcurses_use_attrs ? "1" : "0";
    }
    if (atoi(newtcurses_use_color))
    {
        newtcurses_use_color = "1";
    }
    else
    {
        newtcurses_use_color = NULL;
    }
    endwin();
    for (i = 0; i < (sizeof(newtcurses_getch_buffer)/sizeof(*newtcurses_getch_buffer)); i ++)
    {
        newtcurses_getch_buffer[i] = ERR;
    }
    if (w > 128) w = 128;
    if (h > 50) h = 50;
    if (! newtcurses_initialized)
    {
        if (newtInit())
        {
            return 0;
        }
        newtcurses_initialized = 1;
        atexit(newtcurses_cleanup);
    }
    else
    {
        newtResume();
    }
    for (i = 0; i < COLOR_PAIRS; ++i)
    {
        init_pair(i, i % COLORS, i / COLORS);
    }
    newtPushHelpLine(NULL);
    if (! newtcurses_use_fixedpal)
    {
        memcpy((void *) &newtcurses_palette, (void *) &newtDefaultColorPalette, sizeof(newtcurses_palette));
        newtcurses_palette.shadowFg = newtcurses_color_name[COLOR_WHITE | PEN_BRIGHT];
        newtcurses_palette.shadowBg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.rootTextFg = newtcurses_color_name[COLOR_WHITE];
        newtcurses_palette.rootTextBg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.entryFg = newtcurses_color_name[COLOR_BLACK | PEN_BRIGHT];
        newtcurses_palette.entryBg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.emptyScale = newtcurses_color_name[COLOR_CYAN | PEN_BRIGHT];
        newtcurses_palette.selListboxFg = newtcurses_color_name[COLOR_MAGENTA | PEN_BRIGHT];
        newtcurses_palette.selListboxBg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.actButtonFg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.actButtonBg = newtcurses_color_name[COLOR_WHITE];
        newtcurses_palette.actCheckboxFg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.actCheckboxBg = newtcurses_color_name[COLOR_BLACK | PEN_BRIGHT];
        newtcurses_palette.fullScale = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.actSelListboxFg = newtcurses_color_name[COLOR_BLACK];
        newtcurses_palette.actSelListboxBg = newtcurses_color_name[COLOR_MAGENTA | PEN_BRIGHT];
        newtSetColors(newtcurses_palette);
        newtSetColor(NEWT_COLORSET_EMPTYSCALE, newtcurses_color_name[COLOR_CYAN | PEN_BRIGHT], newtcurses_color_name[COLOR_BLACK]);
        newtSetColor(NEWT_COLORSET_FULLSCALE, newtcurses_color_name[COLOR_BLACK], newtcurses_color_name[COLOR_CYAN | PEN_BRIGHT]);
        if (has_colors())
        {
            for (i = 0; i < 16; i++)
            {
                for (j = 0; j < 16; j++)
                {
                    int colorset;

                    colorset = NEWTCURSES_COLORSET(i, j);
                    if (colorset == NEWTCURSES_UNSAFE_COLORSET(i, j)) {
                        newtSetColor(colorset, newtcurses_color_name[i], newtcurses_color_name[j]);
                    }
                }
            }
        }
    }
    newtGetScreenSize(&w, &h);
    newtcurses_w = w;
    newtcurses_h = h;
    newtcurses_x = 0;
    newtcurses_y = 0;
    newtcurses_attr = 0;
    newtSetSuspendCallback(newtcurses_suspend, NULL);
    newtSetHelpCallback(newtcurses_help);
    newtcurses_mainform = newtForm(NULL, "", 0);
    newtFormSetBackground(newtcurses_mainform, newtcurses_colormap[0]);
    for (i = 0; i < 0x80; i ++)
    {
        newtFormAddHotKey(newtcurses_mainform, i);
    }
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_UP);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_DOWN);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_LEFT);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_RIGHT);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_BKSPC);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_DELETE);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_F12);
    newtFormAddHotKey(newtcurses_mainform, NEWT_KEY_RESIZE);
    newtcurses_maxw = newtcurses_w;
    newtcurses_maxh = newtcurses_h;
    newtcurses_standouts = NULL;
    if (newtcurses_use_attrs)
    {
        newtcurses_standouts = (newtComponent *) malloc(newtcurses_maxw * newtcurses_maxh * sizeof(newtComponent));
    }
    if (newtcurses_standouts)
    {
        for (i = 0; i < (newtcurses_w * newtcurses_h); i ++)
        {
            newtcurses_standouts[i] = NULL;
        }
    }
    newtDrawForm(newtcurses_mainform);
    newtRefresh();
    newtFormSetTimer(newtcurses_mainform, 1);
    newtcurses_ready = 1;
    return 1;
}

static void endwin(void)
{
    if (newtcurses_ready)
    {
        newtcurses_ready = 0;
        newtFormDestroy(newtcurses_mainform);
        if (newtcurses_standouts)
        {
            free((void *) newtcurses_standouts);
            newtcurses_standouts = NULL;
        }
        newtcurses_mainform = NULL;
        newtPopHelpLine();
        newtSuspend();
        SLtt_get_screen_size();
        SLsmg_reinit_smg();
    }
    if (newtcurses_initialized && newtcurses_use_reinit)
    {
        newtFinished();
        newtcurses_initialized = 0;
    }
}

static int move(int y, int x);

static int addch(const chtype ch);

static int erase(void)
{
    int x, y;

    newtcurses_attr = 0;
    for (y = 0; y < LINES; y ++)
        for (x = 0; x < COLS; x ++)
        {
            move(y, x);
            addch((chtype) (unsigned long) (unsigned char) ' ');
        }
    move(0, 0);
    return OK;
}

static int resizeterm(int y, int x)
{
    newtResizeScreen(1);
    newtGetScreenSize(&x, &y);
    if (newtcurses_standouts
        &&
        ((y > newtcurses_maxh)
         ||
         (x > newtcurses_maxw)))
    {
        int i, j;
        newtComponent *newstandouts;

        newstandouts = (newtComponent *) realloc((void *) newtcurses_standouts,
                                        ((x > newtcurses_maxw) ? x : newtcurses_maxw)
                                        *
                                        ((y > newtcurses_maxh) ? y : newtcurses_maxh)
                                        *
                                        sizeof(newtComponent));
        if (newstandouts)
        {
            /* rearrange to support new geometry */
            newtcurses_standouts = newstandouts;
            for (i = ((y > newtcurses_maxh) ? y : newtcurses_maxh); i; i ++)
                for (j = ((x > newtcurses_maxw) ? x : newtcurses_maxw); j; j ++)
                {
                    newtComponent co;

                    co = NULL;
                    if ((i < newtcurses_maxh) && (j < newtcurses_maxw))
                    {
                        co = newtcurses_standouts[i * newtcurses_maxw + j];
                    }
                    newtcurses_standouts[i * ((x > newtcurses_maxw) ? x : newtcurses_maxw) + j] = co;
                }
        }
        else
        {
            /* screen got too big, stop supporting standouts */
            for (i = 0; i < (newtcurses_maxw * newtcurses_maxh); i ++)
            {
                newtComponent co;

                co = newtcurses_standouts[i];
                if (co)
                {
                    newtLabelSetColors(co, newtcurses_colormap[0]);
                    newtLabelSetText(co, "");
                }
            }
            free((void *) newtcurses_standouts);
            newtcurses_standouts = NULL;
        }
    }
    newtcurses_w = x;
    newtcurses_h = y;
    if (newtcurses_w > newtcurses_maxw) newtcurses_maxw = newtcurses_w;
    if (newtcurses_h > newtcurses_maxh) newtcurses_maxh = newtcurses_h;
    return OK;
}

static chtype getch(void) {
    chtype ret = ERR;
    int i, j;

    newtGetScreenSize(&i, &j);
    if ((i != newtcurses_w) || (j != newtcurses_h))
    {
        newtResizeScreen(1);
        return KEY_RESIZE;
    }
    j = -1;
    for (i = 0; i < (sizeof(newtcurses_getch_buffer)/sizeof(*newtcurses_getch_buffer)); i ++)
    {
        if (newtcurses_getch_buffer[i] != ERR)
        {
            j = i;
        }
    }
    if (j != -1)
    {
        if (j == KEY_RESIZE)
        {
            newtResizeScreen(1);
        }
        ret = newtcurses_getch_buffer[j];
        newtcurses_getch_buffer[j] = ERR;
    }
    else
    {
        struct newtExitStruct ev;

        newtRefresh();
        newtFormRun(newtcurses_mainform, &ev);
        /*fprintf(stderr, "ev.reason: %d\n", ev.reason); fflush(stderr);*/
        if (ev.reason == NEWT_EXIT_HOTKEY)
        {
            /*fprintf(stderr, "ev.u.key: %x\n", ev.u.key); fflush(stderr);*/
            ret = ev.u.key;
            if (! ret)
            {
                ret = (chtype) ERR;
            }
            else if (ret == NEWT_KEY_F12)
            {
                ret = (int) (unsigned int) (unsigned char) 'q';
            }
            else if (ret == NEWT_KEY_BKSPC)
            {
                ret = (int) (unsigned int) (unsigned char) '\b';
            }
            else if (ret == NEWT_KEY_DELETE)
            {
                ret = (int) (unsigned int) (unsigned char) '\177';
            }
        }
        else if (ev.reason == NEWT_EXIT_COMPONENT)
        {
            ret = (chtype) (unsigned long) (unsigned char) 'q';
        }
    }
    return ret;
}

static int refresh(void)
{
    if (! newtcurses_ready) return OK;
    newtcurses_ungetch(getch());
    return OK;
}

static void standout()
{
    newtcurses_attr = A_STANDOUT;
}

static void standend()
{
    newtcurses_attr = 0;
}

static int attrset(int a)
{
    newtcurses_attr = a;
    return OK;
}

static int move(int y, int x)
{
    newtcurses_y = y;
    newtcurses_x = x;
    return OK;
}

static int insch(const chtype ch)
{
    return ERR;
}

static int delch(void)
{
    return ERR;
}

static int addch(const chtype ch)
{
    char buf[5];
    int wcw;
    int i;
    int attridx;
    int newtcolorset;
    int wantstandout;
    int wantroottext;

    if (ch != (ch & A_CHARTEXT))
    {
        attr_t oattr;
        int ret;

        oattr = newtcurses_attr;
        attrset((oattr & ~((ch & NEWTCURSES__A_COLOR) ? NEWTCURSES__A_COLOR : 0)) | (ch & ~A_CHARTEXT));
        ret = addch(ch & A_CHARTEXT);
        attrset(oattr);
        return ret;
    }
    memset((void *) buf, '\0', sizeof(buf));
    if (ch == (chtype) (unsigned long) (unsigned char) '\a')
    {
        newtBell();
        return OK;
    }
    else if (ch == (chtype) (unsigned long) (unsigned char) '\b')
    {
        if (newtcurses_y && ! newtcurses_x)
        {
            newtcurses_y --;
        }
        newtcurses_x += newtcurses_w - 1;
        newtcurses_x %= newtcurses_w;
        return OK;
    }
    else if (ch == (chtype) (unsigned long) (unsigned char) '\n')
    {
        newtcurses_y ++;
        newtcurses_x = 0;
        return OK;
    }
    else if (ch == (chtype) (unsigned long) (unsigned char) '\t')
    {
        do
        {
            addch((chtype) (unsigned long) (unsigned char) ' ');
        }
        while (newtcurses_x % 8);
        return OK;
    }
    wcw = newtcurses_wcwidth(ch);
    if (wcw < 1)
    {
        return ERR;
    }
    if (newtcurses_x < 0) return ERR;
    if (newtcurses_y < 0) return ERR;
    if ((newtcurses_x + wcw) > newtcurses_w) return ERR;
    if (newtcurses_y >= newtcurses_h) return ERR;
    if (((newtcurses_x + wcw) == newtcurses_w) &&
        (newtcurses_y + 1 == newtcurses_h))
    {
        return ERR;
    }
    if (ch > ((locale_is_utf8() || (wcwidth(ch) < 1)) ? 0x7f : 0xff))
    {
        switch(ch)
        {
        case ACS_LRCORNER:
        case ACS_URCORNER:
        case ACS_ULCORNER:
        case ACS_LLCORNER:
        case ACS_PLUS:
            buf[0] = '+';
            break;
        case ACS_HLINE:
            buf[0] = '-';
            break;
        case ACS_LTEE:
        case ACS_RTEE:
        case ACS_BTEE:
        case ACS_TTEE:
            buf[0] = '+';
            break;
        case ACS_VLINE:
            buf[0] = '|';
            break;
        case ACS_BULLET:
            buf[0] = '.';
            break;
        default:
            buf[0] = '*';
        }
        if (locale_is_utf8())
        {
            if (ch <= 0x7ff)
            {
                buf[0] = 0xc0 | (ch >> 6);
                buf[1] = 0x80 | (ch & 0x3f);
            }
            else if (ch <= 0xffff)
            {
                buf[0] = 0xe0 | (ch >> 12);
                buf[1] = 0x80 | ((ch >> 6) & 0x3f);
                buf[2] = 0x80 | (ch & 0x3f);
            }
            else if (ch <= 0x10ffff)
            {
                buf[0] = 0xf0 | (ch >> 18);
                buf[1] = 0x80 | ((ch >> 12) & 0x3f);
                buf[2] = 0x80 | ((ch >> 6) & 0x3f);
                buf[3] = 0x80 | (ch & 0x3f);
            }
        }
    }
    else
    {
        buf[0] = ch;
    }
    attridx =
        ((newtcurses_attr & A_REVERSE) ? 8 : 0)
        |
        ((newtcurses_attr & A_BOLD) ? 4 : 0)
        |
        ((newtcurses_attr & A_DIM) ? 2 : 0)
        |
        ((newtcurses_attr & A_UNDERLINE) ? 1 : 0);
    newtcolorset = newtcurses_colormap[attridx];
    if (newtcurses_attr & NEWTCURSES__A_COLOR)
    {
        short pair, fg, bg, tmp;

        pair = PAIR_NUMBER(newtcurses_attr & NEWTCURSES__A_COLOR);
        fg = COLOR_WHITE;
        bg = COLOR_BLACK;
        if (pair_content(pair, &fg, &bg) != ERR)
        {
            if (newtcurses_attr & A_BOLD)
            {
                fg |= PEN_BRIGHT;
            }
            if (newtcurses_attr & A_DIM)
            {
                fg = COLOR_BLACK | PEN_BRIGHT;
            }
            if (newtcurses_attr & A_REVERSE)
            {
                tmp = fg;
                fg = bg;
                bg = tmp;
            }
            if ((fg & COLOR_WHITE) == (bg & COLOR_WHITE))
            {
                fg = ((bg & COLOR_WHITE) == COLOR_BLACK) ? COLOR_WHITE : COLOR_BLACK;
            }
            newtcolorset = NEWTCURSES_COLORSET(fg, bg);
        }
    }
    wantstandout =
        ((newtcolorset != NEWT_COLORSET_ROOTTEXT)
         ||
         ! newtcurses_use_roottext);
    wantroottext = 1;
    if (newtcurses_standouts
        &&
        newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x]
        &&
        (! wantstandout))
    {
        if (! NEWTCURSES_OLD_NEWT)
        {
            wantstandout = 1;
        }
        else
        {
            newtComponent co;

            co = newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x];
            newtLabelSetColors(co, newtcurses_colormap[0]);
            newtLabelSetText(co, "");
        }
    }
    if (newtcurses_standouts && wantstandout)
    {
        if (! newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x])
        {
            newtComponent co;

            co = newtLabel(newtcurses_x, newtcurses_y, "");
            if (co)
            {
                newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x] = co;
                newtFormAddComponent(newtcurses_mainform, co);
            }
        }
        if (! newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x])
        {
            /* too much standouting, remove all standouts */
            for (i = 0; i < (newtcurses_maxw * newtcurses_maxh); i ++)
            {
                newtComponent co;

                co = newtcurses_standouts[i];
                if (co)
                {
                    newtLabelSetColors(co, newtcurses_colormap[0]);
                    newtLabelSetText(co, "");
                }
            }
            free((void *) newtcurses_standouts);
            newtcurses_standouts = NULL;
        }
        if (newtcurses_standouts && newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x] && newtcurses_ready)
        {
            newtComponent co, co2;

            co = newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x];
            if (co)
            {
                for (i = 1; i < wcw; i++)
                {
                    co2 = newtcurses_standouts[newtcurses_y * newtcurses_maxw + newtcurses_x + i];
                    if (co2)
                    {
                        newtLabelSetText(co2, "");
                    }
                }
                newtLabelSetColors(co, newtcolorset);
                newtLabelSetText(co, strdup(buf));
                if (! newtcurses_use_both)
                {
                    wantroottext = 0;
                }
            }
        }
    }
    if (wantroottext)
    {
        newtDrawRootText(newtcurses_x, newtcurses_y, buf);
    }
    newtcurses_x += wcw;
    return OK;
}

static int clrtobot(void)
{
    unsigned int x, y;

    for (x = newtcurses_x; x < COLS; x ++)
    {
        move(newtcurses_y, x);
        addch(' ');
    }
    for (y = newtcurses_y + 1; y < LINES; y ++)
        for (x = 0; x < COLS; x ++)
        {
            move(y, x);
            addch(' ');
        }
    return OK;
}

static int clrtoeol(void)
{
    unsigned int x;

    for (x = newtcurses_x; x < COLS; x ++)
    {
        move(newtcurses_y, x);
        addch(' ');
    }
    return OK;
}

static int addstr(const char *s)
{
    while (*s)
    {
        addch(*s++);
    }
    return OK;
}

static int mvprintw(int y, int x, const char *s)
{
    move(y, x);
    return addstr(s);
}

#undef wcwidth
#define wcwidth(ch) newtcurses_wcwidth(ch)

#endif /* ! defined(MYMAN_NEWTCURS_H_INCLUDED) */
