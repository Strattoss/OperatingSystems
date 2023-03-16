#include <stdio.h>
#include <string.h>

void printHello() {
	printf("Hello World\n");
}

void printInt(int n) {
	printf("%d\n", n);
}

void printReversed(char* str) {
	for (int i = strlen(str)-1; i >= 0; i--) {
		printf("%c", str[i]);
	}
	printf("\n");
}

