#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>

#include "common.h"
#include "shared_memory.h"
#include "queue.h"
#include "semaphores.h"

#define BARBER_TOTAL 3
#define SEATS_TOTAL 2
#define QUEUE_SIZE 5
#define CUSTOMERS_TOTAL 6


semaphore_id sem_queue;
semaphore_id sem_seats;
semaphore_id sem_barbers;
semaphore_id buffer_mutex;

void close_semaphores(void) {
    delete_semaphore(sem_queue);
    delete_semaphore(sem_seats);
    delete_semaphore(sem_barbers);
    delete_semaphore(buffer_mutex);
}

void create_semaphores(void) {
    sem_queue =  create_semaphore(QUEUE_LETTER, SEATS_TOTAL);
    sem_seats =  create_semaphore(SEATS_LETTER, 0);
    sem_barbers =  create_semaphore(BARBERS_LETTER, 0);
    buffer_mutex =  create_semaphore(BUFFER_MUTEX_LETTER, 1);
}

void interrupt_handler(int sig_num) {
    close_semaphores();
    destroy_shared_memory(PROJECT_IDENTIFIER);
    exit(sig_num);
}

int main(void) {
    printf("Hair salon starts working. Parameters:\n"
           "\tBarbers: %d\n"
           "\tSeats: %d\n"
           "\tQueue size: %d\n"
           "\tCustomers waiting: %d\n",
            BARBER_TOTAL,
            SEATS_TOTAL,
            QUEUE_SIZE,
            CUSTOMERS_TOTAL);

    signal(SIGINT, interrupt_handler);

    char *shared = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if(shared == NULL) {
        exit(1);
    }
    shared[0] = '\0';

    create_semaphores();

    for (int i=0;i<BARBER_TOTAL;++i)
        if (fork() == 0) {
            if (execl("./barber.exe", "barber", NULL) == -1) {
                perror("Failed to create barber");
            }
        }
    printf("Barbers created\n");


    for (int i=0;i<CUSTOMERS_TOTAL;++i)
        if (fork() == 0) {
            if (execl("./client.exe", "client", NULL) == -1) {
                perror("Failed to create client");
            }
        }
    printf("Clients created\n");
    fflush(stdout);

    while(wait(NULL) > 0);

    if (!destroy_shared_memory(PROJECT_IDENTIFIER)) {
        fprintf(stderr, "Failed to release shared memory.\n");
        exit(1);
    }
    close_semaphores();
    printf("Hair salon closed.\n");
    return 0;
}
