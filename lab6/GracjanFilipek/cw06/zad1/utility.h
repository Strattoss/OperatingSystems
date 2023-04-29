#ifndef SYSOPY_UTILITY_H
#define SYSOPY_UTILITY_H

#include <stdbool.h>

#define MAX_COMMAND_LENGTH 64
#define MAX_TIME_LENGTH 128
#define MAX_CONTENT_LENGTH 1024
#define PROJECT_ID 'm'
#define MAX_CONNECTED_CLIENTS 5


typedef enum Msg_type {
    MSG_INIT = 1,
    MSG_LIST,
    MSG_TO_ONE,
    MSG_TO_ALL,
    MSG_STOP,
    MSG_TYPES
} Msg_type;

typedef struct Message {
    long msg_type;
    int sender_id;
    int addressee_id;
    int various;
    char time[MAX_TIME_LENGTH];
    char content[MAX_CONTENT_LENGTH];
} Message;


extern const char* MSG_TYPE_NAME[MSG_TYPES];

int open_server_queue(bool create);

int create_queue();
void delete_queue(int queue_id);

void msg_send(int queue_id, Msg_type msg_type, int sender_id, int addressee_id, int various, char *msg_content);

void print_message(Message* message);
Msg_type str_to_msg_type(const char* string);

#endif //SYSOPY_UTILITY_H

