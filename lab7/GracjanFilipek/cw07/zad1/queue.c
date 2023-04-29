#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "common.h"

char queue_get(char *queue) {
    if(queue_empty(queue)) {
        fprintf(stderr, "Cannot get from empty queue\n");
        return '\0';
    }
    char character = queue[0];
    memcpy(queue, queue + 1, strlen(queue) + 1);
    return character;
}

void queue_put(char *queue, char character) {
    if(queue_full(queue)) {
        fprintf(stderr, "Cannot add to full queue\n");
        return;
    }
    int size = strlen(queue);
    queue[size] = character;
    queue[size + 1] = '\0';
}

bool queue_full(char *queue) {
    return (strlen(queue) + 1 == BUFFER_SIZE);
}

bool queue_empty(char *queue) {
    return (strlen(queue) == 0);
}