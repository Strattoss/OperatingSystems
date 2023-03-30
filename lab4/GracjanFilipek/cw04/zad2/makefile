CC=gcc
CFLAGS=-Wall
MAIN_FILE=sigflags_tester
REPORT1=results.txt

all: $(MAIN_FILE).exe

test: $(MAIN_FILE).exe
	rm -f $(REPORT1)
	touch $(REPORT1)

	./$(MAIN_FILE).exe >> $(REPORT1)

$(MAIN_FILE).exe: $(MAIN_FILE).c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe
