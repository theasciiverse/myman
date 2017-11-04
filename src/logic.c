/* logic.c - various game logic for the MyMan video game
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

#include <ctype.h>
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EINVAL
#ifdef EARG
#define EINVAL EARG
#else
#define EINVAL EDOM
#endif
#endif

#ifdef macintosh
#if TARGET_API_MAC_CARBON
/* Case 1: flat Carbon headers */
#include <Carbon.h>
#else /* ! defined(TARGET_API_MAC_CARBON) */
/* Case 2: Toolbox */
#ifndef ANCIENT_HEADERS
#include <ConditionalMacros.h>
#endif
#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0300)
#include <MacTypes.h>
#else
#include <Types.h>
#endif
#include <Files.h>
#endif /* ! defined(TARGET_API_MAC_CARBON) */
#endif /* ! defined(macintosh) */

#ifdef VMS
#include <unixlib.h>
#endif /* defined(VMS) */

/* MyMan utilities; also defines cruft like __MSDOS__ under some circumstances */
#ifndef MYMAN_UTILS_H_INCLUDED
#include "utils.h"
#endif

#ifndef MYMAN_UTILS_H_INCLUDED
#error "Got wrong \"utils.h\"; add -I{{srcdir}}/inc"
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

#ifndef HUGE_VAL
#define HUGE_VAL 1e500
#endif

static void
ghost_eaten_timer_expired(void)
{
    sprite_register_used[GHOST_SCORE] = 0;
    sprite_register_frame[GHOST_SCORE] ++;
    if ((((unsigned) sprite_register[GHOST_SCORE]) + sprite_register_frame[GHOST_SCORE]) > SPRITE_1600) sprite_register_frame[GHOST_SCORE] = SPRITE_1600 - ((unsigned) sprite_register[GHOST_SCORE]);
    if (sprite_register_used[HERO])
    {
        memcpy((void *) (mazes[maze_level] + rmsg * (maze_w[maze_level] + 1) + cmsg),
               (void *) (blank_mazes[maze_level] + rmsg * (maze_w[maze_level] + 1) + cmsg),
               MIN(msglen, maze_w[maze_level] - cmsg));
        memcpy((void *) (maze_colors[maze_level] + rmsg * (maze_w[maze_level] + 1) + cmsg),
               (void *) (blank_maze_colors[maze_level] + rmsg * (maze_w[maze_level] + 1) + cmsg),
               MIN(msglen, maze_w[maze_level] - cmsg));
        {
            int dirty_i;
            for (dirty_i = 0; dirty_i < msglen; dirty_i ++)
            {
                mark_cell(cmsg + dirty_i, rmsg);
            }
        }
        myman_sfx |= myman_sfx_siren0_up;
    }
    else
    {
        sprite_register_used[HERO] = NET_LIVES ? 1 : 0;
        sprite_register_used[munched] = (NET_LIVES || (munched != HERO)) ? 1 : 0;
    }
}

static int
check_level_transition(void)
{
    int reset = 0;
    static int init = 0;
    int i, n;
    int eyes, mean, blue;
    double gx, gy;

    if (! need_reset)
    {
        if (dying && ! -- dying) {
            reset = 1;
            if (! myman_demo) ++ lives_used;
        } else if ((! ghost_eaten_timer) && winning && ! -- winning) {
            need_reset = 1;
            if (INTERMISSION(level)
                &&
                (intermission < INTERMISSION_N))
            {
                intermission_running = (1 + INTERMISSION_TIME);
                key_buffer = key_buffer_ERR;
                return 1;
            }
        }
    }
    if (need_reset)
    {
        myman_sfx |= myman_sfx_start;
        need_reset = 0;
        reset = 1;
        for (n = 0; n < maze_n; ++ n)
        {
            memcpy((void *)mazes[n],
                   (void *)blank_mazes[n],
                   (maze_w[n] + 1) * maze_h[n] * sizeof(my_uchar));
            memcpy((void *)maze_colors[n],
                   (void *)blank_maze_colors[n],
                   (maze_w[n] + 1) * maze_h[n] * sizeof(my_uchar));
        }
        DIRTY_ALL();
        ignore_delay = 1;
        frameskip = 0;
        dots = 0;
        pellet_timer = 0;
        pellet_time -= PELLET_ADJUST(7 * ONESEC);
        if (level && (FLIP_ALWAYS ||
                      INTERMISSION(level)))
        {
            ++ maze_level;
            maze_level %= maze_n;
            if (! maze_level)
            {
                maze_level = flip_to % maze_n;
            }
            if (FLIP_LOCK && ! maze_level)
            {
                maze_level = maze_n - 1;
            }
        }
        if (INTERMISSION(level) &&
            (intermission < INTERMISSION_N))
        {
            ++ intermission_shown;
            if (intermission_shown >= INTERMISSION_REPEAT(intermission))
            {
                ++ intermission;
                intermission_shown = 0;
            }
        }
        ++ level;
        sprite_register_frame[FRUIT] =
            sprite_register_frame[FRUIT_SCORE] = BONUS(level);
        pellet_time += PELLET_ADJUST(7 * ONESEC);
        if (pellet_time > PELLET_ADJUST(ONESEC))
            pellet_time -= PELLET_ADJUST(ONESEC);
        else
            pellet_time = 0;
        if (! myman_demo)
        {
            if ((mymandelay > SPEEDUP)
                &&
                (mymandelay >= (mindelay + SPEEDUP)))
            {
                mymandelay -= SPEEDUP;
            }
        }
    }
    if ((! init) && (winning || reset)) {
        init = 1;
        if (sprite_register_used[FRUIT])
        {
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
        }
        sprite_register_used[FRUIT] =
            sprite_register_used[FRUIT_SCORE] =
            sprite_register_used[GHOST_SCORE] = 0;
        for (i = 0; i < ghosts; i++) {
            eyes = GHOSTEYES(i);
            mean = MEANGHOST(i);
            blue = BLUEGHOST(i);
            sprite_register_used[eyes] =
                sprite_register_used[mean] =
                sprite_register_used[blue] = 0;
        }
        sprite_register_frame[HERO] = 0;
    }
    if (reset) {
        memset((void *) home_dir, 0, MAXGHOSTS * maze_maxheight * (maze_maxwidth + 1) * sizeof(*home_dir));
        for (i = 0; i < ghosts; i++) {
            eyes = GHOSTEYES(i);
            mean = MEANGHOST(i);
            blue = BLUEGHOST(i);
            sprite_register_used[eyes] =
                sprite_register_used[mean] =
                sprite_register_used[blue] = 0;
            sprite_register_frame[eyes] = (ghost_dir[i] = DIRWRAP(i + 1)) - 1;
            ghost_mem[i] = 0;
            ghost_timer[i] = TWOSECS;
            ghost_man[i] = 0;
            gx = XGHOST;
            gx += COGHOST * gfx_w * ((i == GHOST3) - (i == GHOST0));
            gy = YGHOST;
            gy -= ROGHOST * gfx_h * (i == GHOST1);
            sprite_register_x[mean] =
                (sprite_register_x[eyes] = (int) gx);
            sprite_register_y[mean] =
                (sprite_register_y[eyes] = (int) gy);
        }
        key_buffer = key_buffer_ERR;
        init = 0;
        cycles = 0;
        sprite_register_used[HERO] = 0;
        sprite_register_frame[HERO] = 0;
        sprite_register_x[HERO] = (int) XHERO;
        sprite_register_y[HERO] = (int) YHERO;
        dead =
            deadpan = (int) (YHERO - YGHOST);
        if (dead < 0)
            dead = -dead;
        if (dead < 2)
            dead = 2;
        maze_putsn_nonblank(rmsg, cmsg, MSG_COLOR, (NET_LIVES && ! myman_demo) ? msg_READY : msg_GAMEOVER, msglen);
        if (player != oldplayer)
        {
            oldplayer = player;
            ghost_eaten_timer = myman_demo ? 0 : ONESEC;
            munched = HERO;
            if (! myman_demo)
            {
                maze_putsn_nonblank(rmsg2, cmsg2, MSG2_COLOR, PLAYER(player), msglen);
            }
        }
    }
    if (dying)
        sprite_register_frame[HERO] = ((DEATHDELAY - dying) >> DEATHSHIFT) % 4;
    return 0;
}

static void
reset_hero(void)
{
    int i;
    int eyes, mean, blue;

    if (deadpan)
        deadpan += (YHERO < YGHOST) - (YHERO > YGHOST);
    if (! -- dead) {
        for (i = 0; i < ghosts; i ++) {
            eyes = GHOSTEYES(i);
            mean = MEANGHOST(i);
            blue = BLUEGHOST(i);
            sprite_register_used[eyes] = NET_LIVES ? VISIBLE_EYES : 0;
            sprite_register_used[mean] = NET_LIVES ? 1 : 0;
            sprite_register_used[blue] = 0;
        }
        sprite_register_used[HERO] = NET_LIVES ? 1 : 0;
        ghost_eaten_timer = ONESEC;
        munched = HERO;
        if (! myman_demo)
        {
            memcpy((void *) (mazes[maze_level] + rmsg2 * (maze_w[maze_level] + 1) + cmsg2),
                   (void *) (blank_mazes[maze_level] + rmsg2 * (maze_w[maze_level] + 1) + cmsg2),
                   MIN(msglen, maze_w[maze_level] - cmsg2));
            memcpy((void *) (maze_colors[maze_level] + rmsg2 * (maze_w[maze_level] + 1) + cmsg2),
                   (void *) (blank_maze_colors[maze_level] + rmsg2 * (maze_w[maze_level] + 1) + cmsg2),
                   MIN(msglen, maze_w[maze_level] - cmsg2));
            {
                int dirty_i;
                for (dirty_i = 0; dirty_i < msglen; dirty_i ++)
                {
                    mark_cell(cmsg2 + dirty_i, rmsg2);
                }
            }
        }
        hero_dir = dirheros[maze_level];
        sprite_register[HERO] = SPRITE_HERO + ((hero_dir == MYMAN_LEFT) ? 4 : (hero_dir == MYMAN_RIGHT) ? 12 : (hero_dir == MYMAN_DOWN) ? 16 : 0);
        if (! use_color)
            if (use_underline)
            {
                my_clear();
                my_clearok(1);
                DIRTY_ALL();
                ignore_delay = 1;
                frameskip = 0;
                visible_frame = 1;
            }
    }
}

static void
monsterscared(int s, int blue, int dir0, int *dirp1, int dir2)
{
    int i, j;
    size_t k;
    int o0, o1, o2, x, y, i1, j1, i2, j2;
    int qx, qy;
    my_uchar mcell;

    /* runnin' scared */
    if (cycles & 1)
    {
        x = sprite_register_x[blue];
        y = sprite_register_y[blue];
        i1 = XTILE(x);
        j1 = YTILE(y);
        i2 = XTILE(sprite_register_x[HERO]);
        j2 = YTILE(sprite_register_y[HERO]);
        if ((i1 == i2) && (j1 <= j2))
        {
            for (j = j1; j <= j2; j ++)
            {
                mcell = (my_uchar) mazes[maze_level][j * (maze_w[maze_level] + 1)+i1];
                if (! ISOPEN((unsigned) mcell))
                    break;
            }
            if (j > j2)
            {
                ghost_mem[s] = hero_dir;
                ghost_timer[s] = (int) MEMDELAY(s);
            }
        }
        else if ((i1 == i2) && (j1 >= j2))
        {
            for (j = j2; j <= j1; j ++)
            {
                mcell = (my_uchar) mazes[maze_level][j * (maze_w[maze_level] + 1)+i1];
                if (! ISOPEN((unsigned) mcell))
                    break;
            }
            if (j > j1)
            {
                ghost_mem[s] = hero_dir;
                ghost_timer[s] = (int) MEMDELAY(s);
            }
        }
        else if ((j1 == j2) && (i1 <= i2))
        {
            for (i = i1; i <= i2; i ++)
            {
                mcell = (my_uchar) mazes[maze_level][j1 * (maze_w[maze_level] + 1)+i];
                if (! ISOPEN((unsigned) mcell))
                    break;
            }
            if (i > i2)
            {
                ghost_mem[s] = hero_dir;
                ghost_timer[s] = (int) MEMDELAY(s);
            }
        }
        else if ((j1 == j2) && (i1 >= i2))
        {
            for (i = i2; i <= i1; i ++)
            {
                mcell = (my_uchar) mazes[maze_level][j1 * (maze_w[maze_level] + 1)+i];
                if (! ISOPEN((unsigned) mcell))
                    break;
            }
            if (i > i1)
            {
                ghost_mem[s] = hero_dir;
                ghost_timer[s] = (int) MEMDELAY(s);
            }
        }
        o0 = o1 = o2 = 1;
        for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
        {
            for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
            {
                mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir0)) * (maze_w[maze_level] + 1)
                                                     +
                                                     XWRAP(i1 + qx + XDIR(dir0))];
                o0 = o0 && ISOPEN((unsigned) mcell);
                mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR((*dirp1))) * (maze_w[maze_level] + 1)
                                                     +
                                                     XWRAP(i1 + qx + XDIR((*dirp1)))];
                o1 = o1 && ISOPEN((unsigned) mcell);
                mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir2)) * (maze_w[maze_level] + 1)
                                                     +
                                                     XWRAP(i1 + qx + XDIR(dir2))];
                o2 = o2 && ISOPEN((unsigned) mcell);
            }
        }
        if (((gfx_w / 2 == x % gfx_w) && XDIR((*dirp1)))
            || ((gfx_h / 2 == y % gfx_h) && YDIR((*dirp1))))
        {
            if (o0 || o2)
            {
                if (! ghost_mem[s])
                {
                    if (o2)
                        (*dirp1) = dir2;
                    else if (! o1)
                        (*dirp1) = dir0;
                }
                else
                {
                    if (o0 && (dir0 != ghost_mem[s]))
                        (*dirp1) = dir0;
                    else if (o2 && (dir2 != ghost_mem[s]))
                        (*dirp1) = dir2;
                    else if (o1 && ((*dirp1) != ghost_mem[s]))
                        ;
                    else if (o0)
                        (*dirp1) = dir0;
                    else
                        (*dirp1) = dir2;
                }
            } else if (! o1)
                (*dirp1) = DIRWRAP((*dirp1) + 2);
        }
        if (XDIR((*dirp1)))
            y = (1 + 2 * j1) * gfx_h / 2;
        else if ((*dirp1))
            x = (1 + 2 * i1) * gfx_w / 2;
        if ((MYMANSQUARE || gfx_reflect) ? (cycles & 2) : 1)
            sprite_register_x[blue] = XPIXWRAP(x + XDIR((*dirp1)));
        if ((gfx_reflect && ! MYMANSQUARE) ? 1 : (cycles & 2))
            sprite_register_y[blue] = YPIXWRAP(y + YDIR((*dirp1)));
        for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
        {
            for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
            {
                k = (s % ghosts*maze_h[maze_level]+YTILE(sprite_register_y[blue] + qy))*(maze_w[maze_level]+1)+XTILE(sprite_register_x[blue] + qx);
                if (! (unsigned) home_dir[k])
                    home_dir[k] = DIRWRAP((*dirp1) + 2);
            }
        }
    }
}

static void
monsterhunting(int s, int eyes, int mean, int dir0, int *dirp1, int dir2)
{
    /* out huntin' */
    int d0, d1, d2;
    int o0, o1, o2, x, y, i, j, i1, j1, i2, j2;
    size_t k;
    int qx, qy;
    my_uchar mcell;

    x = sprite_register_x[mean];
    y = sprite_register_y[mean];
    i1 = XTILE(x);
    j1 = YTILE(y);
    i2 = XTILE(sprite_register_x[HERO]);
    j2 = YTILE(sprite_register_y[HERO]);
    if ((i1 == i2) && (j1 <= j2))
    {
        for (j = j1; j <= j2; j ++)
        {
            mcell = (my_uchar) mazes[maze_level][j * (maze_w[maze_level] + 1)+i1];
            if (! ISOPEN((unsigned) mcell))
                break;
        }
        if (j > j2)
        {
            ghost_mem[s] = hero_dir;
            ghost_timer[s] = (int) MEMDELAY(s);
        }
    }
    else if ((i1 == i2) && (j1 >= j2))
    {
        for (j = j2; j <= j1; j ++)
        {
            mcell = (my_uchar) mazes[maze_level][j * (maze_w[maze_level] + 1)+i1];
            if (! ISOPEN((unsigned) mcell))
                break;
        }
        if (j > j1)
        {
            ghost_mem[s] = hero_dir;
            ghost_timer[s] = (int) MEMDELAY(s);
        }
    }
    else if ((j1 == j2) && (i1 <= i2))
    {
        for (i = i1; i <= i2; i ++)
        {
            mcell = (my_uchar) mazes[maze_level][j1 * (maze_w[maze_level] + 1)+i];
            if (! ISOPEN((unsigned) mcell))
                break;
        }
        if (i > i2)
        {
            ghost_mem[s] = hero_dir;
            ghost_timer[s] = (int) MEMDELAY(s);
        }
    }
    else if ((j1 == j2) && (i1 >= i2))
    {
        for (i = i2; i <= i1; i ++)
        {
            mcell = (my_uchar) mazes[maze_level][j1 * (maze_w[maze_level] + 1)+i];
            if (! ISOPEN((unsigned) mcell))
                break;
        }
        if (i > i1)
        {
            ghost_mem[s] = hero_dir;
            ghost_timer[s] = (int) MEMDELAY(s);
        }
    }
    o0 = o1 = o2 = 1;
    d0 = d1 = d2 = 0;
    for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
    {
        for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
        {
            int qd0, qd1, qd2;

            qd0 = qd1 = qd2 = 0;
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir0)) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR(dir0))];
            o0 = o0 && ISOPEN((unsigned) mcell);
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR((*dirp1))) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR((*dirp1)))];
            o1 = o1 && ISOPEN((unsigned) mcell);
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir2)) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR(dir2))];
            o2 = o2 && ISOPEN((unsigned) mcell);
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR((*dirp1))) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR((*dirp1)))];
            qd1 = ISDOOR((unsigned) mcell);
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx)];
            if (! ISDOOR((unsigned) mcell))
            {
                mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir0)) * (maze_w[maze_level] + 1)
                                                     +
                                                     XWRAP(i1 + qx + XDIR(dir0))];
                qd0 = ISDOOR((unsigned) mcell);
                mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir2)) * (maze_w[maze_level] + 1)
                                                     +
                                                     XWRAP(i1 + qx + XDIR(dir2))];
                qd2 = ISDOOR((unsigned) mcell);
            }
            qd0 = qd0 && (dir0 !=
                          find_home_dir(s,
                                        YWRAP(j1 + qy + YDIR(dir0)),
                                        XWRAP(i1 + qx + XDIR(dir0))))
                && dir0 != GHOST_VRETURN
                && dir0 != MYMAN_LEFT;
            qd2 = qd2 && (dir2 !=
                          find_home_dir(s,
                                        YWRAP(j1 + qy + YDIR(dir2)),
                                        XWRAP(i1 + qx + XDIR(dir2))))
                && dir2 != GHOST_VRETURN
                && dir2 != MYMAN_LEFT;
            qd1 = qd1 && ((*dirp1) !=
                          find_home_dir(s,
                                        YWRAP(j1 + qy + YDIR((*dirp1))),
                                        XWRAP(i1 + qx + XDIR((*dirp1)))))
                && (*dirp1) != GHOST_VRETURN
                && (*dirp1) != MYMAN_LEFT;
            d0 = d0 || qd0;
            d1 = d1 || qd1;
            d2 = d2 || qd2;
        }
    }
    if (((gfx_w / 2 == x % gfx_w) && XDIR((*dirp1)))
        || ((gfx_h / 2 == y % gfx_h) && YDIR((*dirp1))))
    {
        if (d2 || (o2 && (dir2 == ghost_mem[s])))
            (*dirp1) = dir2;
        else if (d1 || (o1 && ((*dirp1) == ghost_mem[s])))
            ;
        else if (d0 || (o0 && (dir0 == ghost_mem[s])))
            (*dirp1) = dir0;
        else if (o1)
            ;
        else if (o2)
            (*dirp1) = dir2;
        else if (o0)
            (*dirp1) = dir0;
        else
            (*dirp1) = DIRWRAP((*dirp1) + 2);
    }
    if (XDIR((*dirp1)))
        y = (1 + 2 * j1) * gfx_h / 2;
    else if ((*dirp1))
        x = (1 + 2 * i1) * gfx_w / 2;
    if ((MYMANSQUARE || gfx_reflect) ? (cycles & 1) : 1)
        sprite_register_x[mean] = (sprite_register_x[eyes] = XPIXWRAP(x + XDIR((*dirp1))));
    if ((gfx_reflect && ! MYMANSQUARE) ? 1 : (cycles & 1))
        sprite_register_y[mean] = (sprite_register_y[eyes] = YPIXWRAP(y + YDIR((*dirp1))));
    for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
    {
        for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
        {
            k = (s % ghosts*maze_h[maze_level]+YTILE(sprite_register_y[mean] + qy))*(maze_w[maze_level]+1)+XTILE(sprite_register_x[mean] + qx);
            if (! (unsigned) home_dir[k])
                home_dir[k] = DIRWRAP((*dirp1) + 2);
        }
    }
}

