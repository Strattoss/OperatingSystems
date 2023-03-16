CC=gcc
CFLAGS=-Wall

all: my_byte_counter.exe

my_byte_counter.exe: my_byte_counter.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a *so *.exe
