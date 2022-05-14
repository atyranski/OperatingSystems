#include "printutils.h"
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

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

void printPrepare(pid_t pid, int n){
    printf("\033[0;34m");
    printf("[cook #%d] ", pid);
    printf("\033[0m");
    printf("Przygotowuje pizze: %d\n", n);
}

void printBake(pid_t pid, int n, int m){
    printf("\033[0;34m");
    printf("[cook #%d] ", pid);
    printf("\033[0m");
    printf("Dodałem pizze: %d. Liczba pizz w piecu: %d\n", n, m);
}

void printPullOut(pid_t pid, int n, int m, int k){
    printf("\033[0;34m");
    printf("[cook #%d] ", pid);
    printf("\033[0m");
    printf("Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", n, m, k);
}

void printPickUp(pid_t pid, int n, int k){
    printf("\033[0;34m");
    printf("[deliverer #%d] ", pid);
    printf("\033[0m");
    printf("Pobieram pizze: %d. Liczba pizz na stole: %d\n", n, k);
}

void printDeliver(pid_t pid, int n){
    printf("\033[0;34m");
    printf("[deliverer #%d] ", pid);
    printf("\033[0m");
    printf("Dostarczam pizze: %d\n", n);
}