static int
monsterhome(int s, int eyes, int mean, int dir0, int *dirp1, int dir2)
{
    int dx, dy;
    my_uchar d, d1 = 0;
    int o0, o1, o2, x, y, i1, j1;
    size_t k;
    int qx, qy;
    my_uchar mcell;

    /* goin' home */
    ghost_timer[s] = (int) MEMDELAY(s);
    x = sprite_register_x[eyes];
    y = sprite_register_y[eyes];
    i1 = XTILE(x);
    j1 = YTILE(y);
    dx = (int) ((XGHOST - x) / gfx_w);
    dy = (int) (((dx ? YTOP : YGHOST) - y) / gfx_h);
    d = find_home_dir(s, j1, i1);
    if ((unsigned) d)
        d1 = (unsigned) DIRWRAP(find_home_dir(s, YWRAP(j1 + YDIR((unsigned) d)), XWRAP(i1 + XDIR((unsigned) d))) + 2);
    if (((unsigned) d) &&
        (! (((unsigned) d1) &&
            (((unsigned) d1) == (unsigned) d))))
        ghost_mem[s] = (unsigned) d;
    else
    {
        if (dx * dx > dy * dy) {
            if (dx > 0)
                ghost_mem[s] = MYMAN_RIGHT;
            else
                ghost_mem[s] = MYMAN_LEFT;
        }
        else
        {
            if (GHOST_VRETURN == MYMAN_DOWN)
            {
                if (dy < 0)
                    ghost_mem[s] = MYMAN_UP;
                else
                    ghost_mem[s] = MYMAN_DOWN;
            }
            else
            {
                if (dy > 0)
                    ghost_mem[s] = MYMAN_DOWN;
                else
                    ghost_mem[s] = MYMAN_UP;
            }
        }
    }
    if (! (dx || dy))
    {
        sprite_register_used[eyes] = NET_LIVES ? VISIBLE_EYES : 0;
        sprite_register_used[mean] = 1;
        sprite_register_x[mean] = x;
        sprite_register_y[mean] = y;
        sprite_register_frame[eyes] = (ghost_dir[s] = DIRWRAP(s + 1)) - 1;
        ghost_mem[s] = 0;
        return 1;
    }
    o0 = o1 = o2 = 1;
    for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
    {
        for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
        {
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir0)) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR(dir0))];
            o0 = o0 && (ISOPEN((unsigned) mcell)
                        || ISDOOR((unsigned) mcell));
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR((*dirp1))) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR((*dirp1)))];
            o1 = o1 && (ISOPEN((unsigned) mcell)
                        || ISDOOR((unsigned) mcell));
            mcell = (my_uchar) mazes[maze_level][YWRAP(j1 + qy + YDIR(dir2)) * (maze_w[maze_level] + 1)
                                                 +
                                                 XWRAP(i1 + qx + XDIR(dir2))];
            o2 = o2 && (ISOPEN((unsigned) mcell)
                        || ISDOOR((unsigned) mcell));
        }
    }
    if (o2 && (dir2 == ghost_mem[s]))
        (*dirp1) = dir2;
    else if (o1 && ((*dirp1) == ghost_mem[s]))
        ;
    else if (o0 && (dir0 == ghost_mem[s]))
        (*dirp1) = dir0;
    else if (o1)
        ;
    else if (o2)
        (*dirp1) = dir2;
    else if (o0)
        (*dirp1) = dir0;
    else
        (*dirp1) = DIRWRAP((*dirp1) + 2);
    sprite_register_x[mean] = (sprite_register_x[eyes] = XPIXWRAP(x + XDIR((*dirp1))));
    sprite_register_x[eyes] = XPIXWRAP(x + XDIR((*dirp1)));
    sprite_register_y[eyes] = YPIXWRAP(y + YDIR((*dirp1)));
    for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
    {
        for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
        {
            k = (s % ghosts*maze_h[maze_level]+YTILE(sprite_register_y[eyes] + qy))*(maze_w[maze_level]+1)+XTILE(sprite_register_x[eyes] + qx);
            if (! (unsigned) home_dir[k])
                home_dir[k] = DIRWRAP((*dirp1) + 2);
        }
    }
    return 0;
}

static void
monsterlogic(void)
{
    int s;
    int collision_type = 0;
    int eyes, blue, mean;
    int dir0, dir1, dir2;

    for (s = 0; s < ghosts; s ++)
    {
        eyes = GHOSTEYES(s);
        blue = BLUEGHOST(s);
        mean = MEANGHOST(s);
        if ((! ghost_eaten_timer) || ((munched != eyes) && (munched != HERO)))
            sprite_register_frame[blue] =
                (sprite_register_frame[mean] = ((cycles / MYMANFIFTH) & 1) ? 1 : 0);
        if (dying || winning)
        {
            sprite_register_used[eyes] =
                (sprite_register_used[blue] =
                 (sprite_register_used[mean] = 0));
        }
        else if ((collision_type = check_collision(eyes, mean, blue)) != 0)
        {
            if (collision_type == COLLISION_TYPE_HERO)
            {
                s = -1;
                continue;
            }
        }
        else
        {
            dir1 = DIRWRAP(ghost_dir[s]);
            dir0 = DIRWRAP(dir1 + (s != GHOST2) - (s == GHOST2));
            dir2 = DIRWRAP(dir1 - (s != GHOST2) + (s == GHOST2));
            if (sprite_register_used[blue] && ! ghost_eaten_timer)
            {
                monsterscared(s, blue, dir0, &dir1, dir2);
            }
            else if (sprite_register_used[mean] && ! ghost_eaten_timer)
            {
                monsterhunting(s, eyes, mean, dir0, &dir1, dir2);
            }
            else if (sprite_register_used[eyes]
                     &&
                     ((munched != eyes) || (! ghost_eaten_timer))
                     &&
                     ((! ghost_eaten_timer) || ! sprite_register_used[MEANGHOST(s)]))
            {
                if (monsterhome(s, eyes, mean, dir0, &dir1, dir2))
                {
                    continue;
                }
            }
            if ((! ghost_eaten_timer) && ghost_timer[s] && ! -- ghost_timer[s])
            {
                ghost_timer[s] = (int) MEMDELAY(s);
                if (ghost_mem[s])
                {
                    ghost_mem[s] = 0;
                }
                else
                    dir1 = DIRWRAP(ghost_dir[s] + 2);
            }
            ghost_dir[s] = (sprite_register_frame[eyes] = dir1 - 1) + 1;
            collision_type = check_collision(eyes, mean, blue);
            if (collision_type)
            {
                if (collision_type == COLLISION_TYPE_HERO)
                {
                    s = -1;
                    continue;
                }
            }
        }
    }
}

