#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_UNKNOWN_ERROR -1
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2

// ---- Main program
int main(int argc, char **argv){
    
//    if(argc != 2){
//       error("INCORRECT_ARGUMENT_AMOUNT", "provide amount of signals for sender program");
//       return RETURN_INCORRECT_ARGUMENT_AMOUNT;
//    }

//    char* signalsAmount = argv[1];

    printf("Catcher PID: %d\n", getpid());

    raise(SIGSTOP);

    return RETURN_SUCCESS;
}