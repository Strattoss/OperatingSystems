CC=gcc
CFLAGS=-c -Wall

all: repl.c wclib.c
	make static
	make shared
	make dynamic

repl.o: repl.c
	$(CC) $(CFLAGS) $< -o $@

wclib_static.o: wclib.c
	$(CC) $(CFLAGS) $< -o $@

wclib_shared.o: wclib.c
	$(CC) $(CFLAGS) -fPIC $< -o $@

libwclib.a: wclib_static.o
	ar crs $@ $<

libwclib.so: wclib_shared.o
	$(CC) -shared -fPIC $< -o $@

# link repl.o with libwclib.a
static: repl.o libwclib.a wclib.h
	$(CC) -Wall repl.o libwclib.a -o repl_static

# link repl.o with libwclib.so
shared: repl.o libwclib.so wclib.h
	$(CC) -Wall -L $$PWD -Wall repl.o -lwclib -Wl,-rpath=./ -o repl_shared

# compile repl.o with libwclib.so
dynamic: repl.c libwclib.so dl_load.h wclib_dl.h
	$(CC) -Wall -D USE_DL -ldl -Wl,-rpath=./ repl.c  -o repl_dynamic

partial_clean:
	rm -f *.o *.a *.so

clean:
	make partial_clean
	rm -f repl repl_static repl_shared repl_dynamic
