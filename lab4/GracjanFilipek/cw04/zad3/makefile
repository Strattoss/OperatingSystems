CC=gcc
CFLAGS=-Wall
SENDER_NAME=sender
CATCHER_NAME=catcher
REPORT1=results.txt

all: $(SENDER_NAME).exe $(CATCHER_NAME).exe

$(SENDER_NAME).exe: $(SENDER_NAME).c
	$(CC) $(CFLAGS) $< -o $@

$(CATCHER_NAME).exe: $(CATCHER_NAME).c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe
