CC=gcc
CFLAGS=-Wall
SERVER_FILE=server
CLIENT_FILE=client
UTILITY_FILE=utility

all: $(SERVER_FILE).exe $(CLIENT_FILE).exe

test: $(SERVER_FILE).exe $(CLIENT_FILE).exe


$(SERVER_FILE).exe: $(SERVER_FILE).c $(UTILITY_FILE).c
	$(CC) $(CFLAGS) $(SERVER_FILE).c $(UTILITY_FILE).c -o $@


$(CLIENT_FILE).exe: $(CLIENT_FILE).c $(UTILITY_FILE).c
	$(CC) $(CFLAGS) $(CLIENT_FILE).c $(UTILITY_FILE).c -o $@

clean:
	rm -f *.o *.exe
