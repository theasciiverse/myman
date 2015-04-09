/*
 * aacurses.h - aalib driver for the MyMan video game
 * Copyright 2007, Benjamin C. Wiley Sittler <bsittler@gmail.com>
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

#include <aalib.h>

/* work-arounds for aalib */

#ifndef USE_ATTR
#define USE_ATTR 1
#endif

#ifndef USE_UNDERLINE
#define USE_UNDERLINE 1
#endif

#ifndef USE_RAW_UCS
#define USE_RAW_UCS 0
#endif

#ifndef USE_SIGWINCH
#define USE_SIGWINCH 0
#endif

#ifndef USE_IOCTL
#define USE_IOCTL 0
#endif

static int aacurses_ready = 0;

static char *aacurses_bitmap;

typedef unsigned long int chtype;

typedef chtype attr_t;

static aa_context *aacurses_ctx;

static struct aa_hardware_params aacurses_hwparams;

static struct aa_renderparams aacurses_renderparams;

static int aacurses_x = 0, aacurses_y = 0, aacurses_w = 0, aacurses_new_w = 0, aacurses_h = 0, aacurses_new_h = 0;

#define LINES (aacurses_h + 0)

#define COLS (aacurses_w + 0)

static attr_t aacurses_attr = 0;

#define AACURSES_AA_ATTR(a)                                             \
    ((((a) & A_STANDOUT) ||                                             \
      (((a) & A_REVERSE) && ((a) & A_DIM))) ?                           \
     AA_SPECIAL :                                                       \
     ((a) & A_REVERSE) ?                                                \
     ((aacurses_ctx->params.supported & AA_REVERSE_MASK) ?              \
      AA_REVERSE :                                                      \
      AA_SPECIAL) :                                                     \
     (((a) & A_DIM) &&                                                  \
      (aacurses_ctx->params.supported & AA_DIM_MASK)) ?                 \
     AA_DIM :                                                           \
     ((a) & A_BOLD) ?                                                   \
     ((aacurses_ctx->params.supported & AA_BOLD_MASK) ?                 \
      AA_BOLD :                                                         \
      (aacurses_ctx->params.supported & AA_BOLDFONT_MASK) ?             \
      AA_BOLDFONT :                                                     \
      AA_NORMAL) :                                                      \
     AA_NORMAL)

#ifndef OK
#define OK (0)
#endif

#ifndef ERR
#define ERR (-1)
#endif

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

#define getyx(stdscr, y, x) ((y) = aacurses_y), ((x) = aacurses_x)

#define wrefresh(stdscr) refresh()

#define use_env(x)

#define echo()

#define curs_set(x)

#define keypad(stdscr, x)

#define KEY_UP AA_UP

#define KEY_DOWN AA_DOWN

#define KEY_LEFT AA_LEFT

#define KEY_RIGHT AA_RIGHT

#define KEY_RESIZE AA_RESIZE

#define A_CHARTEXT 0x001FFFFF

#define A_ALTCHARSET 0x04000000

#define A_REVERSE 0x08000000

#define A_DIM 0x10000000

#define A_UNDERLINE A_DIM

#define A_BOLD 0x20000000

#define A_STANDOUT 0x40000000

#define ACS_LRCORNER (0x2518 | A_ALTCHARSET)

#define ACS_URCORNER (0x2510 | A_ALTCHARSET)

#define ACS_ULCORNER (0x250c | A_ALTCHARSET)

#define ACS_LLCORNER (0x2514 | A_ALTCHARSET)

#define ACS_PLUS (0x253c | A_ALTCHARSET)

#define ACS_HLINE (0x2500 | A_ALTCHARSET)

#define ACS_LTEE (0x251c | A_ALTCHARSET)

#define ACS_RTEE (0x2524 | A_ALTCHARSET)

#define ACS_BTEE (0x2534 | A_ALTCHARSET)

#define ACS_TTEE (0x252c | A_ALTCHARSET)

#define ACS_VLINE (0x2502 | A_ALTCHARSET)

#define ACS_BULLET (0x00b7 | A_ALTCHARSET)

#define ACS_DIAMOND (0x25c6 | A_ALTCHARSET)

#define ACS_BLOCK (0x2588U | A_ALTCHARSET)

#define ACS_CKBOARD (0x2592U | A_ALTCHARSET)

static int aacurses_use_acs = 1;

static int aacurses_use_xfont = 1;

static int aacurses_swapdots = 1;

#define SWAPDOTS (!!aacurses_swapdots)

