CC=gcc
CFLAGS=-Wall

all:
	make client.exe
	make server.exe

client.exe: client.c
	$(CC) $(CFLAGS) client.c -o client.exe -lpthread

server.exe: server.c
	$(CC) $(CFLAGS) server.c -o server.exe -lpthread

clean:
	rm -f client.exe server.exe