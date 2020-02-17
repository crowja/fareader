#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "fareader.h"
#include "t/tinytest.h"
#include "t/tinyhelp.h"

static void
test_constr(void)
{
   struct fareader *z;

   fprintf_test_info(stderr, "test_constr", "fareader_new, fareader_free");
   z = fareader_new(NULL);
   ASSERT("Constructor test", z);
   fareader_free(&z);
   ASSERT_EQUALS(NULL, z);
}

#if 0                                            /* 12 yy */
static void
test_stub(void)
{
   struct fareader *z;
   double      x = 1.23;                    /* TODO */

   fprintf_test_info(stdout, "test_stub()", NULL);
   z = fareader_new();
   ASSERT("Constructor test, pt 1", z);
   fareader_free(&z);
   ASSERT_EQUALS(NULL, z);
}
#endif


int
main(void)
{
   printf("%s\n", fareader_version());

   RUN(test_constr);

   return TEST_REPORT();
}
