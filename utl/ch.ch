#!/usr/bin/env ch
// -*- C -*-
// For running MyMan inside SoftIntegration Ch (does not work yet)

#define RAWCURSES 1
#define MYGETOPT 1
#define HAVE_LANGINFO_H 0

char *__strrchr(const char *s, int c)
{
    int i;
    const char *ret = 0;

    for (i = 0; s[i]; i ++)
    {
        if (s[i] == c)
        {
            ret = s + i;
        }
    }
    return ret;
}

string_t __myman(void)
{
    char *needle, straw;
    char *src;
    string_t code = "";
    string_t srcfile;
    string_t ret;

    src = _argv[0] ? _argv[0] : __FILE__;
    needle = (__strrchr(src, '/') != 0)
        ? (1 + __strrchr(src, '/'))
        : (__strrchr(src, '\\') != 0)
        ? (1 + __strrchr(src, '\\'))
        : src;
    straw = *needle;
    *needle = '\0';
    src = stradd(src, "../");
    ret = stradd(src, "inc",
                 ";",
                 src, "mygetopt",
                 ";",
                 src, "src",
                 ";");
    *needle = straw;
    return ret;
}

#if _CH_ && strcat(_ipath, __myman())

#include "mygetopt.c"

#include "utils.c"
#include "logic.c"
#include "myman.c"

#endif
