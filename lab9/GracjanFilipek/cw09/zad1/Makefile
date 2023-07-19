CC = gcc
CFLAGS = -Wall
FNAME = santa_workshop

all: $(FNAME).exe
	
$(FNAME).exe: $(FNAME).c
	$(CC) $(CFLAGS) $(FNAME).c -o $(FNAME).exe

clean:
	rm -f $(FNAME).exe
