#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "wcutils.h"

// ---- Return codes
#define RETURN_CODE_SUCCESS 0;
#define RETURN_BAD_ARGUMENT 2;
#define RETURN_CODE_FILE_DONT_EXIST -1;

// ---- External files paths
char* tempPath = "src/temp.txt";
char* raportPath = "out/raport2.txt";

// ---- Console print funtions
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

void printTimeResults(char* title, struct tms* tms_start, struct tms* tms_end){
    double realTime = (double) (tms_end - tms_start) / sysconf(_SC_CLK_TCK);
    double userTime = (double) (tms_end->tms_cutime - tms_start->tms_cutime) / sysconf(_SC_CLK_TCK);
    double systemTime = (double) (tms_end->tms_cstime - tms_start->tms_cstime) / sysconf(_SC_CLK_TCK);

    // printf("r: %f\nu: %f\ns: %f\n", realTime, userTime, systemTime);

    printf("----------- %s execution time --------\n", title);
    printf("[Real time]:     %f\n", realTime);
    printf("[User time]:     %f\n", userTime);
    printf("[System time]:     %f\n", systemTime);
    printf("=========================================\n\n\n\n");
}

// ---- Utilities
int isNumber(char* s){
    for(int i=0; i<strlen(s); i++){
        if(!isdigit(s[i])) return 0;
    }

    return 1;
}

// ---- Save-to-file functions
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
    fprintf(raport, "=====================================\n\n\n\n");
    fclose(raport);

    return RETURN_CODE_SUCCESS;
}


