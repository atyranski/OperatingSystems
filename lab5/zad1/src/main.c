#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2
#define RETURN_COUDNT_IGNORE_SIGNAL -3
#define RETURN_COUDNT_HANDLE_SIGNAL -4
#define RETURN_COUDNT_MASK_SIGNAL -5
#define RETURN_COUDNT_FORK_PROCESS -6
#define RETURN_COUDNT_EXEC_FUNCTION -7

// ---- Main program
int main(int argc, char **argv){

    if(argc != 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide mode name: ignore | handler | mask | pending");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }


    return RETURN_SUCCESS;
}