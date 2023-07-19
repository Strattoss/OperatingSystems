#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "message.h"
#include "common.h"

#define PING_INTERVAL 30    // in seconds
#define MAX_CONNECTIONS 16
#define MAX_EPOLL_EVENTS 10

typedef enum {
    CLIENT_EMPTY = 0,
    CLIENT_INITIALIZING,
    CLIENT_READY
} client_state;

typedef struct client {
    int fd;
    client_state state;
    char nickname[MAX_NICKNAME_LEN];
    bool responding;
} client;

typedef struct event_data {
    enum event_type {
        SOCKET_EVENT,
        CLIENT_EVENT
    } type;
    union data {
        client *client;
        int socket;
    } data;
} event_data;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epoll_fd;
client clients[MAX_CONNECTIONS];

void send_message(client *addressee_client, char *sender, MESSAGE_TYPE type, char *text) {
    message msg;
    msg.type = type;
    strncpy(msg.sender, sender, MAX_NICKNAME_LEN);
    strncpy(msg.addressee, addressee_client->nickname, MAX_NICKNAME_LEN);
    strncpy(msg.text, text, MAX_MESSAGE_TEXT_LEN);

    if (write(addressee_client->fd, &msg, sizeof(msg)) == -1) {
        perror(NULL);
    }
}

void delete_client(client *client) {
    printf("Deleting client '%s'\n", client->nickname);
    client->state = CLIENT_EMPTY;
    client->nickname[0] = '\0';
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
}

void handle_client_message(client *client) {
    if (client->state == CLIENT_INITIALIZING) {
        int nick_size = read(client->fd, client->nickname, sizeof(client->nickname) - 1);
        int client_position = client - clients;
        pthread_mutex_lock(&mutex);

        int searched_nickname = -1;
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (i != client_position &&
                (strncmp(client->nickname, clients[i].nickname, sizeof(clients[i].nickname)) == 0)) {
                searched_nickname = i;
                break;
            }
        }

        if (searched_nickname == -1 &&
            strncmp(client->nickname, SERVER_NICKNAME, sizeof(client->nickname)) != 0) {  // nickname not taken
            client->nickname[nick_size] = '\0';
            client->state = CLIENT_READY;
            printf("New client: %s\n", client->nickname);
        } else {    // nickname has been taken
            message msg = {.type = MSG_USERNAME_TAKEN};
            strncpy(msg.sender, SERVER_NICKNAME, sizeof(msg.sender));
            strncpy(msg.addressee, SERVER_NICKNAME, sizeof(msg.addressee));
            printf("Nickname '%s' is already taken\n", client->nickname);

            if (write(client->fd, &msg, sizeof(msg)) == -1) {
                perror(NULL);
            }

            strncpy(client->nickname, "new client", sizeof(client->nickname));
            delete_client(client); // username taken
        }
        pthread_mutex_unlock(&mutex);
    } else {
        message msg;
        if (read(client->fd, &msg, sizeof(msg)) == -1) {
            perror(NULL);
        }

        printf("Received message %s from %s\n", MESSAGE_TYPE_NAME[msg.type], client->nickname);

        if (msg.type == MSG_PING) {
            pthread_mutex_lock(&mutex);
            client->responding = true;
            pthread_mutex_unlock(&mutex);
        } else if (msg.type == MSG_DISCONNECT || msg.type == MSG_STOP) {
            pthread_mutex_lock(&mutex);
            delete_client(client);
            pthread_mutex_unlock(&mutex);
        } else if (msg.type == MSG_TO_ALL) {
            for (int i = 0; i < MAX_CONNECTIONS; i++) {
                if (clients[i].state != CLIENT_EMPTY)
                    send_message(&clients[i], client->nickname, MSG_GET, msg.text);
            }
        } else if (msg.type == MSG_LIST) {
            for (int i = 0; i < MAX_CONNECTIONS; i++) {
                if (clients[i].state != CLIENT_EMPTY)
                    send_message(client, SERVER_NICKNAME, MSG_GET, clients[i].nickname);
            }
        } else if (msg.type == MSG_TO_ONE) {
            for (int i = 0; i < MAX_CONNECTIONS; i++) {
                if (clients[i].state != CLIENT_EMPTY) {
                    if (strcmp(clients[i].nickname, msg.addressee) == 0) {
                        send_message(&clients[i], msg.sender, MSG_GET, msg.text);
                    }
                }
            }
        }
    }
}