int
gamelogic(void)
{
    int xtile, ytile;
    int x_off, y_off;
    long c = 0;
    int s;
    int qx, qy;
    int eyes, mean, blue;

    xtile = XTILE(sprite_register_x[HERO]);
    ytile = YTILE(sprite_register_y[HERO]);
    x_off = sprite_register_x[HERO] % gfx_w;
    y_off = sprite_register_y[HERO] % gfx_h;
    if (ghost_eaten_timer && ! -- ghost_eaten_timer)
    {
        ghost_eaten_timer_expired();
    }
    if (winning || dying || need_reset)
    {
        return check_level_transition();
    }
    else if (dead && ! ghost_eaten_timer)
    {
        reset_hero();
    }
    else
    {
        int dots_eaten = 0;
        int pellets_eaten = 0;

        if ((BONUSHERO && (score >= BONUSHERO) && ! earned) ||
            ((score >= BONUSHERO2) && (earned == (BONUSHERO ? 1 : 0)))) {
            if (! myman_demo)
            {
                earned ++;
            }
        }
        for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
        {
            for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
            {
                c = (long) (my_uchar) mazes[maze_level][
                    YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                    +
                    XWRAP(xtile + qx)];
                if (ISPELLET(c) || ISDOT(c))
                {
                    ++ dots_eaten;
                    if (ISPELLET(c))
                    {
                        ++ pellets_eaten;
                    }
                    mazes[maze_level][
                        YWRAP(ytile + qy) * (maze_w[maze_level] + 1)
                        +
                        XWRAP(xtile + qx)] = ' ';
                    mark_cell(XWRAP(xtile + qx), YWRAP(ytile + qy));
                }
            }
        }
        if (dots_eaten || pellets_eaten)
        {
            sprite_register_frame[HERO] = 0;
            if (! myman_demo) score += 10 * dots_eaten + 40 * pellets_eaten;
            if (pellets_eaten) {
                myman_sfx |= myman_sfx_pellet;
                points = 200;
                sprite_register_frame[GHOST_SCORE] = 0;
                pellet_timer = pellets_eaten * pellet_time + 1;
                for (s = 0; s < ghosts; s++) {
                    eyes = GHOSTEYES(s);
                    blue = BLUEGHOST(s);
                    mean = MEANGHOST(s);
                    sprite_register_frame[blue] = 0;
                    if (sprite_register_used[mean] || sprite_register_used[blue]) {
                        if (sprite_register_used[mean]) {
                            sprite_register_x[blue] = sprite_register_x[eyes];
                            sprite_register_y[blue] = sprite_register_y[eyes];
                        }
                        sprite_register_used[eyes] = 0;
                        sprite_register_used[mean] = 0;
                        sprite_register_used[blue] = 1;
                        sprite_register[blue] = SPRITE_BLUE;
                        ghost_dir[s] = DIRWRAP(ghost_dir[s] + 2);
                    }
                }
            }
            else
            {
                myman_sfx |= myman_sfx_dot;
            }
            dots += dots_eaten;
            if ((dots == DOTS_FRUIT1) || (dots == DOTS_FRUIT2))
            {
                sprite_register_x[FRUIT] =
                    sprite_register_x[FRUIT_SCORE] = (int) XFRUIT;
                sprite_register_y[FRUIT] =
                    sprite_register_y[FRUIT_SCORE] = (int) YFRUIT;
                sprite_register_used[FRUIT] = 1;
                sprite_register_timer[FRUIT] = FRUITLIFE;
                myman_sfx |= myman_sfx_bonus;
            } else if (dots >= total_dots[maze_level]) {
                munched = HERO;
                winning = 2 * TWOSECS;
                myman_sfx |= myman_sfx_level;
            }
        } else if (sprite_register_used[FRUIT] &&
                   collide(HERO, FRUIT))
        {
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
            sprite_register_used[FRUIT] = 0;
            sprite_register_used[FRUIT_SCORE] = 1;
            sprite_register_timer[FRUIT_SCORE] = TWOSECS;
            if (! myman_demo) score += bonus_score[sprite_register_frame[FRUIT]];
            sprite_register_frame[HERO] = 0;
            myman_sfx |= myman_sfx_fruit;
        }
        if (! ghost_eaten_timer)
        {
            if (XDIR(hero_dir))
                sprite_register_y[HERO] = (1 + 2 * ytile) * gfx_h / 2;
            else if (hero_dir)
                sprite_register_x[HERO] = (1 + 2 * xtile) * gfx_w / 2;
            if (ISZAPLEFT((unsigned) (my_uchar) mazes[maze_level][ytile * (maze_w[maze_level] + 1)+xtile])
                &&
                (! YDIR(hero_dir))
                &&
                (hero_dir == MYMAN_LEFT)) {
                int ii;

                for (ii = 1; ii < maze_w[maze_level] * maze_h[maze_level]; ii ++)
                {
                    if (ISZAPRIGHT((unsigned) (my_uchar) mazes[maze_level][YWRAP(ytile - ii / maze_w[maze_level]) * (maze_w[maze_level] + 1)+XWRAP(xtile - ii % maze_w[maze_level])]))
                    {
                        break;
                    }
                }
                sprite_register_x[HERO] = (1 + 2 * XWRAP(xtile - ii % maze_w[maze_level])) * gfx_w / 2;
                sprite_register_y[HERO] = (1 + 2 * YWRAP(ytile - ii / maze_w[maze_level])) * gfx_h / 2;
            }
            else if (ISZAPRIGHT((unsigned) (my_uchar) mazes[maze_level][ytile * (maze_w[maze_level] + 1)+xtile])
                     &&
                     (! YDIR(hero_dir))
                     &&
                     (hero_dir == MYMAN_RIGHT)) {
                int ii;

                for (ii = 1; ii < maze_w[maze_level] * maze_h[maze_level]; ii ++)
                {
                    if (ISZAPLEFT((unsigned) (my_uchar) mazes[maze_level][YWRAP(ytile + ii / maze_w[maze_level]) * (maze_w[maze_level] + 1)+XWRAP(xtile + ii % maze_w[maze_level])]))
                    {
                        break;
                    }
                }
                sprite_register_x[HERO] = (1 + 2 * XWRAP(xtile + ii % maze_w[maze_level])) * gfx_w / 2;
                sprite_register_y[HERO] = (1 + 2 * YWRAP(ytile + ii / maze_w[maze_level])) * gfx_h / 2;
            }
            else if (ISZAPUP((unsigned) (my_uchar) mazes[maze_level][ytile * (maze_w[maze_level] + 1)+xtile])
                     &&
                     (! XDIR(hero_dir))
                     &&
                     (hero_dir == MYMAN_UP)) {
                int ii;

                for (ii = 1; ii < maze_h[maze_level] * maze_w[maze_level]; ii ++)
                {
                    if (ISZAPDOWN((unsigned) (my_uchar) mazes[maze_level][YWRAP(ytile - ii % maze_h[maze_level]) * (maze_w[maze_level] + 1)+XWRAP(xtile - ii / maze_h[maze_level])]))
                    {
                        break;
                    }
                }
                sprite_register_x[HERO] = (1 + 2 * XWRAP(xtile - ii / maze_h[maze_level])) * gfx_w / 2;
                sprite_register_y[HERO] = (1 + 2 * YWRAP(ytile - ii % maze_h[maze_level])) * gfx_h / 2;
            }
            else if (ISZAPDOWN((unsigned) (my_uchar) mazes[maze_level][ytile * (maze_w[maze_level] + 1)+xtile])
                     &&
                     (! XDIR(hero_dir))
                     &&
                     (hero_dir == MYMAN_DOWN)) {
                int ii;

                for (ii = 1; ii < maze_h[maze_level] * maze_w[maze_level]; ii ++)
                {
                    if (ISZAPUP((unsigned) (my_uchar) mazes[maze_level][YWRAP(ytile + ii % maze_h[maze_level]) * (maze_w[maze_level] + 1)+XWRAP(xtile + ii / maze_h[maze_level])]))
                    {
                        break;
                    }
                }
                sprite_register_x[HERO] = (1 + 2 * XWRAP(xtile + ii / maze_h[maze_level])) * gfx_w / 2;
                sprite_register_y[HERO] = (1 + 2 * YWRAP(ytile + ii % maze_h[maze_level])) * gfx_h / 2;
            }
            else
            {
                my_uchar m3;
                int x3, y3;
                int is_open;

                x3 = xtile + XLEAVING(hero_dir, x_off + XDIR(hero_dir));
                y3 = ytile + YLEAVING(hero_dir, y_off + YDIR(hero_dir));
                is_open = 1;
                for (qx = 0; qx < (QUAD_DOTS ? 2 : 1); qx ++)
                {
                    for (qy = 0; qy < (QUAD_DOTS ? 2 : 1); qy ++)
                    {
                        m3 = (my_uchar)
                            mazes[maze_level][YWRAP(y3 + qy)
                                 *
                                 (maze_w[maze_level] + 1)
                                 +
                                 XWRAP(x3 + qx)];
                        is_open = is_open && ISOPEN((unsigned) m3);
                    }
                }
                if (is_open)
                {
                    sprite_register_x[HERO] = XPIXWRAP(sprite_register_x[HERO] + XDIR(hero_dir) * ((MYMANSQUARE || gfx_reflect) ? (cycles & 1) : 1));
                    sprite_register_y[HERO] = YPIXWRAP(sprite_register_y[HERO] + YDIR(hero_dir) * ((gfx_reflect && ! MYMANSQUARE) ? 1 : (cycles & 1)));
                    sprite_register_frame[HERO] = (cycles / (1 + (MYMANFIFTH / 2))) % 4;
                }
            }
        }
        monsterlogic();
        if (! ghost_eaten_timer)
            for (s = 0; s < SPRITE_REGISTERS; s ++)
            {
                if (sprite_register_used[s] && sprite_register_timer[s])
                    if (! -- sprite_register_timer[s])
                    {
                        sprite_register_used[s] = 0;
                        if (s == FRUIT)
                        {
                            DIRTY_ALL();
                            ignore_delay = 1;
                            frameskip = 0;
                        }
                    }
            }
    }
    return 0;
}

