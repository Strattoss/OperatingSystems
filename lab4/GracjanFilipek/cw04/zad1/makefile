CC=gcc
CFLAGS=-Wall
MAIN_FILE=sig_tester
SECONDARY_FILE=to_exec
AUXIL_FILE=auxiliary
REPORT1=raport1.txt

all: $(MAIN_FILE).exe $(SECONDARY_FILE).exe

test: $(MAIN_FILE).exe $(SECONDARY_FILE).exe
	rm -f $(REPORT1)
	touch $(REPORT1)

	echo IGNORE >> $(REPORT1)
	./sig_tester.exe ignore >> $(REPORT1)
	echo >> $(REPORT1)

	echo HANDLER >> $(REPORT1)
	./sig_tester.exe handler >> $(REPORT1)
	echo >> $(REPORT1)

	echo MASK >> $(REPORT1)
	./sig_tester.exe mask >> $(REPORT1)
	echo >> $(REPORT1)

	echo PENDING >> $(REPORT1)
	./sig_tester.exe pending >> $(REPORT1)
	echo >> $(REPORT1)

$(MAIN_FILE).exe: $(MAIN_FILE).c $(AUXIL_FILE).c
	$(CC) $(CFLAGS) $(MAIN_FILE).c $(AUXIL_FILE).c -o $@

$(SECONDARY_FILE).exe: $(SECONDARY_FILE).c $(AUXIL_FILE).c
	$(CC) $(CFLAGS) $(SECONDARY_FILE).c $(AUXIL_FILE).c -o $@

clean:
	rm -f *.o *.exe