void init_socket(int socket, void *addr, int addr_size) {
    if (bind(socket, (struct sockaddr *) addr, addr_size) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    if (listen(socket, MAX_CONNECTIONS) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event = {.events = EPOLLIN | EPOLLPRI};
    event_data *event_data = malloc(sizeof(*event_data));
    event_data->type = SOCKET_EVENT;
    event_data->data.socket = socket;
    event.data.ptr = event_data;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

client *new_client(int client_fd) {
    pthread_mutex_lock(&mutex);

    int free_index = -1;
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clients[i].state == CLIENT_EMPTY) {
            free_index = i;
            break;
        }
    }

    if (free_index == -1) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    client *client = &clients[free_index];

    client->fd = client_fd;
    client->state = CLIENT_INITIALIZING;
    client->responding = true;
    pthread_mutex_unlock(&mutex);

    return client;
}

void *ping_routine(void *arg_ptr) {
    message msg = {.type = MSG_PING};
    while (1) {
        sleep(PING_INTERVAL);

        pthread_mutex_lock(&mutex);
        printf("Pinging clients\n");
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (clients[i].state != CLIENT_EMPTY) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    if (write(clients[i].fd, &msg, sizeof(msg)) == -1) {
                        perror(NULL);
                    }
                } else {  // hasn't answered since last ping
                    delete_client(&clients[i]);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

void init_local_socket(char *socket_path) {
    struct sockaddr_un local_addr = {.sun_family = AF_UNIX};
    strncpy(local_addr.sun_path, socket_path, sizeof(local_addr.sun_path));

    unlink(socket_path);
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (local_socket == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    init_socket(local_socket, &local_addr, sizeof(local_addr));
}

void init_web_socket(int port) {
    struct sockaddr_in web_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };

    int web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    init_socket(web_socket, &web_addr, sizeof(web_addr));

}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage:[port] [socket_path]\n");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    char *socket_path = argv[2];

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    init_local_socket(socket_path);
    init_web_socket(port);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping_routine, NULL);

    printf("Server listening on:\n"
           "\tport - %d\n"
           "\tlocal unix socket path - '%s'\n", port, socket_path);

    struct epoll_event events[MAX_EPOLL_EVENTS];
    while (1) {
        int num_of_triggered_events = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, -1);
        if (num_of_triggered_events == -1) {
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_of_triggered_events; i++) {
            event_data *data = events[i].data.ptr;
            if (data->type == SOCKET_EVENT) { // new connection
                int client_fd = accept(data->data.socket, NULL, NULL);
                client *client = new_client(client_fd);
                if (client == NULL) {
                    printf("Server is full\n");
                    message msg = {.type = MSG_SERVER_FULL};
                    write(client_fd, &msg, sizeof(msg));
                    close(client_fd);
                    continue;
                }

                event_data *event_data = malloc(sizeof(*event_data));
                event_data->type = CLIENT_EVENT;
                event_data->data.client = client;
                struct epoll_event event = {.events = EPOLLIN | EPOLLET | EPOLLHUP, .data.ptr = event_data};

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror(NULL);
                    exit(EXIT_FAILURE);
                }
            } else if (data->type == CLIENT_EVENT) {
                if (events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    delete_client(data->data.client);
                    pthread_mutex_unlock(&mutex);
                } else {
                    handle_client_message(data->data.client);
                }
            }
        }
    }
}
