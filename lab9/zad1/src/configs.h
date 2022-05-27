#ifndef CONFIGS_H
#define CONFIGS_H
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENTS -1
#define RETURN_COULDNT_JOIN_ELF -2
#define RETURN_COULDNT_CREATE_THREAD -3

// that will assure, that no more than 3 elves is waiting for santa
#define MAX_ELVES_WORKING 3 

#define TIME_SOLVING 1000
#define DIFFERENCE_SOLVING 1000
#define TIME_SLEEPING 1000
#define DIFFERENCE_SLEEPING 1000
#define TIME_WORKING 2000
#define DIFFERENCE_WORKING 3000

typedef struct{
    int id;
    int elves_amount;
    int *elves_in_workshop;
    int *elves_in_workshop_amount;

    pthread_cond_t *condition_shutdown;
    pthread_cond_t *condition_santa;
    pthread_cond_t *condition_elves;
    pthread_mutex_t *mutex_access;
} Arguments;

#endif