CC=gcc
CFLAGS=-Wall

EXAMPLE_FILE_NAME=example.txt
TMP_FILE_NAME=result.txt
RES_FILE_NAME=pomiar_zad_2.txt

all: replace_1B.exe replace_1024B.exe

test: replace_1B.exe replace_1024B.exe
	rm -f $(RES_FILE_NAME)
	touch $(RES_FILE_NAME)
	echo "Pomiar" >> $(RES_FILE_NAME)
	./replace_1B.exe $(EXAMPLE_FILE_NAME) $(TMP_FILE_NAME) >> $(RES_FILE_NAME)
	./replace_1024B.exe $(EXAMPLE_FILE_NAME) $(TMP_FILE_NAME) >> $(RES_FILE_NAME)


replace_1B.exe: replace_1B.c
	$(CC) $(CFLAGS) $< -o $@

replace_1024B.exe: replace_1024B.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a *so *.exe $(TMP_FILE_NAME)