static void endwin(void)
{
    if (aacurses_ready)
    {
        aacurses_ready = 0;
        aa_showcursor(aacurses_ctx);
        aa_flush(aacurses_ctx);
    }
    if (aacurses_new_w)
    {
        aacurses_w = aacurses_new_w;
    }
    if (aacurses_new_h)
    {
        aacurses_h = aacurses_new_h;
    }
}

static void aacurses_cleanup(void)
{
    endwin();
    if (aacurses_ctx)
    {
        aa_uninitkbd(aacurses_ctx);
        aa_close(aacurses_ctx);
        aacurses_ctx = NULL;
    }
}

#define INITSCR_WITH_HINTS

static int initscrWithHints(int h, int w, const char *title, const char *shortname)
{
    int is_x11;

    endwin();
    if (aacurses_new_w)
    {
        w = aacurses_new_w;
    }
    if (aacurses_new_h)
    {
        h = aacurses_new_h;
    }
    is_x11 = 0;
    if (aa_drivers[0] &&
        aa_drivers[0]->shortname &&
        ! strcmp(aa_drivers[0]->shortname, "X11"))
    {
        is_x11 = myman_getenv("DISPLAY") && *myman_getenv("DISPLAY");
    }
    aacurses_bitmap = myman_getenv("AACURSES_BITMAP");
    if (aacurses_bitmap && ! *aacurses_bitmap) aacurses_bitmap = NULL;
    if (aacurses_bitmap)
    {
        h = (h + 1) / 2;
        w = (w + 1) / 2;
    }
    if (aacurses_new_h || aacurses_new_w)
    {
        aacurses_new_w = 0;
        aacurses_new_h = 0;
    }
    else if ((! myman_getenv("AAFont")) ||
             strcmp(myman_getenv("AAFont"), "nil2"))
    {
        if (w > 128) w = 128;
        else if (h > 50) h = 50;
    }
    memcpy((void *) &aacurses_renderparams, (void *) &aa_defrenderparams, sizeof(aa_renderparams));
    memset((void *) &aacurses_hwparams, 0, sizeof(aacurses_hwparams));
    aa_parseoptions(& aacurses_hwparams, & aacurses_renderparams, NULL, NULL);
    if (aacurses_hwparams.maxwidth && (w > aacurses_hwparams.maxwidth)) w = aacurses_hwparams.maxwidth;
    if (w < aacurses_hwparams.minwidth) w = aacurses_hwparams.maxwidth;
    if (aacurses_hwparams.recwidth) w = aacurses_hwparams.recwidth;
    if (aacurses_hwparams.maxheight && (h > aacurses_hwparams.maxheight)) h = aacurses_hwparams.maxheight;
    if (h < aacurses_hwparams.minheight) h = aacurses_hwparams.minheight;
    if (aacurses_hwparams.recheight) h = aacurses_hwparams.recheight;
    if (is_x11 && (! myman_getenv("AAOPTS")))
    {
        aacurses_hwparams.width = w;
        aacurses_hwparams.height = h;
    }
    aacurses_hwparams.supported = AA_NORMAL_MASK | AA_DIM_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK | AA_REVERSE_MASK;
    if (! aacurses_ctx)
    {
        aacurses_ctx = aa_autoinit(& aacurses_hwparams);
        aa_autoinitkbd(aacurses_ctx, 0);
        atexit(aacurses_cleanup);
    }
    else
    {
        aa_resize(aacurses_ctx);
    }
    if (! aacurses_ctx)
    {
        fprintf(stderr, "aa_autoinit failed\n");
        fflush(stderr);
        return 0;
    }
    is_x11 = aacurses_ctx->driver &&
        aacurses_ctx->driver->shortname &&
        ! strcmp(aacurses_ctx->driver->shortname, "X11");
    aacurses_h = aacurses_bitmap ? aa_imgheight(aacurses_ctx) : aa_scrheight(aacurses_ctx);
    aacurses_w = aacurses_bitmap ? aa_imgwidth(aacurses_ctx) : aa_scrwidth(aacurses_ctx);
    aa_hidecursor(aacurses_ctx);
    aacurses_x = 0;
    aacurses_y = 0;
    aacurses_attr = 0;
    aacurses_ready = 1;
    aacurses_swapdots = 1;
    aacurses_use_acs = 0;
    aacurses_use_xfont = 0;
    if (is_x11 && (! myman_getenv("AAFont")) && ! aacurses_bitmap)
    {
        aacurses_use_xfont = 1;
    }
    if (myman_getenv("AACURSES_CP437") && *myman_getenv("AACURSES_CP437"))
    {
        aacurses_use_acs = atoi(myman_getenv("AACURSES_CP437"));
        aacurses_use_xfont = 0;
    }
    if (myman_getenv("AACURSES_XFONT") && *myman_getenv("AACURSES_XFONT"))
    {
        aacurses_use_xfont = atoi(myman_getenv("AACURSES_XFONT"));
    }
    if (aacurses_use_xfont)
    {
        aacurses_use_acs = 1;
    }
    if (myman_getenv("AACURSES_SWAPDOTS") && *myman_getenv("AACURSES_SWAPDOTS"))
    {
        aacurses_swapdots = atoi(myman_getenv("AACURSES_SWAPDOTS"));
    }
    return 1;
}

