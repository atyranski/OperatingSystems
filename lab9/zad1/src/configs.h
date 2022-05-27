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
#define MAX_ELVES_COMPLAINING 3 

typedef struct{
    int id;
    int *elves_complaining;
    int *elves_complaining_amount;

    pthread_cond_t *condition_santa;
    pthread_cond_t *condition_elves;
    pthread_cond_t *condition_solved;

    pthread_mutex_t *mutex_access;
} Arguments;

#endif