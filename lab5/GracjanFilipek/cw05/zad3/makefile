CC=gcc
CFLAGS=-Wall
MAIN_FILE=integrate
SECONDARY_FILE=calculate_integral

all: $(MAIN_FILE).exe $(SECONDARY_FILE).exe

test: $(MAIN_FILE).exe $(SECONDARY_FILE).exe
	echo "Increasing number of processes. Time in [ns]" > report.txt
	./$(MAIN_FILE).exe 0.000000005 1 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 2 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 3 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 4 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 5 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 7 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 10 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 13 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 17 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 22 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 27 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 35 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 50 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 100 >> report.txt

	echo "\nIncreasing precision. Time in [ns]" >> report.txt
	./$(MAIN_FILE).exe 0.5 1 >> report.txt
	./$(MAIN_FILE).exe 0.05 1 >> report.txt
	./$(MAIN_FILE).exe 0.005 1 >> report.txt
	./$(MAIN_FILE).exe 0.0005 1 >> report.txt
	./$(MAIN_FILE).exe 0.00005 1 >> report.txt
	./$(MAIN_FILE).exe 0.000005 1 >> report.txt
	./$(MAIN_FILE).exe 0.0000005 1 >> report.txt
	./$(MAIN_FILE).exe 0.000000005 1 >> report.txt
	./$(MAIN_FILE).exe 0.0000000005 1 >> report.txt


$(MAIN_FILE).exe: $(MAIN_FILE).c
	$(CC) $(CFLAGS) $< -o $@


$(SECONDARY_FILE).exe: $(SECONDARY_FILE).c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe
