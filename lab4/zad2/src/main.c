// Przetestuj działanie trzech wybranych flag w funkcji sigation.
// Jedną z nich powinna być flaga SA_SIGINFO.
// Dla tej flagi zainstaluj procedurę obsługi sygnału (handler) dla odpowiednio dobranych sygnałów stosując składnie procedury handlera z trzema argumentami.
// Wypisz i skomentuj (przygotowując odpowiednie scenariusze) trzy różne informacje,
// a dodatkowo także numer sygnału oraz identyfikator PID procesu wysyłającego dostarczane w strukturze siginfo_t przekazywanej jako drugi argument funkcji handlera.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_COULDNT_HANDLE_SIGNAL -1
#define RETURN_COUDNT_HANDLE_SIGNAL -2

void SA_SIGINFO_handler(int sig, siginfo_t *info, void *ucontext){
    printf("---------------------------------------------\n");
    printInfo("SA_SIGINFO", "informations");
    printf("[1] signal number:\t%d\n", info->si_signo);
    printf("[2] signal code:\t%d\n", info->si_code);
    printf("[3] sending process PID:\t%d\n", info->si_pid);
    printf("[4] real user ID of sending process:\t%d\n", info->si_uid);
    printf("[5] exit value of signal:\t%d\n", info->si_status);
    printf("[6] user times consumed:\t%Lf\n", (long double) info->si_utime);
    printf("[6] system times consumed:\t%Lf\n", (long double) info->si_stime);
    printf("[7] errno value:\t%d\n", info->si_errno);
    printf("\n");
}

void SA_RESETHAND_handler(int sig){
    char message[1000];
    sprintf(message, "after executing 'SA_RESETHAND_handler' function raising signal #%d will reset handler for this perticular signal. The second exectution of raise(SIGUSR1) should terminate the execution of this program.", sig);

    printf("");

    printInfo("SA_RESETHAND", message);
}

// ---- Main program
int main(int argc, char **argv){

    // Testing flag 'SA_SIGINFO'
    printCheck("SA_SIGINFO");
    struct sigaction act_info;
    act_info.sa_sigaction = SA_SIGINFO_handler;
    sigemptyset(&act_info.sa_mask);
    act_info.sa_flags = SA_SIGINFO;
    
    if (sigaction(SIGRTMIN, &act_info, NULL) == -1){
        error("COULDNT_HANDLE_SIGNAL", "program occured problem with handling 'SIGRTMIN' signal with flag 'SA_SIGINFO'");
        return RETURN_COULDNT_HANDLE_SIGNAL;
    }
    if (sigaction(SIGRTMIN + 10, &act_info, NULL) == -1){
        error("COULDNT_HANDLE_SIGNAL", "program occured problem with handling 'SIGRTMIN + 10' signal with flag 'SA_SIGINFO'");
        return RETURN_COULDNT_HANDLE_SIGNAL;
    }
    if (sigaction(SIGRTMAX, &act_info, NULL) == -1){
        error("COULDNT_HANDLE_SIGNAL", "program occured problem with handling 'SIGRTMAX' signal with flag 'SA_SIGINFO'");
        return RETURN_COULDNT_HANDLE_SIGNAL;
    }

    raise(SIGRTMIN);
    raise(SIGRTMIN + 10);
    raise(SIGRTMAX);

    // Testing flag 'SA_RESETHAND'

    // Testing flag 'SA_RESETHAND'
    printCheck("SA_RESETHAND");
    struct sigaction act_resethand;
    sigemptyset(&act_resethand.sa_mask);
    act_resethand.sa_handler = SA_RESETHAND_handler;
    act_resethand.sa_flags = SA_RESETHAND ;

    if (sigaction(SIGUSR1, &act_resethand, NULL) == -1){
        error("COULDNT_HANDLE_SIGNAL", "program occured problem with handling 'SIGUSR1' signal with flag 'SA_RESETHAND'");
        return RETURN_COULDNT_HANDLE_SIGNAL;
    }

    raise(SIGUSR1);
    raise(SIGUSR1);

    printf("Yay it works");

    return RETURN_SUCCESS;
}