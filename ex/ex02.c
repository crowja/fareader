#include <stdlib.h>
#include <stdio.h>
#include "fareader.h"

int
main(int argc, char *argv[])
{
   struct fareader *r = fareader_new(NULL); /* from stdin */
   char       *h, *s;

   if (NULL == r) {
      printf("Could not get an fareader\n");
      exit(0);
   }
   else
      printf("Success getting an fareader\n");

   while (fareader_next(r, &h, &s)) {
      printf(">%s\n", h);
      printf("%s\n", s);
   }

   fareader_free(&r);

   return 0;
}
