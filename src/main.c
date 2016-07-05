/*
 * main.c - variant/size-selecting wrapper for starting the MyMan video game
 * Copyright 1997-2006, Benjamin C. Wiley Sittler <bsittler@gmail.com>
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
#include <string.h>
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <stdio.h>
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <time.h>
#if HAVE_UNISTD_H
#ifdef MY_UNISTD_H
#include MY_UNISTD_H
#else
#include <unistd.h>
#endif
#endif
#include <stdlib.h>

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

/* Allegro sometimes redefines main */
#ifdef ALLEGROCURSES
#ifndef main
#include <allegro.h>
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
#endif
#endif

/* MyMan utilities; also defines cruft like __MSDOS__ under some circumstances */
#ifndef MYMAN_UTILS_H_INCLUDED
#include "utils.h"
#endif

#ifndef MYMAN_UTILS_H_INCLUDED
#error "Got wrong \"utils.h\"; add -I{{srcdir}}/inc"
#endif

#ifdef XCURSES
#define XCURSES_USAGE \
" [ -- toolkit-options]"
#else
#define XCURSES_USAGE
#endif

#define SUMMARY(progname) \
"Summary: %s [--help] [--version] [options]" \
XCURSES_USAGE \
"\n", \
progname

#define USAGE(progname, variant, size) "" \
"Usage: %s [options]" \
XCURSES_USAGE \
"\n" \
"%s" \
"Defaults: -v %s -z %s\n" \
"", \
progname, \
"-v VARIANT or\n" \
"--variant=VARIANT       select game variant [" MYMANVARIANT "]\n" \
"-z SIZE or\n" \
"--size=SIZE             select tile and sprite size [" MYMANSIZE "]\n" \
"-h or\n" \
"--help                  show this message and exit\n" \
"-k or\n" \
"--keys                  show keybindings and exit\n" \
"-L or\n" \
"--legal                 show legal notice and exit\n" \
"-V or\n" \
"--version               print " MYMAN " version and exit\n" \
"Variants: " MYMANVARIANTS "\n" \
"Sizes:    " MYMANSIZES "\n" \
"" \
, \
defvariant \
, \
defsize

#define MYMANVERSION_INFO \
"" MYMAN " " MYMANVERSION "\n" \
MYMANCOPYRIGHT "\n"

#ifdef BUILTIN_SIZES
const char *builtin_size;
const char *tile_s;
const char *sprite_s;
#endif

#ifdef BUILTIN_VARIANTS
const char *builtin_variant;
const char *maze_s;
const char **maze_d;
const char **maze_c;
extern int main_builtin(int argc, char *argv[], char *envp[]);
#endif

#if defined(__APPLE__) || defined(macintosh)
#ifdef BUILTIN_SIZES
extern void my_mac_fixargs(int *argcp, char ***argvp, const char* progname);
#endif
#endif

