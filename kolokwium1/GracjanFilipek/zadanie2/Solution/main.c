#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv) {
    pid_t child_pid = fork();
    switch( child_pid ) {
    case 0:
        sleep(3);
        printf("Potomek: %d kończy działanie\n",getpid());
        exit(EXIT_SUCCESS);
    case -1:
        perror("FORK");
        exit (EXIT_FAILURE);
    default:
    {
        if (argc > 1) sleep(atoi(argv[1]));
        /* Wyślij do potomka sygnał SIGINT i obsłuż błąd */
        if (kill(child_pid, SIGINT) == -1) {
            perror("Failed to send signal");
            exit(1);
        }

        //Czekaj na zakończenie potomka i pobierz status.
        int wstatus;
        waitpid(child_pid, &wstatus, 0);

        //1) Jeśli potomek zakończył się normalnie (przez exit), wypisz informację wraz wartością statusu i jego PID.
        if (WIFEXITED(wstatus)) {
            printf("Proces potomny o pid:%d zakończył się prawidłowo statusem: %d\n", child_pid, wstatus);
        }

        //2) Jeśli potomek zakończył się sygnałem, zwróć taką informację (wartość sygnału)
	    else {
            printf("Proces potomny NIE zakończył się prawidłowo. Status / sygnał: %d\n", wstatus);
        }

	}
 }
  return 1;
}
