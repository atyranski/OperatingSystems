#ifndef CONFIGS_H
#define CONFIGS_H

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_COULDNT_CREATE_KEY -1
#define RETURN_COULDNT_OPEN_QUEUE -2
#define RETURN_COULDNT_CREATE_CLIENT -3

// ---- Server configs
#define MAX_CLIENTS 3
#define QUEUE_PATH "server"
#define PROJ_ID 'S'
#define MAX_REQUEST_SIZE 4096
#define REAL_REQUEST_SIZE sizeof(Request) - sizeof(long)

// ---- Client configs


typedef struct {
    long type;
    int sender_id;
    int recipent_id;
    char content[MAX_REQUEST_SIZE];
} Request;

typedef enum {
    LIST = 1,
    ALL = 2,
    ONE = 3,
    STOP = 4,
    CONNECT = 5,
} Command;

#endif