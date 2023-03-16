CC=gcc
CFLAGS=-c -Wall
REPORT_NAME=report.txt

# tests
test_all: repl.o libwclib.a libwclib.so
	rm -f $(REPORT_NAME)
	touch $(REPORT_NAME)

	make test_static
	make test_shared
	make test_dynamic

	make partial_clean

	for s in static shared dynamic; do echo "Measurements $$s" >> report.txt ;cat results\_$$s.txt >> report.txt; done

test_static: repl_static
	./repl_static < test_commands.txt > results_static.txt

test_shared: repl_shared
	./repl_shared < test_commands.txt > results_shared.txt

test_dynamic: repl_dynamic
	./repl_dynamic < test_commands.txt > results_dynamic.txt


# normal compilation
all: repl_static repl_shared repl_dynamic

repl.o: repl.c
	$(CC) $(CFLAGS) $< -o $@

wclib_static.o: wclib.c
	$(CC) $(CFLAGS) $< -o $@

wclib_shared.o: wclib.c
	$(CC) $(CFLAGS) -fPIC $< -o $@

libwclib.a: wclib_static.o
	ar crs $@ $<

libwclib.so: wclib_shared.o
	$(CC) -shared -fPIC $< -o $@

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
