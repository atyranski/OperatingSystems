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

void printTime(struct tms* tms_start, struct tms* tms_end);

void printTimeResults(char* title, struct tms* tms_start, struct tms* tms_end);

// ---- Save-to-file functions
int saveTestHeader(char* raportPath, bool isLow);

int saveTimeResults(struct tms* tms_start, struct tms* tms_end, char* raportPath);

#endif