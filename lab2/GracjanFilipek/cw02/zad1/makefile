CC=gcc
CFLAGS=-Wall

CHAR_TO_FIND=a
CHAR_TO_REPLACE_WITH=b
EXAMPLE_FILE_NAME=example.txt
TMP_FILE_NAME=result.txt
RES_FILE_NAME=pomiar_zad_1.txt

all: replace_lib.exe replace_sys.exe

test: replace_lib.exe replace_sys.exe
	rm -f $(RES_FILE_NAME)
	touch $(RES_FILE_NAME)
	echo "Pomiar" >> $(RES_FILE_NAME)
	./replace_lib.exe $(CHAR_TO_FIND) $(CHAR_TO_REPLACE_WITH) $(EXAMPLE_FILE_NAME) $(TMP_FILE_NAME) >> $(RES_FILE_NAME)
	./replace_sys.exe $(CHAR_TO_FIND) $(CHAR_TO_REPLACE_WITH) $(EXAMPLE_FILE_NAME) $(TMP_FILE_NAME) >> $(RES_FILE_NAME)


replace_lib.exe: replace_lib.c
	$(CC) $(CFLAGS) $< -o $@

replace_sys.exe: replace_sys.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a *so *.exe $(TMP_FILE_NAME)
