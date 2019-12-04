# fareader 0.6.0-dev0

Read a file or stream of sequences in FASTA format:
```c
    struct fareader *r = fareader_new(NULL); /* from stdin */
    char       *h, *s;
     . . .
    while (fareader_next(r, &h, &s)) {
       printf("seq desc is %s\n", h);
       printf("seq text is %s\n", s);
    }
     . . .
    fareader_free(r);
```
Input can be from stdin or from a file. And in either case the input can be gzip
compressed, so there is no need to uncompress a file or stream explicitly before
using it. This is thanks to the magical magic of **zlib** http://zlib.net of
Jean-Loup Gailly and Mark Adler. Additional thanks to @joewalnes for his
**tinytest** [unit test framework](https://github.com/joewalnes/tinytest).
