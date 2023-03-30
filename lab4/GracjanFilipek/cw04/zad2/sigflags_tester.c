#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_NUM_OF_CHILDREN 5

int child_counter = 0;

// HANDLERS
void siginfo_handler(int sig_num, siginfo_t *sig_info, void *ptr) {
    printf("Handler got signal: %d\n", sig_info->si_signo);
    printf("PID of sending process: %d\n", sig_info->si_pid);
    printf("Passed information: %d\n", sig_info->si_value.sival_int);
}

void sigchild_handler(int sig_num) {
    printf("Signal %d handled\n", sig_num);
}

void nodefer_handler(int sig_num) {
    child_counter++;

    if (child_counter <= MAX_NUM_OF_CHILDREN) {
        printf("Signal stack depth: %d\n", child_counter);
        raise(SIGUSR2);
    }

    child_counter --;
}

// AUXILIARY FUNCTIONS
int send_int_with_sigusr1(int int_to_send) {
    sigval_t signal_value;
    signal_value.sival_int = int_to_send;
    if (sigqueue(getpid(), SIGUSR1, signal_value) == -1) {
        perror("Failed to send a signal");
        return -1;
    }
    return 0;
}

int set_sigaction(int signal_id, int flags, void (*handler) (int)) {
    struct sigaction action;
    action.sa_flags = flags;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);

    if (sigaction(signal_id, &action, NULL) == -1) {
        perror("Failed when performing sigaction");
        return -1;
    }
    return 0;
}

int set_sigaction_siginfo(int signal_id, int flags, void (*handler) (int, siginfo_t*, void*)) {
    struct sigaction action;
    action.sa_flags = flags;
    action.sa_sigaction = handler;
    sigemptyset(&action.sa_mask);

    if (sigaction(signal_id, &action, NULL) == -1) {
        perror("Failed when performing sigaction");
        return -1;
    }
    return 0;
}


int main(int argc, char **argv)
{
    // arguments validation
    if (argc != 1)
    {
        fprintf(stderr, "Expected exactly 0 argument:\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }


    //
    // TEST SA_SIGINFO
    //
    printf("TEST SA_SIGINFO\n");
    if (set_sigaction_siginfo(SIGUSR1, SA_SIGINFO, &siginfo_handler) != -1) {
        send_int_with_sigusr1(1);
        send_int_with_sigusr1(2);
        send_int_with_sigusr1(3);

    }
    else {
        perror("Failed in sigaction with SA_SIGINFO flag");
    }
    printf("\n");


    //
    // TEST SA_RESETHAND
    //
    printf("TEST SA_RESETHAND\n");
    printf("Send signal before setting the handler (should be ignored)\n");
    raise(SIGCHLD);

    printf("Set SIGCHILD handler with SA_RESETHAND flag\n");
    if (set_sigaction(SIGCHLD, SA_RESETHAND, &sigchild_handler) != -1) {
        printf("Send signal\n");
        raise(SIGCHLD);
        printf("Send signal (should be ignored)\n");
        raise(SIGCHLD);
    }
    else {
        perror("Failed in sigaction with SA_RESETHAND flag");
    }
    printf("\n");


    //
    // TEST SA_NODEFER
    //
    printf("TEST SA_NODEFER\n");
    if (set_sigaction(SIGUSR2, SA_NODEFER, &nodefer_handler) != -1) {
        printf("Send signal SIGUSR2 (max stack depth: %d)\n", MAX_NUM_OF_CHILDREN);
        raise(SIGUSR2);
    }
    else {
        perror("Failed in sigaction with SA_RESETHAND flag");
    }
    printf("\n");


    /*
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
    */

    return 0;
}