// ---- Main program
int main(int argc, char **argv){
    
    // Necessary variables
    WC_Table* table;
    int filesCounted = 0;
    int blocksRemoved = 0;

    // TIme structures
    struct tms *tms_start_all= malloc(sizeof(struct tms));
    struct tms *tms_end_all = malloc(sizeof(struct tms));

    struct tms *tms_start_oper;
    struct tms *tms_end_oper;

    times(tms_start_all);

    //Just to print header in the raport file
    saveTestHeader();


    // Proceeding all of operations stored in argv
    for(int i=2; i<argc; i){

        // Initializing time structure for current operation
        tms_start_oper = malloc(sizeof(struct tms));
        tms_end_oper = malloc(sizeof(struct tms));

        times(tms_start_oper);

        // Handle "create_table x" command
        if(strcmp(argv[i], "create_table") == 0){

            // Checking if next argument from command line is a parameter for 'create_table'
            if(!isNumber(argv[i+1])){
                error("BAD_CODE_ARGUMENT", "incorrect or not provided argument for command 'create_table'");
                return RETURN_BAD_ARGUMENT;
            } 
            
            int size = atoi(argv[i+1]);
            char* message[1000];

            // Printing [Operation] in console
            sprintf(message, "%d", size);
            printInfo("Operation", "creating table of size: ", message);

            // Executing library function to create a table
            table = createTable(size);

            // Printing [Check] in console to verify command execution
            sprintf(message, "blocks: %s | amount: %d | capacity: %d", table->blocks, table->amount, table->capacity);
            printCheck(message);

            // Summary execution time for this operation
            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'create_table'", tms_start_oper, tms_end_oper);

            // Setting i-index to get next operation from argv
            i += 2;

            printf("\n");
            continue;
        }

        // Handle "wc_files a.txt b.txt ..." command
        if(strcmp(argv[i], "wc_files") == 0){

            // Checking if next argument from command line is a parameter for 'wc_files'
            if( strcmp(argv[i+1], "create_table") == 0 || 
                strcmp(argv[i+1], "wc_files") == 0 ||
                strcmp(argv[i+1], "remove_block") == 0 ||
                strcmp(argv[i+1], "print_table") == 0 ||
                isNumber(argv[i+1]) ){
                error("BAD_CODE_ARGUMENT", "incorrect or not provided .txt file as argument for command 'wc_files'");
                return RETURN_BAD_ARGUMENT;
            }

            // Counting how many files we have to count
            int amount = 0;
            while(1){
                if(i+1+amount < argc) {
                    if(strstr(argv[i+1+amount],".txt") != NULL) amount++;
                    else break;
                } else break;
            }

            // Proceeding wc for every provided file
            for(int n=0; n<amount; n++){
                char* filePath = argv[i+1+n];

                // Printing [Operation] in console
                char* message[1000];
                printInfo("Operation", "counting for file:", filePath);

                // Executing library function to execute system 'wc', create block and insert to table
                int placed = countFile(table, filePath, tempPath);

                if(placed == -1){
                    error("FILE_DONT_EXIST", "provided file dont exist");
                    return RETURN_CODE_FILE_DONT_EXIST;
                }

                // Printing [Check] in console to verify command execution
                WC_Block* block = table->blocks[placed];
                sprintf(message, "index: %d | lines: %d | words: %d | chars: %d", placed, block->lines, block->words, block->chars);
                printCheck(message);

                // Incrementing number of processed files 
                filesCounted++;
            }

            // Summary execution time for this operation
            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'wc_files'", tms_start_oper, tms_end_oper);

            // Setting i-index to get next operation from argv
            i = i + 1 + amount;

            printf("\n");
            continue;
        }

        // Handle "remove_block x" command
        if(strcmp(argv[i], "remove_block") == 0){

            // Checking if next argument from command line is a parameter for 'wc_files'
            if(!isNumber(argv[i+1])) {
                error("BAD_CODE_ARGUMENT", "incorrect or not provided argument for command 'remove_block'");
                return RETURN_BAD_ARGUMENT;
            }

            int index = atoi(argv[i+1]);
            char* message[1000];

            // Creating copy of removed block just to check if operation is done correctly
            WC_Block* removedBlock = calloc(1, sizeof(WC_Block));
            removedBlock->lines = table->blocks[index]->lines;
            removedBlock->words = table->blocks[index]->words;
            removedBlock->chars = table->blocks[index]->chars;

            // Printing [Operation] in console
            sprintf(message, "%d", index);
            printInfo("Operation", "removing block at index:", message);

            // Executing library function to execute system 'wc', create block and insert to table
            removeBlock(table, index);

            // Printing [Check] in console to verify command execution
            sprintf(message, "\nprev at index %d:\nlines: %d | words: %d | chars: %d\nnow at index %d:\nlines: %d | words: %d | chars: %d", 
                index, removedBlock->lines, removedBlock->words, removedBlock->chars, 
                index, table->blocks[index]->lines, table->blocks[index]->words, table->blocks[index]->chars);
            printCheck(message);

            // Summary execution time for this operation
            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'remove_block'", tms_start_oper, tms_end_oper);

            // Setting i-index to get next operation from argv
            i += 2;

            // Incrementing number of processed files 
            blocksRemoved++;

            printf("\n");
            continue;
        }

        if(strcmp(argv[i], "print_table") == 0){
            
            // Printing [Operation] in console
            printInfo("Operation", "printing table", "");
            printf("Table   | amount: %d capacity: %d\n", table->amount, table->capacity);

            // Printing a line with properties of every block in table
            for(int b=0; b<table->amount; b++){
                printf("Block#%d | line: %d words: %d chars: %d\n", b, 
                    table->blocks[b]->lines, table->blocks[b]->words, table->blocks[b]->chars);
            }

            // Summary execution time for this operation
            times(tms_end_oper);
            printTime(tms_start_oper, tms_end_oper);
            saveTimeResults("'print_table'", tms_start_oper, tms_end_oper);

            // Setting i-index to get next operation from argv
            i += 1;

            printf("\n");
            continue;
        }

        // Returning error, when provided command in argv is unknown
        error("BAD_CODE_ARGUMENT", "name of command is invalid");
        return RETURN_BAD_ARGUMENT;   
    }


    // Summary execution time for whole program
    times(tms_end_all);

    printTimeResults("Total", tms_start_all, tms_end_all);
    saveTimeResults("Total", tms_start_all, tms_end_all);
    saveOperationsResults(filesCounted, blocksRemoved);

    // Free table and time structs
    removeTable(table);
    free(tms_start_all);
    free(tms_end_all);
    free(tms_start_oper);
    free(tms_end_oper);

    return RETURN_CODE_SUCCESS;
}