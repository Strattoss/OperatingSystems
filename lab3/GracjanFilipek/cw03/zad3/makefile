CC=gcc
CFLAGS=-Wall

all: rec_search.exe

# Warning:
# You can add "make test1 ARG1=~" to the test, but the programm can generate a lot of errors when trying to gollow broken symbolic links.
# Program works correctly, it just shows error for each broken symbolic link.
#
# Results of tests are in file results.txt
test: rec_search.exe
	rm -f results.txt
	make test1 ARG1=.
	make test1 ARG1=..
	make test1 ARG1=../..


test1:
	make test2 ARG2=abc
	make test2 ARG2=asdf
	make test2 ARG2=zadanie
	make test2 ARG2=\\#include

test2:
	echo \#### >> results.txt
	echo look in: $(ARG1) >> results.txt
	echo search for: $(ARG2) >> results.txt
	./rec_search.exe $(ARG1) $(ARG2) >> results.txt
	echo >> results.txt

rec_search.exe: rec_search.c
	$(CC) $(CFLAGS) $<  -o $@

clean:
	rm -f *.o *.exe
