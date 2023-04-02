#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "auxiliary.h"


void signal_handler(int signo) {
    printf("Handler got signal: %d\n", signo);
}

void do_sigaction(struct sigaction * action) {
    if (sigaction(MY_SIG, action, NULL) == -1) {
        perror("Failed to do sigaction\n");
        exit(-1);
    }
}

int option_ignore() {
    // preparing signal action
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    do_sigaction(&action);
    return 0;
}

int option_handler() {
    // preparing signal action
    struct sigaction action;
    action.sa_handler = &signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    do_sigaction(&action);
    return 0;
}

int option_mask() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, MY_SIG);

    sigset_t old_mask;

    if (sigprocmask(SIG_BLOCK, &mask, &old_mask) == -1) {
        perror("Failed to block a signal in a mask\n");

        if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1) {
            perror("Failed to restore signal mask\n");
            exit(-1);
        }
    }

    return 0;
}

int option_pending() {
    option_mask();
    return 0;
}


int main(int argc, char **argv)
{
    // arguments validation
    if (argc != 1 + 1)
    {
        fprintf(stderr, "Expected exactly 1 argument:\n"
                        "\tone option required: ignore, handler, mask, pending\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    // option given to the program
    int chosen_option = read_option(argv[1]);

    switch (chosen_option) {
        case IGNORE:
            option_ignore();
            break;
        case HANDLER:
            option_handler();
            break;
        case MASK:
            option_mask();
            break;
        case PENDING:
            option_pending();
            break;
        default:
            fprintf(stderr, "option '%s' doesn't exist\n", argv[1]);
            exit(-1);

    }

    if (chosen_option != MASK) {
        printf("Parent raised signal %d\n", MY_SIG);
        raise(MY_SIG);
    }



    printf("Parent - is the signal pending: ");
    check_pending_signal(MY_SIG);

    fflush(stdout);
    int fork_pid = fork();

    if (fork_pid == -1) {
        perror("Failed to create a child process");
        return 1;
    }

    if (fork_pid > 0) {
        // child process
        if (chosen_option != PENDING) {
            printf("Child raised signal %d\n", MY_SIG);
            raise(MY_SIG);
        }

        printf("Child - is the signal pending: ");
        check_pending_signal(MY_SIG);
    }

    if (fork_pid == 0) {
        // parent process
        fflush(stdout);
        if (chosen_option != HANDLER) {
            if (execl("./to_exec.exe", "to_exec.exe", NULL) == -1) {
                perror("execl failed");
            }
        }
    }

    // waiting for the child process to end
    int child_status = 0;
    wait(&child_status);

    return 0;
}
