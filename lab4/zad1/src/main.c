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

int SIGNAL = SIGUSR1;

void handleSignal(int signal){
    char* message[1000];

    sprintf(message, "currently handled signal number: #%d\n", signal);
    printInfo("Handle", message);
}

int maskSignal(){
    sigset_t newMask; // signals to block
    sigset_t oldMask; // current signal mask
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGNAL);

    if(sigprocmask(SIG_BLOCK, &newMask, &oldMask) < 0){
        error("COUDNT_MASK_SIGNAL", "program occured problem with masking custom user signal SIGUSR1");
        return RETURN_COUDNT_MASK_SIGNAL;
    }

    return RETURN_SUCCESS;
}

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

    if(argc != 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide mode name: ignore | handler | mask | pending");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    char* command = argv[1];



    // Case for 'ignore' command
    if(strcmp(command, "ignore") == 0){
        if(signal(SIGNAL, SIG_IGN) != NULL) {  // SIG_IGN - ignoring incoming signal
            error("COUDNT_IGNORE_SIGNAL", "program occured problem with ignoring custom user signal SIGUSR1");
            return RETURN_COUDNT_IGNORE_SIGNAL;
        }
        char message[1000];
        printInfo("Operation", "ignoring");
    }

    // Case for 'handler' command
    else if(strcmp(command, "handler") == 0){
        if(signal(SIGNAL, handleSignal) != NULL) {
            error("COUDNT_HANDLE_SIGNAL", "program occured problem with handling custom user signal SIGUSR1 with custom handler function");
            return RETURN_COUDNT_HANDLE_SIGNAL;
        }
        char message[1000];
        printInfo("Operation", "handling");
    }

    // Case for 'mask' command
    else if(strcmp(command, "mask") == 0){
        int maskingResult = maskSignal();
        if(maskingResult != 0) return maskingResult;

        char message[1000];
        printInfo("Operation", "masking");
    }

    // Case for 'mask' command
    else if(strcmp(command, "pending") == 0){
        int maskingResult = maskSignal();
        if(maskingResult != 0) return maskingResult;

        char message[1000];
        printInfo("Operation", "pending");
    }

    // Incorrect program parameter
    else {
        error("INCORRECT_ARGUMENT", "provide mode name: ignore | handler | mask | pending");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    // Raising custom signal
    raise(SIGNAL);

    // Just to get where parent ends and where child starts
    printInfo("Process", "parent");
    isSignalPending();
    
    // Creating child proccess
    int pid=fork();

    if (pid == -1){
        error("COUDNT_FORK_PROCESS", "program occured problem with creating a new process");
        return RETURN_COUDNT_FORK_PROCESS;
    }
    
    // Child process operations
    if(pid == 0){
        printInfo("Process", "child");

        if(strcmp(command, "pending") == 0) {
            isSignalPending();

            // Ending child process if mode is 'pending'
            return RETURN_SUCCESS;
        }

        // Checking in child process if signal is pending
        raise(SIGNAL);
        isSignalPending();
        
        if(execl("./exec_function", "exec_function", (char*) NULL) == -1) {
            error("COUDNT_EXEC_FUNCTION", "program occured problem with executing function in execl() in child process");
            return RETURN_COUDNT_EXEC_FUNCTION;
        }

        // Ending child process
        return RETURN_SUCCESS;
    }


    // Parent process waiting to all child process end
    while(wait(NULL) > 0);

    return RETURN_SUCCESS;
}