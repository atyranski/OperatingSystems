#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include "printutils.h"
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

// ---- Console print funtions
void error(char* type, char* message);

void printInfo(char* type, char* message);

void printOper(char* type, char* message);

void printPrompt(char* type, char* message);

#endif