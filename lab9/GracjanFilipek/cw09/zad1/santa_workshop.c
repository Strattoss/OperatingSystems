#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_OF_ELVES 10
#define NUM_OF_REQUIRED_ELVES 3


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t required_num_of_elves_waiting = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_has_been_served = PTHREAD_COND_INITIALIZER;

unsigned int waiting_elves = 0;
unsigned int served_elves = 0;


void *elf_routine(void *arg) {
    while(1) {
        sleep(2 + (rand() % 4));

        pthread_mutex_lock(&mutex);
        if (waiting_elves < NUM_OF_REQUIRED_ELVES ) {
            waiting_elves += 1;
            if (waiting_elves == NUM_OF_REQUIRED_ELVES) {
                printf("ELF %lu wakes up santa\n", pthread_self());
            }
            else {
                printf("Elf %lu is waiting for santa.\n", pthread_self());
            }
            pthread_cond_signal(&required_num_of_elves_waiting);
        }
        else {
            printf("Elf %lu solves the problem themselves\n", pthread_self());
            pthread_mutex_unlock(&mutex);
            continue;
        }

        pthread_cond_wait(&elves_has_been_served, &mutex);
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t elf_thread_ids[NUM_OF_ELVES];
    for (int i=0;i<NUM_OF_ELVES;i++) {
        pthread_create(&elf_thread_ids[i], NULL, elf_routine, NULL);
    }

    while(1) {
        pthread_mutex_lock(&mutex);
        while(waiting_elves < NUM_OF_REQUIRED_ELVES) {
            pthread_cond_wait(&required_num_of_elves_waiting, &mutex);
        }
        if (waiting_elves == NUM_OF_REQUIRED_ELVES) {
            printf("Santa has been awoken! Helping elves.\n");
            sleep(1 + rand() % 2);
            waiting_elves = 0;
            pthread_cond_broadcast(&elves_has_been_served);
        }
        else {
            fprintf(stderr, "Something went wrong, incorrect program state\n");
            exit(1);
        }
        printf("Santa finished helping elves\n");
        pthread_mutex_unlock(&mutex);
    }
    return EXIT_SUCCESS;
}