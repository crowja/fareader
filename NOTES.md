# Dev notes for fareader

*   Requires varstr.c and varstr.h.
*   Look at Makefile to see requirements for -lz.
*   Added "#define \_POSIX\_SOURCE" to fareader.c to deal with compiler
    squawking about missing prototype for fileno(). stdio.h is being included,
    but it seems we need this flag.

Nov 2019

*   Added handling for when zlib is present (define macro HAVE\_ZLIB) and
    otherwise. Currently only slightly tested.

Feb 2020

*   After a bit of testing, it's clear the current approach is not feasible for
    genome-size input. I changed the reading stratgy from gzgetc/getc to
    gzread/fread figuring that might do it. Maybe an improvement, but still not
    even close to the right speed. Without profiling I am guessing the issue is
    in the character-by-character parsing. Maybe try linereader and aggregating
    lines. A quick experiment suggests this might be the way to go.

*   Okay, a bit of profiling with valgrind --tool=callgrind (looking at the log
    with Kcachegrind) shows the greatest time is being spent in varstr_cat(),
    and specifically in the calls to strcat() and strlen(). For the test I ran,
    these amounted to 66.57 and 33.26 percent, respectively. Probably need to be
    using memcpy().

*   wget -P. https://raw.githubusercontent.com/crowja/varstr/master/varstr.c
*   wget -P. https://raw.githubusercontent.com/crowja/varstr/master/varstr.h
*   wget -Pt
    https://raw.githubusercontent.com/joewalnes/tinytest/master/tinytest.h