static int erase(void) {
    aacurses_attr = 0;
    for (aacurses_y = 0; aacurses_y < LINES; aacurses_y ++)
        for (aacurses_x = 0; aacurses_x < COLS; aacurses_x ++)
        {
            if (aacurses_bitmap)
            {
                aa_putpixel(aacurses_ctx, aacurses_x, aacurses_y, 0);
            }
            else
            {
                aa_puts(aacurses_ctx, aacurses_x, aacurses_y, AACURSES_AA_ATTR(aacurses_attr), " ");
            }
        }
    aacurses_x = 0;
    aacurses_y = 0;
    return OK;
}

static int refresh(void) {
    if (! aacurses_ready) return OK;
    if (aacurses_bitmap)
    {
        aa_render(aacurses_ctx, &aacurses_renderparams, 0, 0, aa_scrwidth(aacurses_ctx), aa_scrheight(aacurses_ctx));
    }
    aa_flush(aacurses_ctx);
    return OK;
}

static int resizeterm(int y, int x)
{
    if (! aacurses_new_w)
    {
        aacurses_new_w = x;
    }
    if (! aacurses_new_h)
    {
        aacurses_new_h = y;
    }
    return OK;
}

static chtype getch(void) {
    int ch;

    ch = aa_getevent(aacurses_ctx, 0);
    if (ch)
    {
        if ((ch == AA_RESIZE) || (ch == AA_UNKNOWN))
        {
            aacurses_new_h = aacurses_bitmap ? aa_imgheight(aacurses_ctx) : aa_scrheight(aacurses_ctx);
            aacurses_new_w = aacurses_bitmap ? aa_imgwidth(aacurses_ctx) : aa_scrwidth(aacurses_ctx);
            return KEY_RESIZE;
        }
        else if (ch == AA_BACKSPACE)
        {
            return 0x08;
        }
        else if (ch == AA_ESC)
        {
            return 0x1b;
        }
        else
        {
            return ch;
        }
    }
    return ERR;
}

static void standout() {
    aacurses_attr = A_STANDOUT;
}

static void standend() {
    aacurses_attr = 0;
}

static int attrset(attr_t a)
{
    aacurses_attr = a & ~A_CHARTEXT;
    return OK;
}

static void move(int y, int x) {
    aacurses_y = y;
    aacurses_x = x;
}

static int aacurses_wcwidth(wchar_t wc)
{
    if (! wc)
    {
        return 0;
    }
    if (wc < 0)
    {
        return -1;
    }
    return 1;
}

static int beep(void)
{
    return ERR;
}

