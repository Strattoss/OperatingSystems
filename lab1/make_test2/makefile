
all:	main.o sayHello.o saySomething.o
	gcc -Wall main.o sayHello.o saySomething.o -o main

run:	main
	./main

main.o: main.c
	gcc -Wall -c main.c

sayHello.o: sayHello.c
	gcc -Wall -c sayHello.c

saySomething.o: saySomething.c
	gcc -Wall -c saySomething.c

clean:
	rm *o main
