#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>

#include "shared_memory.h"
#include "queue.h"
#include "common.h"
#include "semaphores.h"

semaphore_id sem_queue;
semaphore_id sem_seats;
semaphore_id sem_barbers;
semaphore_id buffer_mutex;

void open_semaphores()
{
    sem_queue = open_semaphore(QUEUE_LETTER);
    sem_seats = open_semaphore(SEATS_LETTER);
    sem_barbers = open_semaphore(BARBERS_LETTER);
    buffer_mutex = open_semaphore(BUFFER_MUTEX_LETTER);
}

char random_haircut() {
    return (char) (rand() % 128);
}

int main(void) {
    srand(time(NULL) + getpid());

    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if (queue == NULL) {
        fprintf(stderr, "Client %d failed to open queue.\n", getpid());
        exit(1);
    }

    open_semaphores();

    if (strlen(queue) >= BUFFER_SIZE) {
        printf("\tClient %d. Queue full. Leaving.\n", getpid());
    }

    acquire(sem_queue);

    acquire(buffer_mutex);

    char haircut = random_haircut();
    printf("\tClient %d. New client with haircut num. %d \n", getpid(), haircut);
    queue_put(queue, haircut);
    release(buffer_mutex);
    
    release(sem_barbers);
    acquire(sem_seats);

    printf("\tClient %d. Client done.\n", getpid());

    detach_shared_memory(queue);

    return 0;
}
