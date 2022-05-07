#ifndef CONFIGS_H
#define CONFIGS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_COULDNT_CREATE_KEY -1
#define RETURN_COULDNT_CREATE_SEMAPHORE -2
#define RETURN_COULDNT_CREATE_SHARED_MEMORY -3
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -4
#define RETURN_INCORRECT_ARGUMENT -5
#define RETURN_COULDNT_OPEN_SEMAPHORE -6
#define RETURN_COULDNT_OPEN_SHARED_MEMORY -7

#define HOME_PATH getenv("HOME")
#define PROJ_ID 'S'
#define SEMAPHORE_AMOUNT 2

#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5

#define OVEN_ID 0
#define TABLE_ID 1

#define TIME_PREPARE 1000
#define TIME_BAKE 4000
#define TIME_DELIVERY 4000
#define TIME_RETURN 4000
#define TIME_REST 3000

#define TYPE_MIN 0
#define TYPE_MAX 9

typedef struct {
    int oven[5];
    int table[5];
    int oven_amount;
    int table_amount;
} PizzaHouse;

union semun {
    int val;                    /* Value for SETVAL */
    struct semid_ds *buf;       /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array;      /* Array for GETALL, SETALL */
    struct seminfo *__buf;      /* Buffer for IPC_INFO (Linux-specific) */
};


key_t key;
PizzaHouse *pizza_house;
bool running = true;

key_t createKey(){
        if((key = ftok(HOME_PATH, PROJ_ID)) == (key_t) -1){
        error("COULDNT_CREATE_KEY", "ftok() couldn't generate new key");
        return RETURN_COULDNT_CREATE_KEY;
    }
}

#endif