CC=gcc
CFLAGS=-Wall
OTHER=semaphores.c shared_memory.c queue.c

all: hair_salon.exe barber.exe client.exe

hair_salon.exe: hair_salon.c client.c barber.c $(OTHER)
	$(CC) $(CFLAGS) hair_salon.c $(OTHER) -lpthread -o hair_salon.exe

client.exe: client.c $(OTHER)
	$(CC) $(CFLAGS) client.c $(OTHER) -lpthread -o client.exe

barber.exe: barber.c $(OTHER)
	$(CC) $(CFLAGS) barber.c $(OTHER) -lpthread -o barber.exe

clean:
	rm -f *.exe