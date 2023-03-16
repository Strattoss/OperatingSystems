CC=gcc
CFLAGS=-c -Wall
REPORT_NAME=report.txt

# tests
# for s in static shared dynamic; do for o in -O0 -O1 -O2 -O3 -Os; do echo "Measurements $s $o" >> report.txt ;cat results\_$s\_$o.txt >> report.txt; done; done
test_all:
	rm -f $(REPORT_NAME)
	touch $(REPORT_NAME)
	make test OPT=-O0
	make test OPT=-O1
	make test OPT=-O2
	make test OPT=-O3
	make test OPT=-Os
	for s in static shared dynamic; do for o in -O0 -O1 -O2 -O3 -Os; do echo "Measurements $$s $$o" >> report.txt ;cat results\_$$s\_$$o.txt >> report.txt; done; done
	
test:
	$(CC) $(CFLAGS) $(OPT) repl.c -o repl.o

	$(CC) $(CFLAGS) $(OPT) wclib.c -o wclib_static.o
	ar crs libwclib.a wclib_static.o
	make test_static

	$(CC) $(CFLAGS) $(OPT) -fPIC wclib.c -o wclib_shared.o
	$(CC) -shared -fPIC wclib_shared.o -o libwclib.so
	make test_shared
	make test_dynamic

test_static:
	$(CC) -Wall repl.o libwclib.a -o repl_static
	./repl_static < test_commands.txt > results_static_$(OPT).txt

test_shared:
	$(CC) -Wall -L $$PWD -Wall repl.o -lwclib -Wl,-rpath=./ -o repl_shared
	./repl_shared < test_commands.txt > results_shared_$(OPT).txt

test_dynamic:
	$(CC) -Wall -D USE_DL -ldl -Wl,-rpath=./ repl.c  -o repl_dynamic
	./repl_dynamic < test_commands.txt > results_dynamic_$(OPT).txt

# normal compilation
all: repl_static repl_shared repl_dynamic

repl.o: repl.c
	$(CC) $(CFLAGS) repl.c -o repl.o

wclib_static.o: wclib.c
	$(CC) $(CFLAGS) wclib.c -o wclib_static.o

wclib_shared.o: wclib.c
	$(CC) $(CFLAGS) -fPIC wclib.c -o wclib_shared.o

libwclib.a: wclib_static.o
	ar crs libwclib.a wclib_static.o

libwclib.so: wclib_shared.o
	$(CC) -shared -fPIC wclib_shared.o -o libwclib.so

# link repl.o with libwclib.a
repl_static: repl.o libwclib.a
	$(CC) -Wall repl.o libwclib.a -o repl_static

# link repl.o with libwclib.so
repl_shared: repl.o libwclib.so
	$(CC) -Wall -L $$PWD -Wall repl.o -lwclib -Wl,-rpath=./ -o repl_shared

# compile repl.o with libwclib.so
repl_dynamic: repl.c libwclib.so
	$(CC) -Wall -D USE_DL -ldl -Wl,-rpath=./ repl.c  -o repl_dynamic

partial_clean:
	rm -f *.o *.a *.so

clean:
	make partial_clean
	rm -f repl repl_static repl_shared repl_dynamic
