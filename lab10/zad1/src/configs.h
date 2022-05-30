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
#include "printutils.h"

#define UNIX_PATH_MAX 108
#define MAX_CLIENTS_REGISTERED 4
#define CLIENT_NICK_LENGTH 16

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENTS -1

typedef enum RequestType{
    REQ_REGISTER,
    REQ_MOVE,
    REQ_QUIT,
    REQ_CHECK,
} RequestType;

typedef enum ConnectionType{
    CONN_LOCAL,
    CONN_ONLINE,
    CONN_NONE
} ConnectionType;

typedef struct{
    int id;
    int descriptor;
    char nick[CLIENT_NICK_LENGTH];
    char symbol;
    ConnectionType connection;
    struct sockaddr address;
} Client;

typedef struct{
    Client *first;
    Client *second;
} Game;

typedef struct{
    int online;
    int local;
    Client **clients;
    int clients_amount;
    Game **games;
    int games_amount;
} Server;

#endif