#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void confirmation_handler(int sig_num) {
    printf("Got confirmation signal\n");
}

int main(int argc, char** argv) {
    // arguments validation
    if (argc < 3)
    {
        fprintf(stderr, "Expected at least 2 arguments:\n"
                        "PID of process to send signals to\n"
                        "at least one digit from 1 to 5\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    // read catcher's PID
    char *tmp_ptr;
    long catcher_PID = strtol(argv[1], &tmp_ptr, 10);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "Unexpected characters after process PID in: %s\n", argv[1]);
        exit(-1);
    }

    // verify if arguments after PID are correct (digits from 1 to 5)
    // and save them in as longs (easier and safer when using later)
    long modes[argc - 2];
    for (int i = 2; i < argc; ++i) {
        modes[i-2] = strtol(argv[i], &tmp_ptr, 10);
        if (*tmp_ptr != '\0' || modes[i-2] > 5 || modes[i-2] < 1) {
            fprintf(stderr, "Disallowed signal mode (should be an integer from 1 to 5): %s\n", argv[i]);
            exit(-1);
        }
    }

    // set handler
    struct sigaction action;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    action.sa_handler = &confirmation_handler;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("Failed to set handler for SIGUSR1");
        exit(-1);
    }


    // create mask blocking everything except for sigusr1 and sigint
    sigset_t only_usr1_int_mask;
    sigfillset(&only_usr1_int_mask);
    sigdelset(&only_usr1_int_mask, SIGUSR1);
    // so we can easily end process with ctrl+C
    sigdelset(&only_usr1_int_mask, SIGINT);


    printf("Catcher's PID: %ld\n", catcher_PID);
    sigval_t val_to_send;
    for (int i = 0; i < argc - 2; ++i) {
        val_to_send.sival_int = modes[i];
        printf("sending val: %d\n", val_to_send.sival_int);

        if (sigqueue(catcher_PID, SIGUSR1, val_to_send) == -1) {
            perror("Couldn't queue a signal to the catcher");
            exit(-1);
        }

        sigsuspend(&only_usr1_int_mask);

        if (val_to_send.sival_int == 5) {
            printf("I've sent signal with mode 5, which ends catchers process, and received confirmation."
                   "I end this process too...\n");
            exit(0);
        }
    }

    return 0;
}
