CC=gcc
CFLAGS=-c -Wall

all: wclib.o
	make static
	make shared

wclib_static.o: wclib.c
	$(CC) $(CFLAGS) $< -o $@

wclib_shared.o: wclib.c
	$(CC) $(CFLAGS) -fPIC $< -o $@

# compile static library
static: wclib_static.o
	ar crs libwclib.a $<

# compile shared library
shared: wclib_shared.o
	$(CC) -shared -fPIC $< -o libwclib.so

partial_clean:
	rm -f *.o

clean:
	make partial_clean
	rm -f *.a *.so
