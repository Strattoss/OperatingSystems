#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "auxiliary.h"

int check_pending_signal(int signal_no) {
    sigset_t pend;
    if (sigpending(&pend) == -1) {
        perror("Failed to read pending signals\n");
        exit(-1);
    }

    switch (sigismember(&pend, signal_no)) {
        case -1:
            perror("Couldn't read if signal is member of  set\n");
            break;
        case 0:
            printf("NO\n");
            break;
        case 1:
            printf("YES\n");
            break;
    }

    return 0;
}

/* Returns option read in input. If couldn't match existing options - returns -1 */
enum option read_option(const char* input) {
    if (strcmp(input, "ignore") == 0) {
        return IGNORE;
    } else if (strcmp(input, "handler") == 0) {
        return HANDLER;
    } else if (strcmp(input, "mask") == 0) {
        return MASK;
    } else if (strcmp(input, "pending") == 0) {
        return PENDING;
    }
    else {
        return -1;
    }
}

