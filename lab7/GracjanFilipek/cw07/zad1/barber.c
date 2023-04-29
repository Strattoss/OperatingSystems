#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"
#include "shared_memory.h"
#include "queue.h"
#include "semaphores.h"

#define HAIRCUT_TIME 2
#define TIMEOUT 1

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

int main(void)
{
    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if (queue == NULL)
    {
        fprintf(stderr, "Failed to open barber queue.\n");
        exit(EXIT_FAILURE);
    }
    open_semaphores();

    printf("\tBarber %d created\n", getpid());

    while (true)
    {
        acquire(sem_barbers);

        acquire(buffer_mutex);
        char haircut = queue_get(queue);
        release(buffer_mutex);

        printf("\tBarber %d making haircut num. %d\n", getpid(), haircut);

        sleep(HAIRCUT_TIME);

        printf("\tBarber %d finished haircut num. %d\n", getpid(), haircut);

        release(sem_seats);
        release(sem_queue);

        if (queue_empty(queue))
        {
            sleep(TIMEOUT);
            if (queue_empty(queue))
                break;
        }
    }
    printf("\tBarber %d. Queue empty. Closing. \n", getpid());

    detach_shared_memory(queue);
    return 0;
}
