#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "utility.h"


const char* MSG_TYPE_NAME[MSG_TYPES] = {
        "INIT",
        "LIST",
        "TO_ONE",
        "TO_ALL",
        "STOP"
};

int open_server_queue(bool create)
{
    key_t key = ftok(getenv("HOME"), PROJECT_ID);

    // open/create server message queue (depending on "create" argument value)
    int server_queue_id = msgget(key, (create ? IPC_CREAT : 0) | S_IRWXU | S_IRWXG | S_IRWXO);

    if (server_queue_id == -1)
    {
        perror("Failed to open / create server queue");
        return -1;
    }

    return server_queue_id;
}


int create_queue() {
    int new_queue_id = msgget( IPC_PRIVATE, S_IRWXU | S_IRWXG | S_IRWXO);
    if (new_queue_id == -1) {
        perror(NULL);
        exit(1);
    }

    return new_queue_id;
}

void delete_queue(int queue_id) {
    if (queue_id != -1) {
        if (msgctl(queue_id, IPC_RMID, NULL) == -1) {
            perror(NULL);
        }
    }
}

void msg_send(int queue_id, Msg_type msg_type, int sender_id, int addressee_id, int various, char *msg_content)
{
    struct timespec curr_time;
    clock_gettime(CLOCK_REALTIME, &curr_time);

    Message message;
    message.msg_type = msg_type;
    message.sender_id = sender_id;
    message.addressee_id = addressee_id;
    message.various = various;
    sprintf(message.time, "%ld", curr_time.tv_sec);
    strcpy(message.content, msg_content);

    if (msgsnd(queue_id, &message, MAX_CONTENT_LENGTH, 0) == -1) {
        perror("Failed to send a message");
    }
}

void print_message(Message* message) {
    printf("Received message:\n"
           "\ttype: %s\n"
           "\tsender_id: %d\n"
           "\taddressee_id: %d\n"
           "\tvarious: %d\n"
           "\ttime: %s\n"
           "\tcontent: %s\n",
           MSG_TYPE_NAME[message->msg_type - 1],
           message->sender_id,
           message->addressee_id,
           message->various,
           message->time,
           message->content);
}

Msg_type str_to_msg_type(const char* string) {
    for (int i = 0; i < MSG_TYPES - 1; ++i) {
        if (strcmp(string, MSG_TYPE_NAME[i]) == 0) {
            return i+1;
        }
    }
    return -1;
}