int
main(int argc, char *argv[]
#ifndef MAIN_NO_ENVP
     , char *envp[]
#endif
    )
{
    const char *progname = "";
#ifdef MAIN_NO_ENVP
    extern char **environ;
    char **envp = environ;
#endif
    int ret;
    int i, option_index;
    int show_usage = 0;
    static char MYMANVARIANT_str[] = MYMANVARIANT;
    const char *defvariant = MYMANVARIANT_str;
    static char MYMANSIZE_str[] = MYMANSIZE;
    const char *defsize = MYMANSIZE_str;
    char *variant = NULL;
    char *size = NULL;
    char variants[] = MYMANVARIANTS "\0\0";
    char sizes[] = MYMANSIZES "\0\0";
#ifdef PRIVATELIBEXECDIR
    char scratch[] = PRIVATELIBEXECDIR "/\0" MYMANVARIANTS "-" MYMANSIZES;
#endif

    ret = EXIT_FAILURE;
    if ((argc >= 1) && argv[0])
    {
        progname = argv[0];
    }
    while ((variant = strrchr(variants, ' ')) != NULL)
    {
        *variant = '\0';
    }
    while ((size = strrchr(sizes, ' ')) != NULL)
    {
        *size = '\0';
    }

#if defined(__APPLE__) || defined(macintosh)
    /* when launched as a native application under Mac OS X, there may
     * be a bogus process serial number parameter. */
    if ((argc == 2) && (! strncmp(argv[1], "-psn_", strlen("-psn_"))))
    {
        argc = 0;
#ifdef BUILTIN_SIZES
        my_mac_fixargs(&argc, &argv, progname);
#endif
        if (argc == 0)
        {
            argc = 1;
        }
    }
#ifdef BUILTIN_SIZES
    else
    {
        my_mac_fixargs(&argc, &argv, progname);
    }
#endif
#endif
    myman_expand_args(&argc, &argv, progname);
    while((i = getopt_long(argc, argv, short_options,
                           long_options, &option_index))
          != -1)
        switch(i)
        {
	case 'i':
	case 'I':
        case 'a':
        case 'A':
        case 'e':
        case 'E':
        case 'N':
        case 'B':
        case 'b':
        case 'c':
        case '2':
        case '1':
        case 'd':
        case 'D':
        case 'g':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 'R':
        case 's':
        case 't':
        case 'u':
        case 'U':
        case 'Q':
        case 'M':
        case 'S':
        case 'T':
        case 'f':
        case 'F':
        case 'k':
        case 'L':
        case 'x':
        case 'X':
            break;
        case 'z':
            defsize = optarg;
            break;
        case 'v':
            defvariant = optarg;
            break;
        case 'h':
            show_usage = 1;
            goto nomoreargs;
        case 'V':
            printf(MYMANVERSION_INFO);
            goto nomoreargs;
        default:
            fprintf(stderr, SUMMARY(progname));
            fflush(stderr), exit(MYMAN_EXIT_USAGE);
        }
  nomoreargs:
    for (size = sizes;
         *size != '\0';
         size += strlen(size) + 1)
    {
        if (! strcmp(size, defsize))
        {
            break;
        }
    }
    if (*size == '\0')
    {
        fprintf(stderr, "%s: unrecognized size\n", defsize);
        fprintf(stderr, SUMMARY(progname));
        fflush(stderr), exit(MYMAN_EXIT_USAGE);
    }
    for (variant = variants;
         *variant != '\0';
         variant += strlen(variant) + 1)
    {
        if (! strcmp(variant, defvariant))
        {
            break;
        }
    }
    if (*variant == '\0')
    {
        fprintf(stderr, "%s: unrecognized variant\n", defvariant);
        fprintf(stderr, SUMMARY(progname));
        fflush(stderr), exit(MYMAN_EXIT_USAGE);
    }
#ifdef PRIVATELIBEXECDIR
    strncat(scratch, defvariant, strlen(defvariant));
    strncat(scratch, "-", 1);
    strncat(scratch, defsize, strlen(defsize));
#endif
    if (show_usage)
    {
        printf(USAGE(progname, defvariant, defsize));
        printf("Variant ");
        fflush(stdout);
    }
    fflush(stdout);
    fflush(stderr);
#ifdef BUILTIN_SIZES
    builtin_size = 0;
#define BUILTIN_SIZE(size) \
    do \
    { \
        if (! strcmp(defsize, # size)) \
        { \
            extern char *tile_s_ ## size; \
            extern char *tile_d_ ## size [256]; \
            extern int tile_u_ ## size [256]; \
            extern int tile_c_ ## size [256]; \
            extern int tile_w_ ## size; \
            extern int tile_h_ ## size; \
            extern int tile_f_ ## size; \
            extern const char *tile_a_ ## size; \
            extern char *sprite_s_ ## size; \
            extern char *sprite_d_ ## size [256]; \
            extern int sprite_u_ ## size [256]; \
            extern int sprite_c_ ## size [256]; \
            extern int sprite_w_ ## size; \
            extern int sprite_h_ ## size; \
            extern int sprite_f_ ## size; \
            extern const char *sprite_a_ ## size; \
            tile_s = tile_s_ ## size; \
            memcpy((void *) tile_d, (void *) tile_d_ ## size, sizeof(tile_d)); \
            memcpy((void *) tile_u, (void *) tile_u_ ## size, sizeof(tile_u)); \
            memcpy((void *) tile_c, (void *) tile_c_ ## size, sizeof(tile_c)); \
            tile_w = tile_w_ ## size; \
            tile_h = tile_h_ ## size; \
            tile_f = tile_f_ ## size; \
            tile_a = tile_a_ ## size; \
            sprite_s = sprite_s_ ## size; \
            memcpy((void *) sprite_d, (void *) sprite_d_ ## size, sizeof(sprite_d)); \
            memcpy((void *) sprite_u, (void *) sprite_u_ ## size, sizeof(sprite_u)); \
            memcpy((void *) sprite_c, (void *) sprite_c_ ## size, sizeof(sprite_c)); \
            sprite_w = sprite_w_ ## size; \
            sprite_h = sprite_h_ ## size; \
            sprite_f = sprite_f_ ## size; \
            sprite_f = sprite_f_ ## size; \
            sprite_a = sprite_a_ ## size; \
            builtin_size = # size; \
        } \
    } while(0);
    if (1) { BUILTIN_SIZES }
#endif
#ifdef BUILTIN_VARIANTS
#ifdef MYGETOPT_H
    mygetopt_reset();
#else
    optind = 1;
    opterr = 1;
    optopt = 0;
    optarg = 0;
#endif
    builtin_variant = 0;
#ifdef BUILTIN_SIZES
#define BUILTIN_VARIANT(variant) \
    do \
    { \
        if (builtin_size) \
        { \
            if (! strcmp(defvariant, # variant)) \
            { \
                extern const char *maze_s_ ## variant; \
                extern const char **maze_d_ ## variant; \
                extern const char **maze_c_ ## variant; \
                extern int maze_n_ ## variant; \
                extern int *maze_w_ ## variant; \
                extern int *maze_h_ ## variant; \
                extern int maze_f_ ## variant; \
                extern const char *maze_a_ ## variant; \
                maze_s = maze_s_ ## variant; \
                maze_d = maze_d_ ## variant; \
                maze_c = maze_c_ ## variant; \
                maze_n = maze_n_ ## variant; \
                maze_w = maze_w_ ## variant; \
                maze_h = maze_h_ ## variant; \
                maze_f = maze_f_ ## variant; \
                maze_a = maze_a_ ## variant; \
                builtin_variant = # variant; \
                ret = main_builtin(argc,argv,envp); \
                goto main_done; \
            } \
        } \
    } while(0);
    if (1) { BUILTIN_VARIANTS }
#endif
#endif
#ifdef PRIVATELIBEXECDIR
    execve(scratch, argv, envp);
    perror(scratch);
#else
    fprintf(stderr, "%s: internal error: PRIVATELIBEXECDIR not defined and selected variant not built in\n", progname);
#endif
    fflush(stderr);
  main_done:
#ifdef __TINYC__
#ifndef main
    exit(ret);
#endif
#endif
    return ret;
}
#ifdef END_OF_MAIN
END_OF_MAIN()
#endif
