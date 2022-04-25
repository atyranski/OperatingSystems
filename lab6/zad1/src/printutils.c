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

void printTime(struct tms* tms_start, struct tms* tms_end){
    double realTime = (double) (tms_end - tms_start) / sysconf(_SC_CLK_TCK);
    double userTime = (double) (tms_end->tms_cutime - tms_start->tms_cutime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double) (tms_end->tms_cstime - tms_start->tms_cstime) / sysconf(_SC_CLK_TCK);   

    printf("\033[0;34m");
    printf("[Time] ");
    printf("\033[0m");
    printf("real: %f user: %f system: %f\n", realTime, userTime, systemTime);
}

void printTimeResults(char* title, struct tms* tms_start, struct tms* tms_end){
    double realTime = (double) (tms_end - tms_start) / sysconf(_SC_CLK_TCK);
    double userTime = (double) (tms_end->tms_cutime - tms_start->tms_cutime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double) (tms_end->tms_cstime - tms_start->tms_cstime) / sysconf(_SC_CLK_TCK);

    printf("----------- %s execution time --------\n", title);
    printf("[Real time]:     %f\n", realTime);
    printf("[User time]:     %f\n", userTime);
    printf("[System time]:     %f\n", systemTime);
    printf("=========================================\n\n\n\n");
}

// ---- Save-to-file functions
int saveTestHeader(char* raport_path, bool isLow){
    if(isLow){
        // Tego nie ma, bo wyrzuca blad kompilatora przez funkcje open - czemu? nie mam pojecia, bo w pliku main normalnie otwieral przez uzycie identycznej komendy
    //     int raport = open(raport_path, O_CREAT | O_WRONLY | O_APPEND);

    //     if(raport == NULL){
    //         error("COULDNT_OPEN_FILE", "program cannot open raport file.");
    //         return RETURN_COULDNT_OPEN_FILE;
    //     }

    //     write(raport, "=================================================\n", 50 * sizeof(char));
    //     write(raport, "--------------------- TEST ----------------------\n", 50 * sizeof(char));
    //     write(raport, "=================================================\n", 50 * sizeof(char));

    //     close(raport);

    } else {
        // FILE* raport = fopen(raport_path, "a");

        // if(raport == NULL){
        //     error("COULDNT_OPEN_FILE", "program cannot open file from provided path. Path is incorrect, file don't exist or don't have permission in order to read file.");
        //     return NULL;
        // }

        // fwrite ("=================================================\n", sizeof(char), 50, raport);
        // fwrite ("--------------------- TEST ----------------------\n", sizeof(char), 50, raport);
        // fwrite ("=================================================\n", sizeof(char), 50, raport);

        // fclose(raport);
    }

    FILE* raport = fopen(raport_path, "a");

    if(raport == NULL){
        error("COULDNT_OPEN_FILE", "program cannot open file from provided path. Path is incorrect, file don't exist or don't have permission in order to read file.");
        return NULL;
    }

    fwrite ("=================================================\n", sizeof(char), 50, raport);
    if(isLow) fwrite ("---------------------- LOW ----------------------\n", sizeof(char), 50, raport);
    else fwrite ("--------------------- HIGH ----------------------\n", sizeof(char), 50, raport);
    fwrite ("=================================================\n", sizeof(char), 50, raport);

    fclose(raport);

    return RETURN_CODE_SUCCESS;
}

int saveTimeResults(struct tms* tms_start, struct tms* tms_end, char* raportPath){
    FILE* raport = fopen(raportPath, "a");
    char real_time[1000];
    char user_time[1000];
    char system_time[1000];

    double realTime = (double) (tms_end - tms_start) / sysconf(_SC_CLK_TCK);
    double userTime = (double) (tms_end->tms_cutime - tms_start->tms_cutime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double) (tms_end->tms_cstime - tms_start->tms_cstime) / sysconf(_SC_CLK_TCK);

    fwrite("# Total execution time:\n", sizeof(char), 24, raport);

    sprintf(real_time, "[Real time]: %f\n", realTime);
    fwrite(real_time, sizeof(char), 22, raport);

    sprintf(user_time, "[User time]: %f\n", userTime);
    fwrite(user_time, sizeof(char), 22, raport);

    sprintf(system_time, "[System time]: %f\n", systemTime);
    fwrite(system_time, sizeof(char), 24, raport);

    fwrite ("=======================================\n", sizeof(char), 40, raport);
    fclose(raport);

    return RETURN_CODE_SUCCESS;
}