#ifndef CONFIGS_H
#define CONFIGS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define UNIX_PATH_MAX 108
#define MAX_CLIENTS_REGISTERED 4
#define CLIENT_NICK_LENGTH 16

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENTS -1

typedef struct{
    int id;
    char nick[CLIENT_NICK_LENGTH];
    char symbol;
} Client;

typedef struct{
    Client *first;
    Client *second;
} Game;

#endif