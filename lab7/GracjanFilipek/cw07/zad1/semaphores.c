#include <stdlib.h>

#include "semaphores.h"

/*
 * We use only semaphore groups with one semaphore in them - for better readability and other reasons
 * */

semaphore_id create_semaphore(const char* filepath, int init_sem_value) {
    key_t key = ftok(getenv("HOME"), filepath[0]);
    if (key == -1) {
        perror("Failed to create semaphore on ftok");
        return -1;
    }
    semaphore_id sem_id = semget(key, 1, 0644 | IPC_CREAT);
    if (sem_id == -1) {
        perror("Failed to create semaphore on semget");
        return -1;
    }
    if (semctl(sem_id, 0, SETVAL, init_sem_value) == -1) {
        perror("Failed to create semaphore on semctl");
        exit(1);
    }
    return sem_id;
}


semaphore_id open_semaphore(const char* filepath) {
    key_t key = ftok(getenv("HOME"), filepath[0]);
    if (key == -1) {
        return -1;
    }
    int sem_id = semget(key, 1, 0);
    return sem_id;
}

void delete_semaphore(semaphore_id sem_id) {
    semctl(sem_id, 0, IPC_RMID);
}

void acquire(semaphore_id sem) {
    struct sembuf operation;
    operation.sem_num = 0;
    operation.sem_op = -1;
    operation.sem_flg = 0;

    if(semop(sem, &operation, 1) == -1) {
        perror("Failed o acquire place in semaphore");
        exit(1);
    }
}

void release(semaphore_id sem) {
    struct sembuf operation;
    operation.sem_num = 0;
    operation.sem_op = 1;
    operation.sem_flg = 0;

    if(semop(sem, &operation, 1) == -1) {
        perror("Failed to release place in semaphore");
        exit(1);
    }
}
