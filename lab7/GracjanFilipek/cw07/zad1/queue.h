#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

char queue_get(char *queue);
void queue_put(char *queue, char byte);
bool queue_full(char *queue);
bool queue_empty(char *queue);

#endif