#include <stdio.h>
#include "auxiliary.h"

int main(int argc, char** argv) {
    printf("Parent after exec, is signal pending: ");
    check_pending_signal(MY_SIG);

    printf("Parent after exec, raised signal %d\n", MY_SIG);
    raise(MY_SIG);

    printf("Parent after exec, is signal pending: ");
    check_pending_signal(MY_SIG);
}
