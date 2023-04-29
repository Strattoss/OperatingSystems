#ifndef __SEMAPHORES_H__
#define __SEMAPHORES_H__

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

typedef int semaphore_id;

semaphore_id create_semaphore(const char* filepath, int init_sem_value);
semaphore_id open_semaphore(const char* filepath);
void delete_semaphore(semaphore_id sem_id);
void acquire(semaphore_id sem);
void release(semaphore_id sem);

#endif