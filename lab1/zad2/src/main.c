#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "wcutils.h"

#define RETURN_CODE_SUCCESS 0;
#define RETURN_BAD_ARGUMENT 2;

char* tempPath = "src/temp.txt";

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

void printCheck(char* type, char* message){
    printf("\033[0;33m");
    printf("[%s] ", type);
    printf("\033[0m");
    printf("%s\n\n", message);
}

int isNumber(char* s){
    for(int i=0; i<strlen(s); i++){
        if(!isdigit(s[i])) return 1;
    }

    return 0;
}

double timeDifference(clock_t time1, clock_t time2){
    return ((double) (time2 - time1) / sysconf(_SC_CLK_TCK));
}

void getTimeResults(clock_t start, clock_t end, struct tms* time_start, struct tms* time_end, char* operationName){
    printf("--- Operation %s ---\n", operationName);
    printf("[Real time]: %f\n", timeDifference(start, end));
    printf("[User time]: %f\n", timeDifference(time_start->tms_utime, time_end->tms_utime));
    printf("[System time]: %f\n", timeDifference(time_start->tms_stime, time_end->tms_stime));
    // (...)
}

int main(int argc, char **argv){
    WC_Table* table;

    int now = 0;
    struct tms * tms[argc];
    clock_t time[argc];
    for(int i=0; i<argc; i++){
        tms[i] = calloc(1, sizeof(struct tms *));
        time[i] = 0;
    }

    for(int i=2; i<argc; i){
        time[now] = times(tms[now]);
        now += 1;

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
            printCheck("Check", message);

            time[now] = times(tms[now]);
            // getTimeResults(time[now-1], time[now], tms[now-1], tms[now], argv[i]);

            i += 2;

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

                sprintf(message, "index: %d | lines: %d | words: %d | chars: %d", placed, table->blocks[placed].lines, table->blocks[placed].words, table->blocks[placed].chars);
                printCheck("Check", message);
            }

            time[now] = times(tms[now]);
            // getTimeResults(time[now-1], time[now], tms[now-1], tms[now], argv[i]);

            i = i + 1 + amount;

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
            removedBlock->lines = table->blocks[index].lines;
            removedBlock->words = table->blocks[index].words;
            removedBlock->chars = table->blocks[index].chars;

            sprintf(message, "%d", index);
            printInfo("Operation", "removing block at index:", message);
            removeBlock(table, index);

            sprintf(message, "\nprev at index %d:\nlines: %d | words: %d | chars: %d\nnow at index %d:\nlines: %d | words: %d | chars:", 
                index, removedBlock->lines, removedBlock->words, removedBlock->chars, 
                index, table->blocks[index].lines, table->blocks[index].words, table->blocks[index].chars);
            printCheck("Check", message);

            time[now] = times(tms[now]);
            // getTimeResults(time[now-1], time[now], tms[now-1], tms[now], argv[i]);

            i += 2;

            continue;
        }

        if(strcmp(argv[i], "print_table") == 0){
            printInfo("Operation", "printing table", "");
            printf("Table   | amount: %d capacity: %d\n", table->amount, table->capacity);

            for(int b=0; b<table->amount; b++){
                printf("Block#%d | line: %d words: %d chars: %d\n", b, 
                    table->blocks[b].lines, table->blocks[b].words, table->blocks[b].chars);
            }
            printf("\n");

            i += 1;

            continue;
        }

        error("BAD_CODE_ARGUMENT", "name of command is invalid");
        return RETURN_BAD_ARGUMENT;   
    }

    return RETURN_CODE_SUCCESS;
}