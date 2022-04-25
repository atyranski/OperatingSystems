#ifndef CONFIGS_H
#define CONFIGS_H

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_COULDNT_CREATE_KEY -1
#define RETURN_COULDNT_OPEN_QUEUE -2
#define RETURN_COULDNT_CREATE_CLIENT -3
#define RETURN_SERVER_IS_FULL -4
#define RETURN_COULDNT_RECEIVE_MESSAGE -5
#define RETURN_UNEXPECTED_VALUE -6

// ---- Server configs
#define MAX_CLIENTS 3
#define QUEUE_PATH "key_file"
#define PROJ_ID 'S'
#define MAX_REQUEST_SIZE 128
#define REAL_REQUEST_SIZE sizeof(Request) - sizeof(long)
#define EVERY_REQUEST_TYPE -7

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