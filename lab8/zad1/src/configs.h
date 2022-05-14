#ifndef CONFIGS_H
#define CONFIGS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENTS -1

typedef struct{
    int width;
    int height;
    unsigned char maxColor;
    unsigned char *colors;
} Image;

#endif