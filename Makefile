SHELL = /bin/sh

GCC_STRICT_FLAGS = -pedantic -ansi -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -O2
OTHER_SOURCE =
OTHER_INCLUDE = -I.
CPPFLAGS = -I. $(OTHER_INCLUDE) -DHAVE_ZLIB
#CPPFLAGS = -I. $(OTHER_INCLUDE) -DHAVE_ZLIB -DDEBUG
CFLAGS = $(GCC_STRICT_FLAGS) 
LDFLAGS = -lz
LDFLAGS_EFENCE = -L/usr/local/lib -lefence $(LDFLAGS)
#VALGRIND_FLAGS = --verbose --leak-check=full --undef-value-errors=yes --track-origins=yes
VALGRIND_FLAGS =  --leak-check=summary --undef-value-errors=yes --track-origins=yes

INDENT_FLAGS = -TFILE -Tsize_t -Tuint8_t -Tuint16_t -Tuint32_t -Tuint64_t

.PHONY: check check-examples vcheck echeck indent stamp clean

TESTS = t/test
EXAMPLES = ex/ex_1

fareader.o: fareader.c fareader.h 
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ fareader.c

varstr.o: varstr.c varstr.h 
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ varstr.c

check: fareader.o varstr.o
	@for i in $(TESTS); \
	do \
	  echo "--------------------"; \
	  echo "Running test $$i ..."; \
	  ( $(CC)    $(CPPFLAGS) $(OTHER_INCLUDE) $(CFLAGS) $(OTHER_SOURCE) \
		-o t/a.out $$i.c fareader.o varstr.o $(LDFLAGS) ) \
	  && ( t/a.out ); \
	done 

check-examples: fareader.o varstr.o
	@for i in $(EXAMPLES); \
	do \
	  echo "--------------------"; \
	  echo "Running example $$i ..."; \
	  ( $(CC) -g $(CPPFLAGS) $(OTHER_INCLUDE) $(CFLAGS) $(OTHER_SOURCE) \
		-o ex/a.out $$i.c fareader.o varstr.o $(LDFLAGS) ) \
	  && ( valgrind $(VALGRIND_FLAGS) ex/a.out < ex/sequence.fasta ); \
	done 

vcheck: fareader.o varstr.o
	@for i in $(TESTS); \
	do \
	  echo "--------------------"; \
	  echo "Running test $$i ..."; \
	  ( $(CC) -g $(CPPFLAGS) $(OTHER_INCLUDE) $(CFLAGS) $(OTHER_SOURCE) \
		-o t/a.out $$i.c fareader.o varstr.o $(LDFLAGS) ) \
	  && ( valgrind $(VALGRIND_FLAGS) t/a.out ); \
	done 

echeck: fareader.o varstr.o
	@for i in $(TESTS); \
	do \
	  echo "--------------------"; \
	  echo "Running test $$i ..."; \
	  ( $(CC)    $(CPPFLAGS) $(OTHER_INCLUDE) $(CFLAGS) $(OTHER_SOURCE) \
		-o t/a.out $$i.c fareader.o varstr.o $(LDFLAGS_EFENCE) ) \
	  && ( LD_PRELOAD=libefence.so ./t/a.out ); \
	done 

indent: stamp
	@indent $(INDENT_FLAGS) fareader.c
	@indent $(INDENT_FLAGS) fareader.h
	@indent $(INDENT_FLAGS) t/test.c
	@indent $(INDENT_FLAGS) ex/ex_1.c

stamp:
	@bash stamper.bash fareader.c
	@bash stamper.bash fareader.h

clean:
	@/bin/rm -f *.o *~ *.BAK *.bak core.*
	@/bin/rm -f t/*.o t/*~ t/*.BAK t/*.bak t/core.* t/a.out
	@/bin/rm -f ex/*.o ex/*~ ex/*.BAK ex/*.bak ex/core.* ex/a.out
