#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "message.h"
#include "common.h"

#define PARSER_BUFFER_SIZE 512
#define MAX_TOKENS 3

char *nickname;
int socket_num;
int epoll_fd;
struct epoll_event events[2] = {};

void handle_sigint() {
    message msg = {.type = MSG_DISCONNECT};
    strncpy(msg.sender, nickname, MAX_MESSAGE_TEXT_LEN);
    strncpy(msg.addressee, SERVER_NICKNAME, MAX_MESSAGE_TEXT_LEN);
    if (write(socket_num, &msg, sizeof(msg)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void print_help() {
    printf("\nWarning! Parts of the following commands are NOT separated with spaces (like usually).\n"
           "Allowed delimiters: | \\t \\n\n"
           "Available commands:\n"
           "\tlist - lists all users connected to the server\n"
           "\tto_all|[text] - sends the text to all users\n"
           "\tto_one|[text]|[user] - sends text to the user\n"
           "\tstop - disconnect from the server\n"
           "\thelp - show this prompt\n");
}

int connect_unix(char *path) {
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    return sock;
}

int connect_web(char *ipv4, int port) {
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int pton_result = inet_pton(AF_INET, ipv4, &addr.sin_addr);
    switch (pton_result) {
        case -1:
            perror(NULL);
        case 0:
            fprintf(stderr, "Invalid address\n");
            exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    return sock;
}

void handle_user_input() {
    char buffer[PARSER_BUFFER_SIZE];
    char delimiters[] = "|\t\n";
    char *tokens[MAX_TOKENS];
    int num_tokens = 0;

    int x = read(STDIN_FILENO, &buffer, PARSER_BUFFER_SIZE);
    buffer[x] = 0;

    char *token = strtok(buffer, delimiters);
    while (token != NULL && num_tokens < MAX_TOKENS) {
        tokens[num_tokens] = token;
        token = strtok(NULL, delimiters);
        num_tokens++;
    }

    if (num_tokens == 0 || tokens[0][0] == '\n') {
        return;
    }

    MESSAGE_TYPE type;
    char addressee_nickname[MAX_NICKNAME_LEN] = SERVER_NICKNAME;
    char text[MAX_MESSAGE_TEXT_LEN] = {};

    if (strcmp(tokens[0], "list") == 0) {
        type = MSG_LIST;
    } else if (strcmp(tokens[0], "to_all") == 0) {
        type = MSG_TO_ALL;
        if (num_tokens > 1) {
            strncpy(text, tokens[1], MAX_MESSAGE_TEXT_LEN);
        }
    } else if (strcmp(tokens[0], "to_one") == 0) {
        type = MSG_TO_ONE;
        if (num_tokens > 2) {
            strncpy(text, tokens[1], MAX_MESSAGE_TEXT_LEN);
            strncpy(addressee_nickname, tokens[2], MAX_NICKNAME_LEN);
        }
    } else if (strcmp(tokens[0], "stop") == 0) {
        type = MSG_STOP;
    } else if (strcmp(tokens[0], "help") == 0) {
        print_help();
        return;
    } else {
        printf("Command or syntax incorrect. For help, write 'help'\n");
        return;
    }

    message msg;
    msg.type = type;
    strcpy(msg.addressee, addressee_nickname);
    strcpy(msg.sender, nickname);
    strncpy(msg.text, text, MAX_MESSAGE_TEXT_LEN - 1);

    if (write(socket_num, &msg, sizeof(msg)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

//    if (msg.type == MSG_STOP) {
//
//    }
}

void handle_server_message() {
    message msg;
    if (read(socket_num, &msg, sizeof(msg)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    if (msg.type == MSG_USERNAME_TAKEN) {
        printf("This username is already taken\n");
        close(socket_num);
        exit(EXIT_SUCCESS);
    } else if (msg.type == MSG_SERVER_FULL) {
        printf("Server has no free room for new users\n");
        close(socket_num);
        exit(EXIT_SUCCESS);
    } else if (msg.type == MSG_PING) {
        if (write(socket_num, &msg, sizeof(msg)) == -1) {
            perror(NULL);
            exit(EXIT_FAILURE);
        }
    } else if (msg.type == MSG_STOP) {
        printf("Ending client process...\n");
        close(socket_num);
        exit(EXIT_SUCCESS);
    } else if (msg.type == MSG_GET) {
        printf("%s: %s\n", msg.sender, msg.text);
    }
}

void setup_stdin_epoll_event() {
    struct epoll_event stdin_event = {
            .events = EPOLLIN | EPOLLPRI,
            .data = {.fd = STDIN_FILENO}
    };
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

void setup_socket_epoll_event() {
    struct epoll_event socket_event = {
            .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
            .data = {.fd = socket_num}
    };
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_num, &socket_event) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

void setup_epoll() {
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    setup_stdin_epoll_event();
    setup_socket_epoll_event();
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage:\n"
               "\t[username] web [ipv4] [port]\n"
               "\t[username] unix [path]\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[2], "web") == 0 && argc == 5) {
        socket_num = connect_web(argv[3], atoi(argv[4]));
    } else if (strcmp(argv[2], "unix") == 0 && argc == 4) {
        socket_num = connect_unix(argv[3]);
    } else {
        printf("Usage:\n"
               "\t[username] web [ipv4] [port]\n"
               "\t[username] unix [path]\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    nickname = argv[1];
    if (write(socket_num, nickname, strlen(nickname)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    setup_epoll();

    print_help();

    while (1) {
        int num_of_triggered_events = epoll_wait(epoll_fd, events, 2, 1);
        if (num_of_triggered_events == -1) {
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_of_triggered_events; i++) {
            if (events[i].events & EPOLLHUP) {
                printf("Disconnected\n");
                close(socket_num);
                exit(EXIT_SUCCESS);
            } else if (events[i].data.fd == STDIN_FILENO) {
                handle_user_input();
            } else {
                handle_server_message();
            }
        }
    }
}
