#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
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

//     char* signalsAmount = argv[1];

    printf("Sender pid: %d\n", getpid());

    return RETURN_SUCCESS;
}