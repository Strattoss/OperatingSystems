#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#include "utility.h"


int client_queue_id = -1;
int server_queue_id = -1;

int client_id = -1;

Message message_buffer;

char* str_buffer;
int read_chars;

int child_pid;


void interrupt_handler(int sig_num) {
    kill(child_pid, SIGINT);
    msg_send(server_queue_id, MSG_STOP, client_id, -1, -1, "\0");
    exit(0);
}

void usr1_handler(int sig_num) {
    msg_send(server_queue_id, MSG_STOP, client_id, -1, -1, "\0");
    exit(0);
}

void exit_handler() {
    printf("\nDeleting client queue...\n");
    delete_queue(client_queue_id);
    free(str_buffer);
}

int get_server_queue_id() {
    printf("Obtaining server queue id...\n");
    int got_server_queue_id = open_server_queue(false);
    if (got_server_queue_id == -1) {
        fprintf(stderr, "Failed to obtain server queue id\n");
        exit(1);
    }
    printf("Server queue id: %d\n", got_server_queue_id);
    return got_server_queue_id;
}

int get_client_id(int my_server_queue_id, int my_client_queue_id) {
    // send INIT message to server
    msg_send(my_server_queue_id, MSG_INIT, -1, -1, my_client_queue_id, "\0");

    msgrcv(my_client_queue_id, &message_buffer, sizeof(Message), MSG_INIT, 0);
    print_message(&message_buffer);

    printf("Set my client id to: %d\n", message_buffer.various);
    return message_buffer.various;
}

int get_client_queue_id() {
    printf("Creating client's queue...\n");
    int _client_queue_id = create_queue();
    if (_client_queue_id == -1) {
        fprintf(stderr, "Failed to create client's queue\n"
                        "Exiting...\n");
    }
    printf("Client queue id: %d\n", _client_queue_id);
    return _client_queue_id;
}

void init() {
    signal(SIGINT, interrupt_handler);
    signal(SIGUSR1, usr1_handler);

    atexit(exit_handler);

    str_buffer = malloc(MAX_CONTENT_LENGTH + MAX_COMMAND_LENGTH);

    client_queue_id = get_client_queue_id();
    server_queue_id = get_server_queue_id();
    client_id = get_client_id(server_queue_id, client_queue_id);
}

void read_pending_messages() {
    while (msgrcv(client_queue_id, &message_buffer, sizeof(Message), MSG_TO_ONE, IPC_NOWAIT) != -1) {
        print_message(&message_buffer);
    }
    while (msgrcv(client_queue_id, &message_buffer, sizeof(Message), MSG_TO_ALL, IPC_NOWAIT) != -1) {
        print_message(&message_buffer);
    }
    return;
}

void stop_listener_exit_handler() {
    return;
}

void wait_for_stop_message(int parent_pid) {
    signal(SIGINT, SIG_DFL);
    signal(SIGUSR1, SIG_DFL);
    atexit(stop_listener_exit_handler);

    msgrcv(client_queue_id, &message_buffer, sizeof(Message), MSG_STOP, 0);
    kill(parent_pid, SIGUSR1);
    exit(0);
}


void handle_command(Msg_type msg_type) {
    char *first_arg, *second_arg;

    switch (msg_type) {
        case MSG_LIST:
            msg_send(server_queue_id, MSG_LIST, client_id, -1, -1, "\0");
            msgrcv(client_queue_id, &message_buffer, sizeof(Message), MSG_LIST, 0);
            print_message(&message_buffer);
            break;
        case MSG_TO_ONE:
            first_arg = strtok(NULL, " ");
            second_arg = strtok(NULL, "\n");

            if(second_arg == NULL) {
                fprintf(stderr, "Incorrect arguments\n");
                break;
            }
            msg_send(server_queue_id, MSG_TO_ONE, client_id, atoi(first_arg), -1, second_arg);
            break;
        case MSG_TO_ALL:
            first_arg = strtok(NULL, "\n");

            if(first_arg == NULL) {
                fprintf(stderr, "Incorrect arguments\n");
                break;
            }
            msg_send(server_queue_id, MSG_TO_ALL, client_id, -1, -1, first_arg);
            break;
        case MSG_STOP:
            interrupt_handler(SIGINT);
        default:
            fprintf(stderr, "Incorrect command\n");
            break;
    }
}

void read_send_listen_loop() {
    size_t max_size = MAX_CONTENT_LENGTH + MAX_COMMAND_LENGTH;
    while(1) {
        read_pending_messages();

        printf("\n>>");
        read_chars = getline(&str_buffer, &max_size, stdin);
        if (read_chars >= max_size - 1) {
            fprintf(stderr, "Message exceeded maximal allowed length. It has %d characters, but it can have only %zu characters\n"
                            "Try again\n", read_chars, max_size);
            continue;
        }

        char* cmd_type_ptr = strtok(str_buffer, " \n");

        if (cmd_type_ptr == NULL) {
            fprintf(stderr, "Incorrect command\n");
            continue;
        }

        for (char* ptr = cmd_type_ptr; *ptr != '\0' ; ptr++) {
            *ptr = toupper(*ptr);
        }

        Msg_type msg_type = str_to_msg_type(cmd_type_ptr);

        handle_command(msg_type);
    }
}

int main() {
    init();
    int parent_pid = getpid();

    child_pid = fork();
    if (child_pid == -1) {
        perror("Couldn't fork the process");
        exit(1);
    }
    if (child_pid == 0) {
        // child (stop_listener)
        // this process listens to STOP messages from server
        // and informs parent (client) about it if receives one
        wait_for_stop_message(parent_pid);
    }
    else {
        // parent (client)
        read_send_listen_loop();
    }


    return 0;
}
