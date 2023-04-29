#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>

#include "utility.h"

int server_queue_id = -1;

int next_client_id = 1;
int client_id[MAX_CONNECTED_CLIENTS];
int client_queue_id[MAX_CONNECTED_CLIENTS];

Message message_buffer;

FILE* log_diary;

void (*client_handlers[MAX_CONNECTED_CLIENTS])();

char* str_buffer;

void write_to_log(int force_write) {
    if (message_buffer.msg_type == MSG_INIT && force_write == false) {
        // init handler invokes write_to_log itself
        return;
    }

    int read_chars = sprintf(str_buffer, "%s %s %d\n", message_buffer.time, MSG_TYPE_NAME[message_buffer.msg_type - 1], message_buffer.sender_id);
    fwrite(str_buffer, sizeof(char), read_chars, log_diary);
}

int find_client_index(int searched_client_id) {
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; ++i) {
        if (client_id[i] == searched_client_id) {
            return i;
        }
    }

    fprintf(stderr, "There's no client with following id: %d\n", searched_client_id);
    return -1;
}

int find_client_queue_id(int searched_client_id) {
    int client_ind = find_client_index(searched_client_id);

    if (client_ind == -1) {
        fprintf(stderr, "There's no client with following id: %d\n", searched_client_id);
        return -1;
    }

    return client_queue_id[client_ind];
}

/********************************************
FAMILY OF FUNCTIONS TO HANDLE CLIENT REQUESTS
********************************************/

void handle_client_init() {
    int available_i = -1;

    // find available place in user arrays
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; ++i) {
        if (client_id[i] == -1) {
            available_i = i;
            break;
        }
    }

    if (available_i == -1) {
        // no available space to place new client in
        return;
    }

    client_id[available_i] = next_client_id;
    next_client_id++;
    client_queue_id[available_i] = message_buffer.various;

    message_buffer.sender_id = client_id[available_i];
    write_to_log(true);

    msg_send(client_queue_id[available_i],
             message_buffer.msg_type,
             -1,
             client_id[available_i],
             client_id[available_i],
             "\0");

}

void handle_client_list() {
    // prepare string including active client ids
    char* buffer_copy = str_buffer;
    int written_chars;

    for (int i = 0; i < MAX_CONNECTED_CLIENTS; ++i) {
        if(client_id[i] != -1) {
            written_chars = sprintf(buffer_copy, "%d ", client_id[i]);
            buffer_copy += written_chars;
        }

        if (buffer_copy >= str_buffer+MAX_CONTENT_LENGTH) {
            fprintf(stderr, "Cannot fit all client ids in the buffer\n");
            return;
        }
    }

    // get client queue id
    int searched_client_queue_id = find_client_queue_id(message_buffer.sender_id);
    if (searched_client_queue_id == -1) {
        return;
    }
    msg_send(searched_client_queue_id,
             message_buffer.msg_type,
             -1,
             message_buffer.sender_id,
             -1,
             str_buffer);
}

void handle_client_send_to_one() {
    int searched_client_queue_id = find_client_queue_id(message_buffer.addressee_id);
    if (searched_client_queue_id == -1) {
        return;
    }

    msg_send(searched_client_queue_id,
             message_buffer.msg_type,
             message_buffer.sender_id,
             message_buffer.addressee_id,
             -1,
             message_buffer.content);
}

void handle_client_send_to_all() {
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; ++i) {
        if(client_id[i] != -1 && client_id[i] != message_buffer.sender_id) {
            msg_send(client_queue_id[i],
                     message_buffer.msg_type,
                     message_buffer.sender_id,
                     -1,
                     -1,
                     message_buffer.content);
        }
    }
}

void handle_client_stop() {
    printf("Removing client...\n");
    int client_ind = find_client_index(message_buffer.sender_id);
    client_id[client_ind] = -1;
    client_queue_id[client_ind] = -1;
}

void interrupt_handler(int sig_num) {
    exit(0);
}

void exit_handler() {
    printf("\n");

    // send STOP messages to every active client and wait for their responses
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; ++i) {
        if (client_id[i] != -1) {
            msg_send(client_queue_id[i], MSG_STOP, -1, client_id[i], -1, "\0");
            msgrcv(server_queue_id, &message_buffer, sizeof(Message), MSG_STOP, 0);
            print_message(&message_buffer);
            write_to_log(false);
        }
    }

    printf("\n");
    printf("Deleting server queue...\n");
    delete_queue(server_queue_id);
    free(str_buffer);
    fclose(log_diary);
}

void init() {
    signal(SIGINT, interrupt_handler);
    atexit(exit_handler);

    // set client ids and client queue ids to -1
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; ++i) {
        client_id[i] = -1;
        client_queue_id[i] = -1;
    }

    // set client handlers
    client_handlers[0] = handle_client_init;
    client_handlers[1] = handle_client_list;
    client_handlers[2] = handle_client_send_to_one;
    client_handlers[3] = handle_client_send_to_all;
    client_handlers[4] = handle_client_stop;

    // create server queue
    printf("Creating server queue...\n");
    server_queue_id = open_server_queue(true);
    printf("Server queue id: %d\n", server_queue_id);

    // create / open log file
    log_diary = fopen("./server_logs.txt", "a");

    // allocate buffer
    str_buffer = malloc(MAX_CONTENT_LENGTH);
}

void listen() {
    while(1) {
        printf("\nListening...\n");

        //receive STOP message
        if (msgrcv(server_queue_id, &message_buffer, sizeof(Message), MSG_STOP, IPC_NOWAIT) != -1) {}
        else if(msgrcv(server_queue_id, &message_buffer, sizeof(Message), MSG_LIST, IPC_NOWAIT) != -1) {}
        else {
            msgrcv(server_queue_id, &message_buffer, sizeof(Message), 0, 0);
        }

        print_message(&message_buffer);
        write_to_log(false);
        client_handlers[message_buffer.msg_type - 1]();
    }
}

int main() {
    init();

    listen();

    return 0;
}
