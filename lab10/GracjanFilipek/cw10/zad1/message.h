#ifndef SYSOPY_MESSAGE_H
#define SYSOPY_MESSAGE_H

#define MAX_MESSAGE_TEXT_LEN 256
#define MAX_NICKNAME_LEN 16

typedef enum {
    MSG_PING = 0,
    MSG_USERNAME_TAKEN,
    MSG_SERVER_FULL,
    MSG_DISCONNECT,
    MSG_GET,
    MSG_INIT,
    MSG_LIST,
    MSG_TO_ONE,
    MSG_TO_ALL,
    MSG_STOP,   // from client to server
} MESSAGE_TYPE;

const char* MESSAGE_TYPE_NAME[] = {
        "MSG_PING",
        "MSG_USERNAME_TAKEN",
        "MSG_SERVER_FULL",
        "MSG_DISCONNECT",
        "MSG_GET",
        "MSG_INIT",
        "MSG_LIST",
        "MSG_TO_ONE",
        "MSG_TO_ALL",
        "MSG_STOP"
};

typedef struct {
    MESSAGE_TYPE type;
    char sender[MAX_NICKNAME_LEN];
    char addressee[MAX_NICKNAME_LEN];
    char text[MAX_MESSAGE_TEXT_LEN];
} message;

#endif //SYSOPY_MESSAGE_H
