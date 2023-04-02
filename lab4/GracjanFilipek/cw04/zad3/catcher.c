#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>


int received_signals = 0;
int flag_stay_in_loop = true;


void mode_1() {
    for (int i = 1; i < 101; ++i) {
        printf("%d\n", i);
    }
}

void mode_2() {
    int hours, minutes, seconds;

    time_t now;
    time(&now);

    struct tm *local = localtime(&now);

    hours = local->tm_hour;
    minutes = local->tm_min;
    seconds = local->tm_sec;

    // local time
    printf("Time is %02d:%02d:%02d\n", hours, minutes, seconds);
}

void mode_3() {
    printf("Received %d signals so far\n", received_signals);
}

void mode_4() {
    printf("Start printing loop");
    flag_stay_in_loop = false;
}

void mode_5() {
    printf("Closing catcher...\n");
    exit(0);
}


void catcher_handler(int sig_num, siginfo_t *info, void * uncontext) {
    received_signals++;

    if (kill(info->si_pid, SIGUSR1) == -1) {
        perror("Failed to send confirmation signal");
        exit(-1);
    }

    switch (info->si_value.sival_int) {
        case 1:
            mode_1();
            break;
        case 2:
            mode_2();
            break;
        case 3:
            mode_3();
            break;
        case 4:
            mode_4();
            break;
        case 5:
            mode_5();
            break;
    }

    if (info->si_value.sival_int != 4) {
        flag_stay_in_loop = true;
    }

    printf("\n");
}


int main(int argc, char **argv) {
    // arguments validation
    if (argc != 1)
    {
        fprintf(stderr, "Expected exactly 0 argument:\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    // inform about the process pid
    printf("Sender's PID: %d\n\n", getpid());


    // set handler
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = &catcher_handler;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("Failed to set handler for SIGUSR1");
        exit(-1);
    }


    // mask blocking everything except for sigusr1 and sigint
    sigset_t only_sigusr1;
    sigfillset(&only_sigusr1);
    sigdelset(&only_sigusr1, SIGUSR1);
    // so we can easily end process with ctrl+C
    sigdelset(&only_sigusr1, SIGINT);

    while (1) {
        printf("listening...\n");
        sigsuspend(&only_sigusr1);

        // loop for mode 4
        while (flag_stay_in_loop) {
            mode_2();
            sleep(1);
        }
    }

    return 0;
}