static void
render_inner_vline(int vline0, int r_off, int c_off, int x1, int y1, int rlines, int cols, int *ppause_shown)
{
    int i, j;
    long c = 0, oldc = 0;
    int p, oldp;
    int line, col;
    int vcol0;

    oldp = MY_PEN_NORMAL;
    for (vcol0 = 0;
         ((vcol0 / (use_bitmap ? 2 : 1)) < cols) && ((vcol0 / (use_bitmap ? 2 : 1)) < (reflect ? (gfx_h * maze_h[maze_level]) : (gfx_w * maze_w[maze_level])));
         vcol0++) {
        int vloff, vcoff = 0;
        int xtile, ytile;
        int xtile2, ytile2;
        int x_off, y_off;
        int s;
        int vline1, vcol1;
        int vline2;
        int is_wall = 0;
        int line2, col2;

        vloff = vcol0 % (use_bitmap ? 2 : 1);
        if (! vloff) {
            oldc = 0;
            oldp = MY_PEN_NORMAL;
        }
        vline1 = vline0 + vloff;
        vcol1 = (vcol0 / (use_bitmap ? 2 : 1)) + vcoff;
        vline2 = vline0 + ((vcol0 ^ 1) % (use_bitmap ? 2 : 1));
        if (reflect)
        {
            line = vcol1;
            line2 = vcol1;
            col = vline1;
            col2 = vline2;
        }
        else
        {
            line = vline1;
            line2 = vline2;
            col = vcol1;
            col2 = vcol1;
        }
        p = MY_PEN_NORMAL;
        c = 0;
        xtile = XTILE((i = col + x1));
        ytile = YTILE((j = line + y1));
        xtile2 = XTILE((col2 + x1));
        ytile2 = YTILE((line2 + y1));
        if (! (line || col))
        {
            int nscrolling;

            nscrolling = (i != scroll_offset_x0) ? 2 : (j != scroll_offset_y0) ? 1 : 0;
            scroll_offset_x0 = i;
            scroll_offset_y0 = j;
            if ((scrolling != nscrolling))
            {
                if (! nscrolling)
                {
                    frameskip1 = frameskip;
                    frameskip = (frameskip > frameskip0) ? frameskip0 : frameskip;
                    ignore_delay = 1;
                }
                else
                {
                    frameskip0 = frameskip;
                    frameskip = (frameskip1 > frameskip) ? frameskip1 : frameskip;
                    ignore_delay = 1;
                }
                scrolling = nscrolling;
            }
            if (scrolling)
            {
                DIRTY_ALL();
            }
        }
        if ((paused && ! (snapshot || snapshot_txt))
            &&
            ((vcol1 + (reflect ? r_off : c_off)) >= (cols - tile_w * (int) strlen(PAUSE) + 1) / 2)
            &&
            ((vcol1 + (reflect ? r_off : c_off)) < ((cols - tile_w * (int) strlen(PAUSE) + 1) / 2 + tile_w * (int) strlen(PAUSE)))
            &&
            (cols >= tile_w * (int) strlen(PAUSE))
            &&
            ((reflect ? (maze_h[maze_level] * gfx_h) : (maze_w[maze_level] * gfx_w)) >= tile_w * (int) strlen(PAUSE))
            &&
            ((vline1 + (reflect ? c_off : r_off)) >= (rlines - tile_h + 1) / 2)
            &&
            ((vline1 + (reflect ? c_off : r_off)) < ((rlines - tile_h + 1) / 2 + tile_h))
            &&
            ((reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h)) >= tile_h))
        {
            int pause_x;
            int pause_y;
            my_uchar pause_tile;
            const char *msg_pause = PAUSE;

            (*ppause_shown) = 1;
            pause_x = vcol1 + (reflect ? r_off : c_off) - ((cols - tile_w * (int) strlen(PAUSE) + 1) / 2);
            pause_y = vline1 + (reflect ? c_off : r_off) - ((rlines - tile_h + 1) / 2);
            pause_tile = (my_ulong) (my_uchar) msg_pause[pause_x / tile_w];
            if (tile_u[(unsigned) pause_tile])
            {
                c = (my_ulong) (my_uchar) tile_d[(my_ulong) (my_uchar) msg_pause[pause_x / tile_w]][pause_y * tile_w + (pause_x % tile_w)];
                if (! c)
                {
                    c = ' ';
                }
                if (use_color)
                {
                    p = PAUSE_COLOR;
                }
                else
                {
                    p = MY_PEN_REVERSE;
                }
                mark_cell(xtile, ytile);
                mark_cell(xtile2, ytile2);
            }
        }
        if (IS_CELL_DIRTY(xtile, ytile)
            ||
            IS_CELL_DIRTY(xtile2, ytile2)
            ||
            ISPELLET((unsigned) (my_uchar) (char) mazes[maze_level][ytile * (maze_w[maze_level] + 1)+xtile])
            ||
            ISPELLET((unsigned) (my_uchar) (char) mazes[maze_level][ytile2 * (maze_w[maze_level] + 1)+xtile2])
            ||
            winning)
        {
            if (! c)
            {
                for (s = 0; s < SPRITE_REGISTERS; s ++) {
                    int t, x, y, iseyes;

                    t = ((unsigned) sprite_register[s]) + ((sprite_register_frame[s] < 0) ? (-sprite_register_frame[s]) : sprite_register_frame[s]);
                    iseyes = ((s == GHOSTEYES(UNGHOSTEYES(s)))
                              &&
                              (UNGHOSTEYES(s) >= 0)
                              &&
                              (UNGHOSTEYES(s) < ghosts));
                    if (debug
                        && sprite_register_used[s]
                        && ((x = sprite_register_x[s]) == i)
                        && ((y = sprite_register_y[s]) == j))
                    {
                        if (iseyes)
                        {
                            const char *msg__uldr = ".^<v>";

                            c = msg__uldr[ghost_mem[UNGHOSTEYES(s)]];
                        }
                        else
                        {
                            c = '.';
                        }
                        if (use_color) {
                            p = sprite_c[t];
                            if (! p)
                                p = sprite_register_color[s];
                        }
                        else
                        {
                            if (sprite_c[t] >= 0x10)
                            {
                                p |= MY_PEN_REVERSE;
                            }
                            if ((s == HERO) || (((unsigned) sprite_register[s]) == SPRITE_WHITE) || iseyes)
                            {
                                p |= use_dim_and_bright ? MY_PEN_BOLD : 0;
                                break;
                            }
                            if (((unsigned) sprite_register[s]) == SPRITE_BLUE)
                            {
                                p |= use_underline ? MY_PEN_UNDERLINE : 0;
                                break;
                            }
                        }
                        break;
                    } else if (sprite_register_used[s]
                               && sprite_u[t]
                               && ((x = sprite_register_x[s] - sgfx_w / 2) <= i)
                               && ((x_off = i - x) < sgfx_w)
                               && ((y = sprite_register_y[s] - sgfx_h / 2) <= j)
                               && ((y_off = j - y) < sgfx_h)
                               && ((c = sgfx(t, y_off, x_off)) != 0)) {
                        if (use_color) {
                            p = sprite_c[t];
                            if (! p)
                                p = sprite_register_color[s];
                        }
                        else
                        {
                            if ((sprite_c[t] >= 0x10)
                                &&
                                ! (p & MY_PEN_REVERSE))
                            {
                                p |= MY_PEN_REVERSE;
                            }
                            if ((s == HERO) || (((unsigned) sprite_register[s]) == SPRITE_WHITE) || iseyes)
                            {
                                p |= use_dim_and_bright ? MY_PEN_BOLD : 0;
                                break;
                            }
                            if (((unsigned) sprite_register[s]) == SPRITE_BLUE)
                            {
                                p |= use_underline ? MY_PEN_UNDERLINE : 0;
                                break;
                            }
                        }
                        break;
                    } else if (sprite_register_used[s]
                               && (! sprite_u[t])
                               && ((unsigned) cp437_sprite[t])
                               && tile_u[(my_ulong) (my_uchar) cp437_sprite[t]]
                               && ((x = sprite_register_x[s] - gfx_w / 2) <= i)
                               && ((x_off = i - x) < gfx_w)
                               && ((y = sprite_register_y[s] - gfx_h / 2) <= j)
                               && ((y_off = j - y) < gfx_h)
                               && ((c = gfx((my_ulong) (my_uchar) cp437_sprite[t], y_off, x_off)) != 0))
                    {
                        if (use_color)
                        {
                            p = tile_c[t];
                            if (! p)
                                p = sprite_register_color[s];
                        }
                        else
                        {
                            if ((s == HERO) || (((unsigned) sprite_register[s]) == SPRITE_WHITE) || iseyes)
                            {
                                p |= use_dim_and_bright ? MY_PEN_BOLD : 0;
                                break;
                            }
                            if (((unsigned) sprite_register[s]) == SPRITE_BLUE)
                            {
                                p |= use_underline ? MY_PEN_UNDERLINE : 0;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            if ((! c) && (ytile < maze_h[maze_level]) && (xtile < maze_w[maze_level])) {
                c = maze_visual(maze_level, ytile, xtile);
                {
                    int c_mapped;

                    c_mapped = c;
                    if (c_mapped == ':')
                    {
                        c_mapped = ' ';
                    }
                    else if (c_mapped == 'l')
                    {
                        c_mapped = 179;
                    }
                    else if (c_mapped == '~')
                    {
                        c_mapped = 196;
                    }
                    else if ((c_mapped == 'o') && (! tile_u[c_mapped]))
                    {
                        c_mapped = 254;
                    }
                    while ((! tile_u[c_mapped])
                           &&
                           (((int) (unsigned) fallback_cp437[c_mapped]) != c)
                           &&
                           (((int) (unsigned) fallback_cp437[c_mapped]) != c_mapped))
                    {
                        c_mapped = (my_ulong) (my_uchar) fallback_cp437[c_mapped];
                    }
                    if (tile_u[c_mapped]) {
                        if ((ISWALL(c) && ! ISDOOR(c)) || (c == ' '))
                        {
                            is_wall = 1;
                        }
                        if (use_color)
                        {
                            p = (int) (my_uchar) maze_colors[maze_level][ytile * (maze_w[maze_level] + 1)+xtile];
                            if (! p)
                            {
                                p = tile_c[c_mapped];
                            }
                            if (! p)
                            {
                                if (ISPELLET(c))
                                {
                                    p = PELLET_COLOR ? PELLET_COLOR : DOT_COLOR;
                                }
                                else if (ISDOT(c))
                                {
                                    p = DOT_COLOR;
                                }
                                else if (is_wall)
                                {
                                    p = EFFECTIVE_MORTAR_COLOR;
                                }
                                else if (ISTEXT(c))
                                {
                                    p = TEXT_COLOR;
                                }
                            }
                        }
                        else
                        {
                            if (ISPELLET(c))
                                p |= use_dim_and_bright ? MY_PEN_BOLD : 0;
                            if (use_underline)
                                if (ISWALL(c) && (! ISDOOR(c)))
                                    p |= MY_PEN_UNDERLINE;
                        }
                        if (debug) {
                            int s = WHOSE_HOME_DIR(ytile, xtile);
                            unsigned sf;
                            my_uchar d;
                                            
                            d = home_dir[(s % ghosts*maze_h[maze_level]+ytile)*(maze_w[maze_level]+1)+xtile];
                            c = (((unsigned) d) == MYMAN_UP) ? '^'
                                : (((unsigned) d) == MYMAN_DOWN) ? 'v'
                                : (((unsigned) d) == MYMAN_LEFT) ? '<'
                                : (((unsigned) d) == MYMAN_RIGHT) ? '>'
                                : ISDOT(c) ? ','
                                : ISPELLET(c) ? ';'
                                : ISOPEN(c) ? ' '
                                : ISDOOR(c) ? 'X'
                                : '@';
                            sf = ((unsigned) sprite_register[MEANGHOST(s)]) + sprite_register_frame[MEANGHOST(s)];
                            if (use_color && ((unsigned) d)) {
                                p = sprite_c[sf];
                                if (! p)
                                    p = sprite_register_color[MEANGHOST(s)];
                            }
                            else if ((sprite_c[sf] >= 0x10)
                                     &&
                                     ! (p & MY_PEN_REVERSE))
                            {
                                p |= MY_PEN_REVERSE;
                            }
                        }
                        else
                        {
                            if ((ISPELLET(c) && ((cycles / MYMANFIFTH) & 4) && (! dead) &&
                                 ! (sprite_register_used[HERO] && ghost_eaten_timer)) ||
                                ((winning < ONESEC) && winning && (! myman_intro) && (! intermission_running) && (! myman_start) &&
                                 (ISDOT(c) || ISPELLET(c) || (((winning / MYMANFIFTH) & 4) && ISDOOR(c)))))
                            {
                                is_wall = 0;
                                c_mapped = ' ';
                            }
                            else if ((winning < (2 * TWOSECS)) && ((winning / MYMANFIFTH) & 4) && ! ghost_eaten_timer)
                            {
                                is_wall = 0;
                                if (use_color)
                                    p = 0xF;
                                else
                                    c_mapped = ' ';
                            }
                            c = gfx(c_mapped, j, i);
                            if ((SOLID_WALLS || SOLID_WALLS_BGCOLOR)
                                &&
                                TRANSLATED_WALL_COLOR
                                &&
                                is_wall
                                &&
                                (! (myman_intro || myman_start || intermission_running))
                                &&
                                (! IS_FULLY_NON_INVERTED(xtile, ytile)))
                            {
                                if (SOLID_WALLS
                                    &&
                                    (((c == ' ') && (IS_FULLY_INVERTED(xtile, ytile) || (! IS_INVERTED(xtile, ytile))))
                                     ||
                                     ((c == '\0') && (IS_FULLY_INVERTED(xtile, ytile) || IS_INVERTED(xtile, ytile)))))
                                {
                                    if (! SOLID_WALLS_BGCOLOR)
                                    {
                                        c = '\xdb';
                                    }
                                    else
                                    {
                                        c = ' ';
                                    }
                                    if (use_color)
                                    {
                                        if (TRANSLATED_WALL_COLOR)
                                        {
                                            p = (((my_ulong) (my_uchar) TRANSLATED_WALL_COLOR) * (SOLID_WALLS_BGCOLOR ? 16 : 1)) % 256;
                                        }
                                    }
                                    else
                                    {
                                        if (SOLID_WALLS_BGCOLOR)
                                        {
                                            p |= MY_PEN_REVERSE;
                                        }
                                        if (use_underline)
                                            p |= MY_PEN_UNDERLINE;
                                    }
                                }
                                else if ((c_mapped != ' ')
                                         &&
                                         (! ISNONINVERTABLE(c_mapped))
                                         &&
                                         SOLID_WALLS_BGCOLOR
                                         &&
                                         (((c != '\0') && (IS_FULLY_INVERTED(xtile, ytile) || (! IS_INVERTED(xtile, ytile))))
                                          ||
                                          ((c != ' ') && (IS_FULLY_INVERTED(xtile, ytile) || IS_INVERTED(xtile, ytile)))))
                                {
                                    if (use_color)
                                    {
                                        if (TRANSLATED_WALL_COLOR)
                                        {
                                            int pencode =
                                                (((my_ulong) (my_uchar) EFFECTIVE_MORTAR_COLOR)
                                                 |
                                                 (((my_ulong) (my_uchar) TRANSLATED_WALL_COLOR) * 16)) % 256;
                                            if ((pencode % 16) == (pencode / 16))
                                            {
                                                pencode &= ~0xF;
                                            }
                                            p = pencode;
                                        }
                                    }
                                    else
                                    {
                                        if (SOLID_WALLS_BGCOLOR)
                                        {
                                            p |= MY_PEN_REVERSE;
                                        }
                                        if (use_underline)
                                            p |= MY_PEN_UNDERLINE;
                                    }
                                }
                            }
                        }
                    }
                    else
                        c = ' ';
                }
            }
            if (! c)
                c = ' ';
        }
        if (use_bitmap)
        {
            if (! vloff)
            {
                oldc = c;
                oldp = p;
                continue;
            }
            if (c || oldc)
            {
                if (! c)
                {
                    c = ' ';
                }
                if (! oldc)
                {
                    oldc = ' ';
                }
                while (fallback_cp437[c & 255] != c)
                {
                    c = fallback_cp437[c & 255];
                }
                while (fallback_cp437[oldc & 255] != oldc)
                {
                    oldc = fallback_cp437[oldc & 255];
                }
                if (((p / 16) != (p % 16)) || ((oldp / 16) != (oldp % 16)))
                {
                    oldp = pen_to_color_pen(oldp);
                    p = pen_to_color_pen(p);
                }
                oldc = (oldc == ' ') ? ' ' : '#';
                c = (c == ' ') ? ' ' : '#';
                if (p != oldp)
                {
                    if (c == ' ')
                    {
                        if ((p % 16) != (p / 16))
                        {
                            if (p / 16)
                            {
                                p = p / 16;
                                c = '#';
                            }
                            else
                            {
                                p = 7;
                            }
                        }
                        else
                        {
                            if ((p & (MY_PEN___SPECIAL | MY_PEN_REVERSE)) == MY_PEN_REVERSE)
                            {
                                c = '#';
                                p &= ~MY_PEN_REVERSE;
                            }
                        }
                    }
                    else
                    {
                        if ((p % 16) != (p / 16))
                        {
                            if (p % 16)
                            {
                                p = p % 16;
                            }
                            else
                            {
                                c = ' ';
                                p = 7;
                            }
                        }
                        else
                        {
                            if ((p & (MY_PEN___SPECIAL | MY_PEN_REVERSE)) == MY_PEN_REVERSE)
                            {
                                c = ' ';
                                p &= ~MY_PEN_REVERSE;
                            }
                        }
                    }
                    if (oldc == ' ')
                    {
                        if ((oldp % 16) != (oldp / 16))
                        {
                            if (oldp / 16)
                            {
                                oldp = oldp / 16;
                                oldc = '#';
                            }
                            else
                            {
                                oldp = 7;
                            }
                        }
                        else
                        {
                            if ((oldp & (MY_PEN___SPECIAL | MY_PEN_REVERSE)) == MY_PEN_REVERSE)
                            {
                                oldc = '#';
                                oldp &= ~MY_PEN_REVERSE;
                            }
                        }
                    }
                    else
                    {
                        if ((oldp % 16) != (oldp / 16))
                        {
                            if (oldp % 16)
                            {
                                oldp = oldp % 16;
                            }
                            else
                            {
                                oldc = ' ';
                                oldp = 7;
                            }
                        }
                        else
                        {
                            if ((oldp & (MY_PEN___SPECIAL | MY_PEN_REVERSE)) == MY_PEN_REVERSE)
                            {
                                oldc = ' ';
                                oldp &= ~MY_PEN_REVERSE;
                            }
                        }
                    }
/*
                    if (((oldp / 16) != (oldp % 16)) ? (oldp / 16) : (oldp & MY_PEN_REVERSE)) {
                        oldc = (oldc == '#') ? ' ' : '#';
                        oldp = ((oldp / 16) != (oldp % 16)) ? (oldp / 16) : (oldp & ~MY_PEN_REVERSE);
                    } else {
                        oldc = (oldc == ' ') ? ' ' : '#';
                    }
                    if (((p / 16) != (p % 16)) ? (p / 16) : (p & MY_PEN_REVERSE)) {
                        c = (c == '#') ? '#' : ' ';
                        oldc = (oldc == ' ') ? '#' : ' ';
                        oldp =
                            ((oldp / 16) != (oldp % 16))
                            ?
                            ((oldp / 16) + 16 * (oldp % 16))
                            :
                            (oldp ^ MY_PEN_REVERSE);
                    } else {
                        c = (c == ' ') ? ' ' : '#';
                    }
*/
                    if ((((p / 16) != (p % 16))) || ((oldp / 16) != (oldp % 16)))
                    {
                        if ((c == ' ') && (oldc != ' '))
                        {
                            if ((oldp % 16) != (p / 16))
                            {
                                oldp = p = (oldp % 16) + 16 * (p / 16);
                            }
                            else
                            {
                                oldc = ' ';
                                oldp = p;
                            }
                        }
                        else if ((c == ' ') && (oldc == ' '))
                        {
                            if ((oldp / 16) != (p / 16))
                            {
                                c = '#';
                                oldp = p = (p / 16) + 16 * (oldp / 16);
                            }
                            else
                            {
                                oldp = p;
                            }
                        }
                        else if ((c != ' ') && (oldc == ' '))
                        {
                            if ((oldp / 16) != (p % 16))
                            {
                                oldp = p = (p % 16) + 16 * (oldp / 16);
                            }
                            else
                            {
                                oldc = '#';
                                oldp = p;
                            }
                        }
                        else if ((c != ' ') && (oldc != ' '))
                        {
                            if ((oldp % 16) != (p % 16))
                            {
                                oldc = ' ';
                                oldp = p = (p % 16) + 16 * (oldp % 16);
                            }
                            else
                            {
                                oldp = p;
                            }
                        }
                        if ((((p % 16) % 8) < ((p / 16) % 8)) || (((p % 16) == 0) && ((p / 16) == 8))) {
                            p = (p / 16) + 16 * (p % 16);
                            oldp = (oldp / 16) + 16 * (oldp % 16);
                            c = (c == ' ') ? '#' : ' ';
                            oldc = (oldc == ' ') ? '#' : ' ';
                        }
                    }
                    else
                    {
                        if (oldp & MY_PEN_REVERSE)
                        {
                            oldc = (oldc == '#') ? ' ' : '#';
                            oldp &= ~MY_PEN_REVERSE;
                        }
                        if (p & MY_PEN_REVERSE)
                        {
                            c = (c == '#') ? ' ' : '#';
                            p &= ~MY_PEN_REVERSE;
                        }
                        if (oldc == ' ')
                        {
                            oldp = 0;
                        }
                        if (c == ' ')
                        {
                            p = 0;
                        }
                        oldp = p = (p | oldp);
                    }
                }
                {
                    static const char *blocks = " \xdf\xdc\xdb";
                    c = blocks[(oldc != ' ') + 2 * (c != ' ')];
                }
            }
        }
        oldc = 0;
        oldp = MY_PEN_NORMAL;
        if (c)
        {
            int x, y;

            x = col + c_off;
            y = line + r_off;
            if (reflect)
            {
                my_move((x) / (use_bitmap ? 2 : 1), (y) * (use_fullwidth ? 2 : 1));
            }
            else
            {
                my_move((y) / (use_bitmap ? 2 : 1), (x) * (use_fullwidth ? 2 : 1));
            }
            if ((! use_color)
                &&
                use_underline
                &&
                (p & MY_PEN_UNDERLINE)
                &&
                (! (p & MY_PEN_REVERSE))
                &&
                (c == ' '))
            {
                p &= ~MY_PEN_UNDERLINE;
            }
            my_addch((my_ulong) (my_uchar) c, p);
        }
    }
}

static void
render_vline(int vline0, int c_off, int r_off, int x1, int y1, int rlines, int cols, int *ppause_shown)
{
    long c = 0;
    int line, col;
    int vcol0;

    if ((vline0 + (reflect ? c_off : r_off)) < 0) return;
    if ((vline0 + (reflect ? c_off : r_off)) >= rlines) return;
    if (use_bitmap && ((vline0 + (reflect ? c_off : r_off)) % 1)) return;
    if ((vline0 < 0) || (vline0 >= (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level]))))
    {
        for (vcol0 = 0; (vcol0 < cols) && (vcol0 < (reflect ? (gfx_h * maze_h[maze_level]) : (gfx_w * maze_w[maze_level]))); vcol0++)
        {
            int filler_tile = 0;
            int filler_tile_c = MY_PEN_NORMAL;

            if (snapshot || snapshot_txt || all_dirty)
            {
                filler_tile = ' ';
            }

            if ((reflect ? c_off : r_off) >= (2 * tile_h))
            {
                int player_anchor;

                player_anchor = (reflect ? r_off : c_off) + 7 * tile_w - 1;
                if (player_anchor >= cols) player_anchor = cols - 1;
                line = vline0 + (((reflect ? c_off : r_off) >= (3 * tile_h)) ? (3 * tile_h) : (2 * tile_h));
                if ((line >= 0) && (line < tile_h))
                {
                    col = (vcol0 + (reflect ? r_off : c_off));
                    if ((col >= 0) && (col < cols))
                    {
                        if (col <= player_anchor)
                        {
                            int player_col;
                            /*int player_tile_x;*/
                            my_uchar player_tile;
                            int tmp, tmp2;

                            player_tile = 0;
                            player_col = (player_anchor - col) / tile_w;
                            /*player_tile_x = tile_w - 1 - (player_anchor - col) % tile_w;*/
                            {
                                player_tile = (player_col > 3) ? 0 : '0';
                                if (player_col < 3)
                                {
                                    const char *msg_up_ = "UP ";

                                    player_tile = msg_up_[2 - player_col];
                                }
                                else
                                {
                                    tmp = player * 1000;
                                    tmp2 = player * 1000;
                                    while (player_col)
                                    {
                                        player_col --;
                                        tmp /= 10;
                                        tmp2 /= 10;
                                    }
                                    if (tmp)
                                    {
                                        player_tile = '0' + (tmp % 10);
                                    }
                                    else if (tmp2)
                                    {
                                        player_tile = ' ';
                                    }
                                }
                            }
                            if (((unsigned) player_tile)
                                &&
                                ! ((! intermission_running)
                                   &&
                                   ((((cycles * 2) % TWOSECS) <= ONESEC)
                                    ||
                                    myman_demo
                                    ||
                                    myman_start
                                    ||
                                    myman_intro)
                                   &&
                                   (0 < (NET_LIVES - ((munched == HERO) && dying && sprite_register_used[HERO])))))
                            {
                                player_tile = ' ';
                            }
                            if (((unsigned) player_tile)
                                &&
                                tile_u[(unsigned) player_tile])
                            {
                                filler_tile = player_tile;
                            }
                        }
                    }
                }
            }
            if ((reflect ? c_off : r_off) >= tile_h)
            {
                int score_anchor;

                score_anchor = (reflect ? r_off : c_off) + 7 * tile_w - 1;
                if (score_anchor >= cols) score_anchor = cols - 1;
                line = vline0 + (((reflect ? c_off : r_off) >= (3 * tile_h)) ? (2 * tile_h) : tile_h);
                if ((line >= 0) && (line < tile_h))
                {
                    col = (vcol0 + (reflect ? r_off : c_off));
                    if ((col >= 0) && (col < cols))
                    {
                        if ((col <= score_anchor)
                            &&
                            ! intermission_running)
                        {
                            int score_col;
                            /*int score_tile_x;*/
                            my_uchar score_tile;
                            int tmp;

                            score_col = (score_anchor - col) / tile_w;
                            /*score_tile_x = tile_w - 1 - (score_anchor - col) % tile_w;*/
                            score_tile = (score_col > 1) ? ' ' : '0';
                            tmp = score;
                            while (score_col)
                            {
                                score_col --;
                                tmp /= 10;
                            }
                            if (tmp)
                            {
                                score_tile = '0' + (tmp % 10);
                            }
                            if (tile_u[(unsigned) score_tile] && (' ' != (unsigned) score_tile))
                            {
                                filler_tile = (unsigned) score_tile;
                            }
                        }
                    }
                }
            }
            if (filler_tile && tile_u[filler_tile])
            {
                if (use_color) {
                    filler_tile_c = tile_c[filler_tile];
                    if (! filler_tile_c) filler_tile_c = TEXT_COLOR;
                }
                my_move((vline0 + (reflect ? c_off : r_off)) / (use_bitmap ? 2 : 1), (vcol0 + (reflect ? r_off : c_off)) * (use_fullwidth ? 2 : 1));
                my_addch((my_ulong) (my_uchar) tile_d[filler_tile][((vline0 + (3 * tile_h)) % tile_h) * tile_w + (vcol0 % tile_w)], filler_tile_c);
            }
        }
        return;
    }
    if (((reflect ? c_off : r_off) < tile_h)
        &&
        ((reflect ? r_off : c_off) >= (5 * tile_w))
        &&
        (vline0 < tile_h)
        &&
        (! intermission_running))
    {
        int hud_score_anchor;

        hud_score_anchor = (reflect ? r_off : c_off) - 1;
        for (col = 0; col <= hud_score_anchor; col ++)
        {
            int score_x;
            int score_col;
            int tmp;

            score_x = hud_score_anchor - col;
            score_col = 0;
            tmp = score;
            while (score_col < score_x / tile_w)
            {
                score_col ++;
                tmp /= 10;
            }
            if (tmp
                ||
                (score_col < 2))
            {
                my_uchar score_tile;

                score_tile = (tmp % 10) + '0';
                if (tile_u[(unsigned) score_tile])
                {
                    int score_tile_c = MY_PEN_NORMAL;

                    if (use_color) {
                        score_tile_c = tile_c[(unsigned) score_tile];
                        if (! score_tile_c) score_tile_c = TEXT_COLOR;
                    }
                    my_move((vline0 + (reflect ? c_off : r_off)) / (use_bitmap ? 2 : 1), col * (use_fullwidth ? 2 : 1));
                    my_addch((my_ulong) (my_uchar) tile_d[(unsigned) score_tile][vline0 * tile_w + tile_w - 1 - (score_x % tile_w)], score_tile_c);

                }
            }
        }
    }
    else if (((reflect ? r_off : c_off) >= sprite_w)
             &&
             (! intermission_running)
             &&
             (rlines < ((reflect ? c_off : r_off) + (reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h)) + sprite_h))
             &&
             (rlines >= (tile_h + sprite_h))
             &&
             (((vline0 + sprite_h) >= rlines)
              ||
              ((vline0 + sprite_h) >= (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level])))))
    {
        int hud_line;
        int hud_life_anchor;

        hud_line = vline0 + sprite_h - ((rlines > (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level]))) ? (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level])) : rlines);
        hud_life_anchor = showlives * sprite_w;
        for (col = 0; col < hud_life_anchor; col ++)
        {
            int life_sprite;

            life_sprite = SPRITE_LIFE;
            if (! sprite_u[life_sprite])
            {
                life_sprite = SPRITE_HERO + 4 + 2;
            }
            if (sprite_u[life_sprite])
            {
                c = (my_ulong) (my_uchar) sprite_d[life_sprite][hud_line * sprite_w + (col % sprite_w)];
                if (c)
                {
                    int hud_life_anchor_color = MY_PEN_NORMAL;

                    if (use_color) {
                        hud_life_anchor_color = sprite_c[life_sprite];
                        if (! hud_life_anchor_color)
                        {
                            hud_life_anchor_color = sprite_register_color[HERO];
                        }
                    }
                    else
                    {
                        hud_life_anchor_color |= use_dim_and_bright ? MY_PEN_BOLD : 0;
                        if (sprite_c[life_sprite] >= 0x10)
                        {
                            hud_life_anchor_color |= MY_PEN_REVERSE;
                        }
                    }
                    if ((col + (reflect ? r_off : c_off) - hud_life_anchor) >= 0)
                    {
                        my_move((vline0 + (reflect ? c_off : r_off)) / (use_bitmap ? 2 : 1),
                                (col + (reflect ? r_off : c_off) - hud_life_anchor) * (use_fullwidth ? 2 : 1));
                        my_addch((my_ulong) (my_uchar) c, hud_life_anchor_color);
                    }
                    continue;
                }
            }
        }
    }
    render_inner_vline(vline0, r_off, c_off, x1, y1, rlines, cols, ppause_shown);
    if (((cols - (reflect ? (r_off + maze_h[maze_level] * gfx_h) : (c_off + maze_w[maze_level] * gfx_w))) >= sprite_w)
        &&
        (rlines >= (tile_h + sprite_h))
        &&
        (rlines < ((reflect ? c_off : r_off) + (reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h)) + sprite_h))
        &&
        (((vline0 + sprite_h) >= rlines)
         ||
         ((vline0 + sprite_h) >= (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level])))))
    {
        int hud_line;
        int hud_level_anchor;
        int hud_level_anchor2;

        hud_line = vline0 + sprite_h - ((rlines > (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level]))) ? (reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level])) : rlines);
        hud_level_anchor = reflect ? (r_off + maze_h[maze_level] * gfx_h) : (c_off + maze_w[maze_level] * gfx_w);
        hud_level_anchor2 = hud_level_anchor + sprite_w * ((level > 7) ? 7 : level) - 1;
        if (hud_level_anchor2 >= cols)
        {
            hud_level_anchor2 = cols - 1;
        }
        for (col = hud_level_anchor; col <= hud_level_anchor2; col ++)
        {
            int level_sprite;
            int level_x;

            level_x = col - hud_level_anchor;
            level_sprite = SPRITE_FRUIT + BONUS(level - (level_x / sprite_w));
            if (sprite_u[level_sprite] && ! myman_demo)
            {
                c = (my_ulong) (my_uchar) sprite_d[level_sprite][hud_line * sprite_w + (level_x % sprite_w)];
                if (c)
                {
                    int hud_level_anchor_color = MY_PEN_NORMAL;

                    if (use_color) {
                        hud_level_anchor_color = sprite_c[level_sprite];
                        if (! hud_level_anchor_color)
                        {
                            hud_level_anchor_color = sprite_register_color[FRUIT];
                        }
                    }
                    else
                    {
                        if (sprite_c[level_sprite] >= 0x10)
                        {
                            hud_level_anchor_color |= MY_PEN_REVERSE;
                        }
                    }
                    my_move((vline0 + (reflect ? c_off : r_off)) / (use_bitmap ? 2 : 1), col * (use_fullwidth ? 2 : 1));
                    my_addch((my_ulong) (my_uchar) c, hud_level_anchor_color);
                    continue;
                }
            }
        }
    }
}

