#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0

int SIGNAL = SIGUSR1;

void isSignalPending(){
    char* message[1000];
    sigset_t pendingSignals;
    sigpending(&pendingSignals);

    if(sigismember(&pendingSignals, SIGNAL)){
        sprintf(message, "signal #%d is currently pending\n", SIGNAL);
        printInfo("Pending", message);
        return true;
    } else {
        sprintf(message, "signal #%d is not currently pending\n", SIGNAL);
        printInfo("Pending", message);
        return false;
    }
}

// ---- Main program
int main(int argc, char **argv){
    printCheck("Exec function");
    isSignalPending();

    // raise(SIGNAL);

    return RETURN_SUCCESS;
}