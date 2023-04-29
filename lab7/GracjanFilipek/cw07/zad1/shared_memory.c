#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#include <sys/shm.h>

#include "shared_memory.h"

int get_shared_memory(const char *filename, size_t size) {
    key_t key = ftok(filename, 0);
    if (key == -1) {
        perror("Failed on ftok");
        return -1;
    }
    return shmget(key, size, 0644 | IPC_CREAT);
}

char* attach_shared_memory(const char* filename, int size) {
    int shared_memory_id = get_shared_memory(filename, size);
    char *shared_memory;
    if (shared_memory_id == -1) {
        fprintf(stderr, "Failed to get shared memory id for filename: %s\n", filename);
        return NULL;
    }
    shared_memory = shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (char*)(-1)) {
        fprintf(stderr, "Failed to attach block to shared memory with id %d\n", shared_memory_id);
        return NULL;
    }
    return shared_memory;
}

bool detach_shared_memory(char *shared_memory) {
    return (shmdt(shared_memory) != -1);
}

bool destroy_shared_memory(const char *filename) {
    int shared_memory_id = get_shared_memory(filename, 0);
    if(shared_memory_id == -1) {
        return false;
    }
    return (shmctl(shared_memory_id, IPC_RMID, NULL) != -1);
}
