CC = gcc
CFLAGS = -Wall
FNAME = life

all: $(FNAME).exe
	
$(FNAME).exe: grid.c $(FNAME).c
	$(CC) $(CFLAGS) grid.c $(FNAME).c -o $(FNAME).exe -lncursesw

clean:
	rm -f $(FNAME).exe
