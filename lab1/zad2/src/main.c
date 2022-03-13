#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "wcutils.h"

#define RETURN_CODE_SUCCESS 0;
#define RETURN_BAD_ARGUMENT 2;

char* tempPath = "src/temp.txt";
char* raportPath = "out/raport2.txt";

typedef struct Time_Summary{
    clock_t r_start;
    clock_t r_end;
    clock_t u_start;
    clock_t u_end;
    clock_t s_start;
    clock_t s_end;
} Time_Summary;

void error(char* type, char* message){
    printf("\033[1;31m");
    printf("[%s] ", type);
    printf("\033[0m");
    printf("%s\n", message);
}

void printInfo(char* type, char* message, char* parameter){
    printf("\033[0;32m");
    printf("[%s] ", type);
    printf("\033[0m");
    printf("%s %s\n", message, parameter);
}

void printCheck(char* message){
    printf("\033[0;33m");
    printf("[Check] ");
    printf("\033[0m");
    printf("%s\n", message);
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

int isNumber(char* s){
    for(int i=0; i<strlen(s); i++){
        if(!isdigit(s[i])) return 1;
    }

    return 0;
}

void printTimeResults(char* title, struct tms* tms_start, struct tms* tms_end){
    double realTime = (double) (tms_end - tms_start) / sysconf(_SC_CLK_TCK);
    double userTime = (double) (tms_end->tms_cutime - tms_start->tms_cutime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double) (tms_end->tms_cstime - tms_start->tms_cstime) / sysconf(_SC_CLK_TCK);

    // printf("r: %f\nu: %f\ns: %f\n", realTime, userTime, systemTime);

    printf("----------- %s execution time --------\n", title);
    printf("[Real time]:     %f\n", realTime);
    printf("[User time]:     %f\n", userTime);
    printf("[System time]:     %f\n", systemTime);
    printf("=========================================\n");
}

int saveTestHeader(){
    FILE* raport = fopen(raportPath, "a");

    fprintf(raport, "=================================================\n");
    fprintf(raport, "--------------------- TEST ----------------------\n");
    fprintf(raport, "=================================================\n");
    fclose(raport);

    return RETURN_CODE_SUCCESS;
}

int saveTimeResults(char* title, struct tms* tms_start, struct tms* tms_end){
    FILE* raport = fopen(raportPath, "a");

    double realTime = (double) (tms_end - tms_start) / sysconf(_SC_CLK_TCK);
    double userTime = (double) (tms_end->tms_cutime - tms_start->tms_cutime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double) (tms_end->tms_cstime - tms_start->tms_cstime) / sysconf(_SC_CLK_TCK);

    fprintf(raport, "# %s execution time\n", title);
    fprintf(raport, "[Real time]: %f\n", realTime);
    fprintf(raport, "[User time]: %f\n", userTime);
    fprintf(raport, "[System time]: %f\n", systemTime);
    fprintf(raport, "-------------------------------------\n");
    fclose(raport);

    return RETURN_CODE_SUCCESS;
}

int saveOperationsResults(int filesCounted, int blocksRemoved){
    FILE* raport = fopen(raportPath, "a");

    fprintf(raport, "[Files counted]: %d\n", filesCounted);
    fprintf(raport, "[Blocks removed]: %d\n", blocksRemoved);
    fprintf(raport, "=====================================\n\n");
    fclose(raport);

    return RETURN_CODE_SUCCESS;
}

int main(int argc, char **argv){
    struct tms t;
    WC_Table* table;
    int filesCounted = 0;
    int blocksRemoved = 0;

    struct tms *tms_start_all= malloc(sizeof(struct tms));
    struct tms *tms_end_all = malloc(sizeof(struct tms));

    struct tms *tms_start_oper;
    struct tms *tms_end_oper;

    times(tms_start_all);

    saveTestHeader();

    for(int i=2; i<argc; i){
        tms_start_oper = malloc(sizeof(struct tms));
        tms_end_oper = malloc(sizeof(struct tms));

        times(tms_start_oper);

        // Handle "create_table x" command
        if(strcmp(argv[i], "create_table") == 0){
            if(isNumber(argv[i+1])){
                error("BAD_CODE_ARGUMENT", "incorrect or not provided argument for command 'create_table'");
                return RETURN_BAD_ARGUMENT;
            } 
            
            int size = atoi(argv[i+1]);
            char* message[1000];
            sprintf(message, "%d", size);

            printInfo("Operation", "creating table of size: ", message);
            table = createTable(size);

            sprintf(message, "blocks: %s | amount: %d | capacity: %d", table->blocks, table->amount, table->capacity);
            printCheck(message);

            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'create_table'", tms_start_oper, tms_end_oper);

            i += 2;

            printf("\n");
            continue;
        }

        // Handle "wc_files a.txt b.txt ..." command
        if(strcmp(argv[i], "wc_files") == 0){
            if(strstr(argv[i+1], ".txt") == NULL){
                error("BAD_CODE_ARGUMENT", "incorrect or not provided .txt file as argument for command 'wc_files'");
                return RETURN_BAD_ARGUMENT;
            }

            int amount = 0;
            while(1){
                if(i+1+amount < argc) {
                    if(strstr(argv[i+1+amount],".txt") != NULL) amount++;
                    else break;
                } else break;
            }

            for(int n=0; n<amount; n++){
                char* filePath = argv[i+1+n];
                char* message[1000];
                printInfo("Operation", "counting for file:", filePath);

                int placed = countFile(table, filePath, tempPath);

                WC_Block* block = table->blocks[placed];
                // printf("%d\n", block->lines);

                sprintf(message, "index: %d | lines: %d | words: %d | chars: %d", placed, block->lines, block->words, block->chars);
                printCheck(message);

                filesCounted++;
            }

            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'wc_files'", tms_start_oper, tms_end_oper);

            i = i + 1 + amount;

            printf("\n");
            continue;
        }

        // Handle "remove_block x" command
        if(strcmp(argv[i], "remove_block") == 0){
            if(isNumber(argv[i+1])) {
                error("BAD_CODE_ARGUMENT", "incorrect or not provided argument for command 'remove_block'");
                return RETURN_BAD_ARGUMENT;
            }

            int index = atoi(argv[i+1]);
            char* message[1000];

            WC_Block* removedBlock = calloc(1, sizeof(WC_Block));
            removedBlock->lines = table->blocks[index]->lines;
            removedBlock->words = table->blocks[index]->words;
            removedBlock->chars = table->blocks[index]->chars;

            sprintf(message, "%d", index);
            printInfo("Operation", "removing block at index:", message);
            removeBlock(table, index);

            sprintf(message, "\nprev at index %d:\nlines: %d | words: %d | chars: %d\nnow at index %d:\nlines: %d | words: %d | chars: %d", 
                index, removedBlock->lines, removedBlock->words, removedBlock->chars, 
                index, table->blocks[index]->lines, table->blocks[index]->words, table->blocks[index]->chars);
            printCheck(message);


            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'remove_block'", tms_start_oper, tms_end_oper);

            i += 2;
            blocksRemoved++;

            printf("\n");
            continue;
        }

        if(strcmp(argv[i], "print_table") == 0){
            printInfo("Operation", "printing table", "");
            printf("Table   | amount: %d capacity: %d\n", table->amount, table->capacity);

            for(int b=0; b<table->amount; b++){
                printf("Block#%d | line: %d words: %d chars: %d\n", b, 
                    table->blocks[b]->lines, table->blocks[b]->words, table->blocks[b]->chars);
            }

            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'print_table'", tms_start_oper, tms_end_oper);

            i += 1;

            printf("\n");
            continue;
        }

        error("BAD_CODE_ARGUMENT", "name of command is invalid");
        return RETURN_BAD_ARGUMENT;   
    }


    times(tms_end_all);

    printTimeResults("Total", tms_start_all, tms_end_all);
    saveTimeResults("Total", tms_start_all, tms_end_all);
    saveOperationsResults(filesCounted, blocksRemoved);

    removeTable(table);
    free(tms_start_all);
    free(tms_end_all);
    free(tms_start_oper);
    free(tms_end_oper);

    return RETURN_CODE_SUCCESS;
}