#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>

// ---- Console print funtions
void error(char* type, char* message);

void printInfo(char* type, char* message);

void printOper(char* type, char* message);

void printPrompt(char* type, char* message);

void printTime(int id, double *time);

#endif