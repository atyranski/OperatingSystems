#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2
#define RETURN_COULDNT_OPEN_FILE -3
#define PI 3.141592654

double radiantToDegree(double randian){
    return (randian * 180) / PI;
}

double getFunctionValue(double x){
    return 4 * atan(x);
}

double integralValue(double x1, double x2){
    return getFunctionValue(x2) - getFunctionValue(x1);
}

double sumPartials(int intervalAmount){
    double result = 0;

    for(int i=0; i<intervalAmount; i++){
        char fileName[30];
        char buffer[30];
        double value;
        sprintf(fileName, "out/w%d.txt", i + 1);

        FILE* file = fopen(fileName,"r");
        fgets(buffer, 30, file);
        fclose(file);

        sscanf(buffer, "%lf", &value);
        result += value;
    }

    return result;
};

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

    if(argc < 3 || argc > 3){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide: [interval] [number of intervals]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    int intervalAmount = atoi(argv[2]);
    double interval = 1.0/intervalAmount;

    for(int i=0; i<intervalAmount; i++){
        pid_t pid = fork();
        // int status;

        if(pid == 0){
            double x1 = interval * (double) i;
            double x2 = interval * (double) i + interval;
            double result = integralValue(x1, x2);

            char fileName[30];
            sprintf(fileName, "out/w%d.txt", i + 1);

            FILE* file = fopen(fileName,"wb");

            if(file == NULL){
                error("COULDNT_OPEN_FILE", "program cannot open file from provided path. Path is incorrect, file don't exist or don't have permission in order to read file.");
                return RETURN_COULDNT_OPEN_FILE;
            }

            fprintf(file, "%f\n", result);
            fclose(file);

            return RETURN_SUCCESS;
        }
    }

    while (wait(NULL) > 0);
    double summary = sumPartials(intervalAmount);
    
    char message[1000];
    sprintf(message, "integral on range [0,1] of 4/(x^2 + 1) equals: %f", summary);
    
    printInfo("Summary", message);

    // TIME FUNC
    times(end_tms);

    double real_time_in_ns = (end_t.tv_sec - start_t.tv_sec) + (end_t.tv_nsec - start_t.tv_nsec);
    double time2 = (double) ((*end_tms).tms_cutime - (*start_tms).tms_cutime / sysconf(_SC_CLK_TCK));
    double time3 = (double) ((*end_tms).tms_cstime - (*start_tms).tms_cstime / sysconf(_SC_CLK_TCK));
    // TIME FUNC

    printf("\nReal time: %f ns", real_time_in_ns / 1000000000);
    printf("\nSystem time: %f ns", time2);
    printf("\nUser time: %f ns\n", time3);

    return RETURN_SUCCESS;
}