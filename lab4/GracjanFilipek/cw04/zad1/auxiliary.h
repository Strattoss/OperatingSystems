#include <signal.h>

#ifndef SYSOPY_MISCELLANEOUS_H
#define SYSOPY_MISCELLANEOUS_H

#define MY_SIG SIGUSR1

enum option {IGNORE, HANDLER, MASK, PENDING};

int check_pending_signal(int signal_no);

enum option read_option(const char* input);

#endif //SYSOPY_MISCELLANEOUS_H
