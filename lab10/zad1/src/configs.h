#ifndef CONFIGS_H
#define CONFIGS_H
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include "printutils.h"

#define UNIX_PATH_MAX 108
#define MAX_CLIENTS_REGISTERED 4
#define CLIENT_NICK_LENGTH 16
#define RESPONSE_SIZE 128
#define REQUEST_SIZE 128
#define CHECK_ATTEMPTS 35
#define CHECK_RESPONSE_INTERVAL 100

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENTS -1

int getTime(){
    struct timespec spec;
    clock_gettime(1, &spec);
    return spec.tv_sec * 1000 + spec.tv_nsec / 1e6;
}

typedef enum{
    ONLINE,
    LOCAL,
    NONE
} ConnectionType;

typedef enum {
    X_WIN,
    O_WIN,
    DRAW,
    NOT_SETTLED
} GameResult;

typedef struct{
    char *nickname;
    char *enemy;
    ConnectionType connection_type;
    int descriptor;
    int check_attempts;
    struct sockaddr address;
    struct Client *next;
} Client;

void addClient(Client **client_list, int descriptor, ConnectionType connection_type, char *nickname, struct sockaddr address ){
    Client *newClient = malloc(sizeof(Client));

    char *client_nickname = malloc(sizeof(char) * (strlen(nickname) + 1));
    strcpy(client_nickname, nickname);

    Client values = {
        .nickname = client_nickname,
        .enemy = NULL,
        .connection_type = connection_type,
        .descriptor = descriptor,
        .check_attempts = 0,
        .address = address,
        .next = *client_list};
    *newClient = values;

    *client_list = newClient;
}

Client *findClient(Client *client_list, char *nickname){
    if (client_list == NULL) return NULL;

    if (strcmp(client_list->nickname, nickname) == 0) return client_list;

    return findClient(client_list->next, nickname);
}

void removeClient(Client **client_list, char *nickname){
    Client *prev = NULL;
    Client *curr = *client_list;

    while (curr != NULL && strcmp(curr->nickname, nickname) != 0){
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return;

    if (prev != NULL) prev->next = curr->next;
    else *client_list = curr->next;
    
    free(curr->nickname);
    free(curr);
}

#endif