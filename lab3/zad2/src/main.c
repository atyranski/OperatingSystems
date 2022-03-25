#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2

// ---- Main program
int main(int argc, char **argv){

    // TIME FUNC
    struct timespec start_t, end_t;
    struct tms* start_tms = calloc(1, sizeof(struct tms));
    struct tms* end_tms = calloc(1, sizeof(struct tms));

    times(start_tms);
    if(clock_gettime(CLOCK_REALTIME, &start_t) == -1){
        printf("clock gettime");
        exit(EXIT_FAILURE);
    }
    // TIME FUNC



    if(argc != 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide one number, which is an amount of child processes you want to create");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    int childrenAmount = atoi(argv[1]);

    for(int i=0; i<childrenAmount; i++){
        pid_t pid = fork();

        if(pid == 0){
            printf("\n[Child] this is print from process: #%d", getpid());
            return 0;
        }
    }


    // TIME FUNC
    times(end_tms);

    double real_time_in_ns = (end_t.tv_sec - start_t.tv_sec) + (end_t.tv_nsec - start_t.tv_nsec);
    double time2 = (double) ((*end_tms).tms_cutime - (*start_tms).tms_cutime / sysconf(_SC_CLK_TCK));
    double time3 = (double) ((*end_tms).tms_cstime - (*start_tms).tms_cstime / sysconf(_SC_CLK_TCK));
    // TIME FUNC

    printf("Real time: %f ns\n", real_time_in_ns / 1000000000);
    printf("System time: %f ns\n", time2);
    printf("User time: %f ns\n", time3);

    return 0;
}