void
gamerender(void)
{
    int i;
    long c = 0;
    int x1, y1;
    int r_off, c_off;
    int line, col;
    int vline0;
    int vlines, vcols;
    int rlines;
    int cols;
    int pause_shown;

    if (QUAD_DOTS)
    {
        int s;
        for (s = 0; s < SPRITE_REGISTERS; s ++)
        {
            sprite_register_x[s] += gfx_w - (gfx_w + sgfx_w % 2) / 2;
            sprite_register_y[s] += gfx_h - (gfx_h + sgfx_h % 2) / 2;
        }
    }
    pause_shown = 0;
    {
        int s;
        for (s = 0; s < SPRITE_REGISTERS; s ++)
        {
            if (sprite_register_used[s])
            {
                mark_sprite_register(s);
            }
        }
    }
    if (snapshot || snapshot_txt || all_dirty)
    {
        my_erase();
        DIRTY_ALL();
        ignore_delay = 1;
        frameskip = 0;
    }
    vlines = my_vlines();
    vcols = my_vcols();
    rlines = my_rlines() * (use_bitmap ? 2 : 1);
    cols = my_cols();
    x1 = sprite_register_x[HERO] - vcols / 2;
    y1 = sprite_register_y[HERO] - vlines / 2 - deadpan;
    if (x1 + vcols - (reflect ? 1 : 0) > maze_w[maze_level] * gfx_w)
        x1 = maze_w[maze_level] * gfx_w - (vcols - (reflect ? 1 : 0));
    if (y1 + vlines - (reflect ? 0 : 1) > maze_h[maze_level] * gfx_h)
        y1 = maze_h[maze_level] * gfx_h - (vlines - (reflect ? 0 : 1));
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    r_off = 0;
    c_off = 0;
    if (((gfx_h * maze_h[maze_level] + (reflect ? 0 : (3 * tile_h + sprite_h)))) <= vlines)
        r_off = (vlines - (reflect ? 0 : (3 * tile_h + sprite_h)) - gfx_h * maze_h[maze_level] + 1) / 2 + (reflect ? 0 : (3 * tile_h));
    else if (((gfx_h * maze_h[maze_level] + (reflect ? 0 : (2 * tile_h + sprite_h)))) <= vlines)
        r_off = (vlines - (reflect ? 0 : (2 * tile_h + sprite_h)) - gfx_h * maze_h[maze_level] + 1) / 2 + (reflect ? 0 : (2 * tile_h));
    else if (gfx_h * maze_h[maze_level] <= vlines)
        r_off = (vlines - gfx_h * maze_h[maze_level] + 1) / 2;
    if (r_off < 0) r_off = 0;
    if ((gfx_w * maze_w[maze_level] + (reflect ? (3 * tile_h + sprite_h) : 0)) <= vcols)
        c_off = (vcols - (reflect ? (3 * tile_h + sprite_h) : 0) - gfx_w * maze_w[maze_level] + 1) / 2 + (reflect ? (3 * tile_h) : 0);
    else if ((gfx_w * maze_w[maze_level] + (reflect ? (2 * tile_h + sprite_h) : 0)) <= vcols)
        c_off = (vcols - (reflect ? (2 * tile_h + sprite_h) : 0) - gfx_w * maze_w[maze_level] + 1) / 2 + (reflect ? (2 * tile_h) : 0);
    else if (gfx_w * maze_w[maze_level] <= vcols)
        c_off = (vcols - gfx_w * maze_w[maze_level] + 1) / 2;
    if (c_off < 0) c_off = 0;
    
    my_clearattrs();
    for (vline0 = -(3 * tile_h); (vline0 < rlines) && (vline0 < (sprite_h + ((reflect ? (gfx_w * maze_w[maze_level]) : (gfx_h * maze_h[maze_level]))))); vline0++)
    {
        render_vline(vline0, c_off, r_off, x1, y1, rlines, cols, &pause_shown);
    }
    if (rlines >= ((reflect ? c_off : r_off) + (reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h)) + sprite_h))
    {
        int life_anchor;
        int level_anchor;
        int level_anchor2;

        life_anchor = showlives * sprite_w + (reflect ? r_off : c_off) + 2 * tile_w - 1;
        level_anchor2 = (reflect ? r_off : c_off) + ((reflect ? r_off : c_off) ? (reflect ? (maze_h[maze_level] * gfx_h) : (maze_w[maze_level] * gfx_w)) : cols) - 2 * tile_w - 1;
        level_anchor = level_anchor2 + 1 - ((level > 7) ? 7 : level) * sprite_w;
        while ((level_anchor <= life_anchor)
               &&
               ((level_anchor + 2 * sprite_w - 1) <= level_anchor2))
        {
            level_anchor += sprite_w;
        }
        while ((life_anchor >= level_anchor)
               &&
               ((life_anchor + 1 - (reflect ? r_off : c_off) - 2 * sprite_w) >= 2 * tile_w))
        {
            life_anchor -= sprite_w;
        }
        for (line = 0; line < sprite_h; line ++)
        {
            for (col = 0; col < cols; col ++)
            {
                if ((col - (reflect ? r_off : c_off) >= (2 * tile_w))
                    &&
                    (col <= life_anchor)
                    &&
                    (! intermission_running))
                {
                    int life_sprite;

                    my_move((line + (reflect ? c_off : r_off) + (reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h))) / (use_bitmap ? 2 : 1), col * (use_fullwidth ? 2 : 1));
                    life_sprite = SPRITE_LIFE;
                    if (! sprite_u[life_sprite])
                    {
                        life_sprite = SPRITE_HERO + 4 + 2;
                    }
                    if (sprite_u[life_sprite])
                    {
                        c = (my_ulong) (my_uchar) sprite_d[life_sprite][line * sprite_w + ((col - (reflect ? r_off : c_off) - 2 * tile_w) % sprite_w)];
                        if (c)
                        {
                            int life_sprite_c = MY_PEN_NORMAL;

                            if (use_color) {
                                life_sprite_c = sprite_c[life_sprite];
                                if (! life_sprite_c)
                                {
                                    life_sprite_c = sprite_register_color[HERO];
                                }
                            }
                            else
                            {
                                if (use_dim_and_bright)
                                {
                                    life_sprite_c |= MY_PEN_BOLD;
                                }
                                if (sprite_c[life_sprite] >= 0x10)
                                {
                                    life_sprite_c |= MY_PEN_REVERSE;
                                }
                            }
                            my_addch((my_ulong) (my_uchar) c, life_sprite_c);
                            continue;
                        }
                    }
                } else if ((col <= level_anchor2)
                           &&
                           (col >= level_anchor))
                {
                    int level_sprite;
                    int level_x;

                    my_move((line + (reflect ? c_off : r_off) + (reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h))) / (use_bitmap ? 2 : 1), col * (use_fullwidth ? 2 : 1));
                    level_x = col - level_anchor;
                    level_sprite = SPRITE_FRUIT + BONUS(level - (level_x / sprite_w));
                    if (sprite_u[level_sprite] && ! myman_demo)
                    {
                        c = (my_ulong) (my_uchar) sprite_d[level_sprite][line * sprite_w + (level_x % sprite_w)];
                        if (c)
                        {
                            int level_sprite_c = MY_PEN_NORMAL;

                            if (use_color) {
                                level_sprite_c = sprite_c[level_sprite];
                                if (! level_sprite_c)
                                {
                                    level_sprite_c = sprite_register_color[FRUIT];
                                }
                            }
                            else
                            {
                                if (sprite_c[level_sprite] >= 0x10)
                                {
                                    level_sprite_c |= MY_PEN_REVERSE;
                                }
                            }
                            my_addch((my_ulong) (my_uchar) c, level_sprite_c);
                            continue;
                        }
                    }
                }
            }
        }
    }
    my_choose_pen(MY_PEN_NORMAL);
    if (debug)
    {
        my_move(0, 0);
        for (i = 0; i < MAXFRAMESKIP; i ++)
        {
            if (i <= frameskip)
            {
                my_addstr("\xdb", MY_PEN_NORMAL);
            }
            else
            {
                my_addstr(" ", MY_PEN_NORMAL);
            }
        }
    }
    if (sprite_register_used[FRUIT] && (rlines > 6) && ! use_sound) {
        static const char msg[8][12] = {
            " <  <N>  > ",
            "<  <ONU>  >",
            "  <BONUS>  ",
            " < BONUS > ",
            "<  BONUS  >",
            " > BONUS < ",
            "  >BONUS<  ",
            ">  >ONU<  <"};

        my_move((rlines / (use_bitmap ? 2 : 1)) - 1, 1 * (use_fullwidth ? 2 : 1));
        my_addstr(msg[(cycles / MYMANFIFTH) & 7], MY_PEN_NORMAL);
    }
    if ((! myman_demo)
        &&
        (! myman_intro)
        &&
        (! myman_start)
        &&
        ((rlines > 6) && (cols > 46))
        &&
        (((reflect ? c_off : r_off) < tile_h)
         ||
         ((rlines - ((reflect ? c_off : r_off) + (reflect ? (maze_w[maze_level] * gfx_w) : (maze_h[maze_level] * gfx_h)))) < sprite_h))
        &&
        ((rlines < (tile_h + sprite_h))
         ||
         ((reflect ? r_off : c_off) < (5 * tile_w))
         ||
         ((reflect ? r_off : c_off) < sprite_w)
         ||
         ((cols - (reflect ? (r_off + maze_h[maze_level] * gfx_h) : (c_off + maze_w[maze_level] * gfx_w))) < sprite_w)))
    {
        static char buf[128];

        sprintf(buf,
                " Level: %-10u Lives: %d Score: %-10u ",
                level, NET_LIVES, score);
        my_move((rlines / (use_bitmap ? 2 : 1)) - 1, (cols - 46) * (use_fullwidth ? 2 : 1));
        my_addstr(buf, MY_PEN_NORMAL);
    }
    if (paused && ! (snapshot || snapshot_txt || pause_shown))
    {
        my_showpause();
    }
    {
        int was_inverted;

        was_inverted = snapshot || snapshot_txt;
        my_refresh();
        if (was_inverted)
        {
            DIRTY_ALL();
            ignore_delay = 1;
            frameskip = 0;
        }
        else
        {
            CLEAN_ALL();
        }
    }
    {
        int s;
        for (s = 0; s < SPRITE_REGISTERS; s ++)
        {
            if (sprite_register_used[s])
            {
                mark_sprite_register(s);
            }
        }
    }
    if (QUAD_DOTS)
    {
        int s;
        for (s = 0; s < SPRITE_REGISTERS; s ++)
        {
            sprite_register_x[s] -= gfx_w - (gfx_w + sgfx_w % 2) / 2;
            sprite_register_y[s] -= gfx_h - (gfx_h + sgfx_h % 2) / 2;
        }
    }
}

