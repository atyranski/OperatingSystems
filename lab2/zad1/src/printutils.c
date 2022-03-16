#include "printutils.h"
#include <stdio.h>

// ---- Console print funtions
void error(char* type, char* message){
    printf("\033[1;31m");
    printf("[%s]\n", type);
    printf("\033[0m");
    printf("%s\n\n", message);
}

void printInfo(char* type, char* message){
    printf("\033[0;32m");
    printf("[%s] ", type);
    printf("\033[0m");
    printf("%s\n", message);
}

void printCheck(char* message){
    printf("\033[0;33m");
    printf("[Check] ");
    printf("\033[0m");
    printf("%s\n", message);
}

void printPrompt(char* type, char* message){
    printf("\033[0;34m");
    printf("[%s] %s: ", type, message);
    printf("\033[0m");
}