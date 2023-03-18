CC=gcc
CFLAGS=-Wall

all: matron.exe

test: matron.exe
	./matron.exe 4

matron.exe: matron.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe
