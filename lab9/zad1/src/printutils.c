#include "printutils.h"

// ---- Return codes
#define RETURN_CODE_SUCCESS 0
#define RETURN_BAD_ARGUMENT 2
#define RETURN_CODE_FILE_DONT_EXIST -1
#define RETURN_COULDNT_OPEN_FILE -2

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

void printOper(char* type, char* message){
    printf("\033[0;33m");
    printf("[%s] ", type);
    printf("\033[0m");
    printf("%s\n", message);
}

void printPrompt(char* type, char* message){
    printf("\033[0;34m");
    printf("[%s] %s: ", type, message);
    printf("\033[0m");
}

void printTime(int id, double *time){
    printf("\tThread #%d: %lfms\n", id, time);
}