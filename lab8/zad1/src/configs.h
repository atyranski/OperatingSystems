#ifndef CONFIGS_H
#define CONFIGS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENTS -1

// according to https://people.sc.fsu.edu/~jburkardt/data/pgma/pgma.html
// every line should be no longer than 70 chars, so if every color can be
// possibly 3chars long (100-255), so the safe amount of columns is 17
// 17 * 4 (3chars + whitespace) = 67
#define COLS_IN_FILE 17 

typedef struct{
    int width;
    int height;
    int amount;
    unsigned char maxColor;
    unsigned char *colors;
} Image;

typedef struct{
    int x_start;
    int x_end;
    int y_start;
    int y_end;
} Area;

typedef struct{
    int id;
    Area *area;
} Thread_blocks;

typedef struct{
    int id;
    int range_from;
    int range_to;
} Thread_numbers;

#endif