void
pager_addch(my_ulong c, int p)
{
    int x, y, i, j;
    int plines, pcols;
    int rlines, cols;

    plines = pager_lines();
    pcols = pager_cols();
    rlines = my_rlines();
    cols = my_cols();
    pager_getyx(&y, &x);
    if (x >= pcols)
    {
        x = 0;
        y ++;
    }
    if (y >= plines)
    {
        pager_move(plines - 1, pcols - 1);
    }
    else
    {
        if (pager_big())
        {
            my_ulong cc;
    
            cc = c;
            while ((! tile_u[cc])
                   &&
                   (((my_ulong) (my_uchar) fallback_cp437[cc]) != c)
                   &&
                   (((my_ulong) (my_uchar) fallback_cp437[cc]) != cc))
            {
                cc = (my_ulong) (my_uchar) fallback_cp437[cc];
            }
            c = cc;
            for (j = 0;
                 j < (((y + 1) == plines)
                      ?
                      (rlines - y * pager_tile_h)
                      :
                      pager_tile_h);
                 j ++)
            {
                for (i = 0; i < tile_w; i ++)
                {
                    cc = (my_ulong) (my_uchar) ((j < tile_h) ? tile_d[c][j * tile_w + i] : ' ');
                    if (! cc) cc = (my_ulong) (my_uchar) ' ';
                    my_move(y * pager_tile_h + j,
                            (x * tile_w + i) * (use_fullwidth ? 2 : 1));
                    my_addch(cc,
                             (j < pager_tile_h)
                             ?
                             p
                             :
                             (use_color
                              ?
                              TEXT_COLOR
                              :
                              MY_PEN_NORMAL));
                }
                if ((x + 1) == pcols)
                {
                    for (i = (x + 1) * tile_w; i < cols; i ++)
                    {
                        my_move(y * pager_tile_h + j,
                                i * (use_fullwidth ? 2 : 1));
                        my_addch((my_ulong) (my_uchar) ' ',
                                 use_color
                                 ?
                                 TEXT_COLOR
                                 :
                                 MY_PEN_NORMAL);
                    }
                }
            }
        }
        else
        {
            my_addch(c, p);
        }
        if ((x + 1) < pcols)
        {
            pager_move(y, x + 1);
        }
        else if ((y + 1) < plines)
        {
            pager_move(y + 1, 0);
        }
        else
        {
            pager_move(plines - 1, pcols - 1);
        }
    }
}
