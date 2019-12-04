/**
 *  @file fareader.c
 *  @version 0.6.0-dev0
 *  @date Mon Dec  2 12:05:03 CST 2019
 *  @copyright %COPYRIGHT%
 *  @brief FIXME
 *  @details FIXME
 */

/* Needed because fileno() seems to be POSIX C but not ANSI C */
#define _POSIX_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "varstr.h"
#ifdef   HAVE_ZLIB
#include "zlib.h"
#else
typedef FILE *gzFile;
#define  gzdopen(a, b) fdopen((a), (b))
#define  gzopen(a, b)  fopen((a), (b))
#define  gzclose(a)    fclose((a))
#define  gzgetc(a)     getc((a))
#endif
#include "fareader.h"

#ifdef  DEBUG
#define DEBUG_printf(args) printf args
#else
#define DEBUG_printf(args)
#endif

#ifdef  _IS_NULL
#undef  _IS_NULL
#endif
#define _IS_NULL(p)   ((NULL == (p)) ? (1) : (0))

#ifdef  _FREE
#undef  _FREE
#endif
#define _FREE(p)      ((NULL == (p)) ? (0) : (free((p)), (p) = NULL))

enum states { s_at_start, s_at_end, s_in_h, s_in_h_eol, s_in_s, s_in_s_eol };

struct fareader {
   struct varstr *h;
   struct varstr *s;
   enum states state;
   gzFile      in;
};

/*** fareader_new() ***/

struct fareader *
fareader_new(char *fname)
{
   struct fareader *tp;

   tp = (struct fareader *) malloc(sizeof(struct fareader));
   if (_IS_NULL(tp))
      return NULL;

   if (_IS_NULL(fname))
      tp->in = gzdopen(fileno(stdin), "r");

   else
      tp->in = gzopen(fname, "r");

#ifdef   HAVE_ZLIB
   gzbuffer(tp->in, 1024 * 32);                  /* requires at least zlib-1.2.4 */
#endif

   tp->h = varstr_new();
   varstr_extend(tp->h, 1024);
   tp->s = varstr_new();
   varstr_extend(tp->s, 1024);
   /* varstr_buffersize(tp->s, 1024 * 1024, 1024 * 1024); */
   tp->state = s_at_start;

   return tp;
}


/*** fareader_free() ***/

void
fareader_free(struct fareader *p)
{
   gzclose(p->in);
   varstr_free(p->h);
   varstr_free(p->s);

   _FREE(p);
}


/*** fareader_version() ***/

const char *
fareader_version(void)
{
   static const char version[] = "0.6.0-dev0";
   return version;
}

/*** fareader_buffersize() ***/

int
fareader_buffersize(struct fareader *p, unsigned size, unsigned extend)
{
   return varstr_buffersize(p->s, size, extend);
}

/*** fareader_next() ***/

int
fareader_next(struct fareader *p, char **h, char **s)
{
   unsigned    rc = 1;
   int         c;

   varstr_empty(p->h);
   varstr_empty(p->s);

   if (p->state == s_at_end)
      return 0;

   while ((c = gzgetc(p->in)) > 0) {

      switch (p->state) {

         case s_at_start:
            if (isspace(c)) {
               DEBUG_printf(("MADE IT TO s_at_start\t1\n"));
               continue;
            }
            else if (c == '>' || c == ';') {
               DEBUG_printf(("MADE IT TO s_at_start\t2\n"));
               p->state = s_in_h;
            }
            else {
               DEBUG_printf(("MADE IT TO s_at_start\t3\n"));
               varstr_catc(p->s, c);
               p->state = s_in_s;
            }
            break;

         case s_in_h:
            if (c == '\n') {
               DEBUG_printf(("MADE IT TO s_in_h\t1\n"));
               p->state = s_in_h_eol;
            }
            else {
               DEBUG_printf(("MADE IT TO s_in_h\t2\n"));
               varstr_catc(p->h, c);
            }
            break;

         case s_in_h_eol:
            if (isspace(c)) {
               DEBUG_printf(("MADE IT TO s_in_h_eol\t1\n"));
               continue;
            }
            else if (c == '>' || c == ';') {
               DEBUG_printf(("MADE IT TO s_in_h_eol\t2\n"));
               p->state = s_in_h;                /* and emit FIXME */
               varstr_empty(p->h);
               varstr_empty(p->s);
            }
            else {
               DEBUG_printf(("MADE IT TO s_in_h_eol\t3\n"));
               varstr_catc(p->s, c);
               p->state = s_in_s;
            }
            break;

         case s_in_s:
            if (c == '\n') {
               DEBUG_printf(("MADE IT TO s_in_s\t1\n"));
               p->state = s_in_s_eol;
            }
            else if (isspace(c)) {
               DEBUG_printf(("MADE IT TO s_in_s\t2\n"));
               continue;
            }
            else {
               DEBUG_printf(("MADE IT TO s_in_s\t2 with %c\n", c));
               varstr_catc(p->s, c);
            }
            break;

         case s_in_s_eol:
            if (isspace(c)) {
               DEBUG_printf(("MADE IT TO s_in_s_eol\t1\n"));
               continue;
            }
            else if (c == '>' || c == ';') {
               DEBUG_printf(("MADE IT TO s_in_s_eol\t2\n"));
               p->state = s_in_h;
               goto DONE;
            }
            else {
               DEBUG_printf(("MADE IT TO s_in_s_eol\t3\n"));
               varstr_catc(p->s, c);
               p->state = s_in_s;
            }
            break;

         default:
            break;
      }
   }

   p->state = s_at_end;
   if (strlen(varstr_str(p->h)) == 0 || strlen(varstr_str(p->s)) == 0)
      rc = 0;

 DONE:

   *h = varstr_str(p->h);
   *s = varstr_str(p->s);

   return rc;
}

#undef _IS_NULL
#undef _FREE