static int addch(chtype ch) {
    if (ch & ~A_CHARTEXT)
    {
        int ret;
        attr_t oattr;

        oattr = aacurses_attr;
        attrset((oattr
                 |
                 (ch & ~(A_REVERSE | A_CHARTEXT)))
                ^
                (ch & A_REVERSE));
        ret = addch(ch & A_CHARTEXT);
        attrset(oattr);
        return ret;
    }
    if (aacurses_bitmap)
    {
        int pixel;

        pixel = 192;
        if ((ch == ' ')
            ||
            (ch == '\0')
            ||
            (ch == '\xff'))
        {
            pixel = 0;
        }
        else if ((ch == '\xdf')
                 ||
                 (ch == '\xdc')
                 ||
                 (ch == '\xdd')
                 ||
                 (ch == '\xde'))
        {
            pixel /= 2;
        }
        if (aacurses_attr & (A_STANDOUT | A_BOLD))
        {
            pixel = pixel * 255 / 192;
        }
        if (aacurses_attr & A_DIM)
        {
            pixel /= 2;
        }
        if (aacurses_attr & (A_STANDOUT | A_REVERSE))
        {
            pixel = 255 - pixel;
        }
        aa_putpixel(aacurses_ctx, aacurses_x, aacurses_y, pixel);
    }
    else
    {
        char s[2];

        if ((aacurses_attr & A_ALTCHARSET) && (ch != ' '))
        {
            if (aacurses_use_xfont)
            {
                switch(ch | (aacurses_attr & A_ALTCHARSET))
                {
                case ACS_LRCORNER: ch = ('j' - '_'); break;
                case ACS_URCORNER: ch = ('k' - '_'); break;
                case ACS_ULCORNER: ch = ('l' - '_'); break;
                case ACS_LLCORNER: ch = ('m' - '_'); break;
                case ACS_PLUS: ch = ('n' - '_'); break;
                case ACS_HLINE: ch = ('q' - '_'); break;
                case ACS_LTEE: ch = ('t' - '_'); break;
                case ACS_RTEE: ch = ('u' - '_'); break;
                case ACS_BTEE: ch = ('v' - '_'); break;
                case ACS_TTEE: ch = ('w' - '_'); break;
                case ACS_VLINE: ch = ('x' - '_'); break;
                case ACS_BULLET: ch = ('~' - '_'); break;
                case ACS_CKBOARD: ch = ('a' - '_'); break;
                case ACS_BLOCK:
                    return addch(A_REVERSE | ' ');
                case ACS_DIAMOND:
                default:
                    ch = ('`' - '_');
                }
            }
            else if (aacurses_use_acs)
            {
                switch(ch | (aacurses_attr & A_ALTCHARSET))
                {
                case ACS_LRCORNER: ch = 0xd9; break;
                case ACS_URCORNER: ch = 0xbf; break;
                case ACS_ULCORNER: ch = 0xda; break;
                case ACS_LLCORNER: ch = 0xc0; break;
                case ACS_PLUS: ch = 0xc5; break;
                case ACS_HLINE: ch = 0xc4; break;
                case ACS_LTEE: ch = 0xc3; break;
                case ACS_RTEE: ch = 0xb4; break;
                case ACS_BTEE: ch = 0xc1; break;
                case ACS_TTEE: ch = 0xc2; break;
                case ACS_VLINE: ch = 0xb3; break;
                case ACS_BULLET: ch = aacurses_swapdots ? 0xf9 : 0xfe; break;
                case ACS_CKBOARD: ch = 0xb1; break;
                case ACS_BLOCK: ch = 0xdb; break;
                case ACS_DIAMOND:
                default:
                    ch = 0x04;
                }
            }
            else
            {
                switch(ch | (aacurses_attr & A_ALTCHARSET))
                {
                case ACS_LRCORNER: ch = '+'; break;
                case ACS_URCORNER: ch = '+'; break;
                case ACS_ULCORNER: ch = '+'; break;
                case ACS_LLCORNER: ch = '+'; break;
                case ACS_PLUS: ch = '+'; break;
                case ACS_HLINE: ch = '-'; break;
                case ACS_LTEE: ch = '+'; break;
                case ACS_RTEE: ch = '+'; break;
                case ACS_BTEE: ch = '+'; break;
                case ACS_TTEE: ch = '+'; break;
                case ACS_VLINE: ch = '|'; break;
                case ACS_BULLET: ch = aacurses_swapdots ? '.' : 'o'; break;
                case ACS_CKBOARD: ch = '#'; break;
                case ACS_BLOCK:
                    return addch(A_REVERSE | ' ');
                case ACS_DIAMOND:
                default:
                    ch = '*';
                }
            }
        }
        s[0] = (char) ch;
        s[1] = '\0';
        aa_puts(aacurses_ctx, aacurses_x, aacurses_y, AACURSES_AA_ATTR(aacurses_attr), s);
    }
    aacurses_x ++;
    return OK;
}

static int clrtobot(void) {
    unsigned int x, y;

    for (x = aacurses_x; x < COLS; x ++)
    {
        move(aacurses_y, x);
        addch(' ');
    }
    for (y = aacurses_y + 1; y < LINES; y ++)
        for (x = 0; x < COLS; x ++)
        {
            move(y, x);
            addch(' ');
        }
    return OK;
}

static int clrtoeol(void) {
    unsigned int x;

    for (x = aacurses_x; x < COLS; x ++)
    {
        move(aacurses_y, x);
        addch(' ');
    }
    return OK;
}

static int addstr(const char *s) {
    while (*s)
    {
        addch((int) (unsigned) (unsigned char) *s++);
    }
    return OK;
}

static int mvprintw(int y, int x, const char *s) {
    move(y, x);
    return addstr(s);
}

#ifdef wcwidth
#undef wcwidth
#endif
#define wcwidth aacurses_wcwidth
