# Dev notes for fareader

*   Requires varstr.c and varstr.h.
*   Look at Makefile to see requirements for -lz.
*   Added "#define \_POSIX\_SOURCE" to fareader.c to deal with compiler
    squawking about missing prototype for fileno(). stdio.h is being included,
    but it seems we need this flag.

Nov 2019
*   Added handling for when zlib is present (define macro HAVE\_ZLIB) and 
    otherwise. Currently only slightly tested.
