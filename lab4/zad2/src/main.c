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
#define RETURN_COUDNT_FORK_PROCESS -3

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
    sprintf(message, "after executing 'SA_RESETHAND_handler' function raising signal #%d will reset handler for this perticular signal.\nThe second exectution of raise(SIGUSR1) should terminate the execution of this program.", sig);

    printf("");

    printInfo("SA_RESETHAND", message);
}

void SA_RESTART_handler(int signal){
    printInfo("Restart", "going on");
}

void SA_RESTART_example(int time){
    printf("Function will end if not interupted in: %d sec\n", time);
    sleep(time);
}

// ---- Main program
int main(int argc, char **argv){

    // SA_SIGINFO (since Linux 2.2)
    //     The signal handler takes three arguments, not one.  In
    //     this case, sa_sigaction should be set instead of
    //     sa_handler.  This flag is meaningful only when
    //     establishing a signal handler.
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

    // SA_RESTART
    //     Provide behavior compatible with BSD signal semantics by
    //     making certain system calls restartable across signals.
    //     This flag is meaningful only when establishing a signal
    //     handler.  See signal(7) for a discussion of system call
    //     restarting.
    printCheck("SA_RESTART");
    struct sigaction act_restart;
    act_restart.sa_sigaction = SA_RESTART_handler;
    sigemptyset(&act_restart.sa_mask);
    act_restart.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &act_restart, NULL) == -1){
        error("COULDNT_HANDLE_SIGNAL", "program occured problem with handling 'SIGINT' signal with flag 'SA_RESTART'");
        return RETURN_COULDNT_HANDLE_SIGNAL;
    }

    SA_RESTART_example(5);

    // SA_RESETHAND
    //     Restore the signal action to the default upon entry to the
    //     signal handler.  This flag is meaningful only when
    //     establishing a signal handler.
    printCheck("SA_RESETHAND");
    struct sigaction act_resethand;
    act_resethand.sa_handler = SA_RESETHAND_handler;
    sigemptyset(&act_resethand.sa_mask);
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