#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 128

// Napisać Makefile służący do kompilowania pliku main.c

/*
 * Funkcja 'spawn_fib' powinna utworzyć proces potomny i wywołać w nim
 * program 'main' podając jako argument liczbę 'n'.
 */
void spawn_fib(int n) {
    int child_pid = fork();
    if (child_pid == -1) {
        perror("Failed to fork a process");
        exit(1);
    }
    if (child_pid == 0) {
        // child process
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "%d", n);
        if (execl("./main.exe", "main", buffer, NULL) == -1) {
            perror("Failed to execl");
            exit(1);
        }
    }
}

int get_child_code(void) {
    int status;
    wait(&status);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return -1;
    }
}

int fib(int n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        spawn_fib(n - 1);
        spawn_fib(n - 2);
        return get_child_code() + get_child_code();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fputs("Usage: ./main N\n", stderr);
        exit(-1);
    }
    int n = atoi(argv[1]);
    if (n > 11 || n < 0) {
        fprintf(stderr, "Argument out of range: %d\n", n);
        exit(-1);
    } else {
        return fib(n);
    }
}
