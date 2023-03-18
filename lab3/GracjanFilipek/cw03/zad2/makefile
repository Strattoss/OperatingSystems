CC=gcc
CFLAGS=-Wall

all: exec_ls.exe

test: exec_ls.exe
	./exec_ls.exe empty_dir
	./exec_ls.exe one_file_dir
	./exec_ls.exe multiple_files_dir

exec_ls.exe: exec_ls.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe
