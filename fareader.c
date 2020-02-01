/**
 *  @file fareader.c
 *  @version 0.9.0-dev0
 *  @date Tue Dec 10 15:17:30 CST 2019
 *  @copyright 2020 John A. Crow <crowja@gmail.com>
 *  @license Unlicense <http://unlicense.org/>
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
#define  gzdopen(a, b)    fdopen((a), (b))
#define  gzopen(a, b)     fopen((a), (b))
#define  gzclose(a)       fclose((a))
#define  gzgetc(a)        getc((a))
#define  gzread(a, b, c)  fread((b), sizeof(char), (c), (a))
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

#ifdef  _RBUFSIZ
#undef  _RBUFSIZ
#endif
#define _RBUFSIZ      32768

enum states { s_at_start, s_at_end, s_in_h, s_in_h_eol, s_in_s, s_in_s_eol };

struct fareader {
   struct varstr *h;
   struct varstr *s;
   enum states state;
   char       *rbuf;                        /* use int because we might want to include EOF in it */
   unsigned    rbuf_len;
   unsigned    rbuf_pos;
   unsigned    rbuf_size;
   gzFile      in;
};

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

   tp->rbuf_len = 0;
   tp->rbuf_pos = 0;
   tp->rbuf_size = 31;
   tp->rbuf = NULL;

   return tp;
}

void
fareader_free(struct fareader **pp)
{
   gzclose((*pp)->in);
   varstr_free(&(*pp)->h);
   varstr_free(&(*pp)->s);
   if (!_IS_NULL((*pp)->rbuf))
      free((*pp)->rbuf);

   _FREE(*pp);
   *pp = NULL;
}

const char *
fareader_version(void)
{
   static const char version[] = "0.9.0-dev0";
   return version;
}

int
fareader_buffersize(struct fareader *p, unsigned size, unsigned extend)
{
   return varstr_buffersize(p->s, size, extend);
}

int
fareader_next(struct fareader *p, char **h, char **s)
{
   unsigned    rc = 1;
   int         c;

   varstr_empty(p->h);
   varstr_empty(p->s);

   if (_IS_NULL(p->rbuf)) {
      p->rbuf = malloc(p->rbuf_size * sizeof(*p->rbuf));
   }

   if (p->state == s_at_end)
      return 0;

   printf("DEBUG In fareader_next, about to do a read\n");

   while (1) {

      if (p->rbuf_pos == p->rbuf_len) {
         p->rbuf_len = fread(p->rbuf, sizeof(*p->rbuf), p->rbuf_size, p->in);
         if (p->rbuf_len == 0) {
            p->state = s_at_end;
            /* TODO are these correct? */
            *h = varstr_str(p->h);
            *s = varstr_str(p->s);

            return strlen(*s) > 0 ? 1 : 0;
            /* return 0; */
         }
         p->rbuf_pos = 0;
         continue;
      }

      c = p->rbuf[p->rbuf_pos];
      p->rbuf_pos++;

#if 0
      printf("DEBUG Just read character %c\n", c);
#endif

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
               goto DONE;                        /* TODO here's where we have a complete sequence */
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
   /**
    * TODO why is the next line here? We don't get here usually, skipping to
    * DONE instead
    */
   if (strlen(varstr_str(p->h)) == 0 || strlen(varstr_str(p->s)) == 0)
      rc = 0;

 DONE:

   *h = varstr_str(p->h);
   *s = varstr_str(p->s);

   return rc;
}

#undef _IS_NULL
#undef _FREE
