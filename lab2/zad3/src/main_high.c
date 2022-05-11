#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_UNKNOWN_ERROR -1
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2
#define RETURN_COULDNT_OPEN_FILE -3

#define _BUFFER_SIZE 256
static char BUFFER[_BUFFER_SIZE];

char* RAPORT_PATH = "out/raport2_high.txt";

FILE* getFileFromPath(char* path, char* mode){
    
    // Variables
    FILE* file;
    char* feedback[1000];

    // Checking if file exists (only with "r+" mode)
    if(strcmp(mode, "r+") == 0){
        if(access(path, F_OK ) != 0) {
            error("COULDNT_OPEN_FILE", "file don't exist or don't have permission");
            return NULL;
        }
    }


    // Opening file
    file = fopen(path, mode);
    if(file == NULL){
        error("COULDNT_OPEN_FILE", "program cannot open file from provided path. Path is incorrect, file don't exist or don't have permission in order to read file.");
        return NULL;
    }

    // Printing information about opening file correctly
    sprintf(feedback, "file '%s' opened successfuly\n", path);
    printCheck(feedback);

    return file;
}

int appearancesInFile(char* character, FILE* file){
   
    char* buffer = BUFFER;
    int bytes_left = fread(buffer, sizeof(char), 1, file);
    int appearances = 0;

    while(bytes_left){
        if(strcmp(buffer, character) == 0) {
            appearances++;
            printf("\033[1;31m%s\033[0m", buffer);
            // fwrite (buffer, sizeof(char), 1, file);
        } else printf("%s", buffer);

        bytes_left = fread(buffer, sizeof(char), 1, file);
    }

    printf("\n");

    return appearances;
}

// ---- Main program
int main(int argc, char **argv){
    
    printf("[Main] Execute version: %s\n\n", "fread() and fwrite()");

    if(argc == 3){
        // Variables for files
        FILE* file;
        FILE* output;
        char* character = argv[1];
        int result;
        char* message[1000];

        // Open file
        file = getFileFromPath(argv[2], "r+");
        if(file == NULL) return RETURN_COULDNT_OPEN_FILE;

        // Open output file
        output = getFileFromPath(RAPORT_PATH, "a");
        if(output == NULL) return RETURN_COULDNT_OPEN_FILE;

        result = appearancesInFile(character, file);

        sprintf(message, "character '%s' found %d time(s) in file '%s'\n", character, result, argv[2]);
        printInfo("Result", message);

        fwrite(message, sizeof(char), strlen(message), output);

        // Closing and saving changes to particular files
        fclose(file);
        fclose(output);

        return RETURN_SUCCESS;
    }

    if(argc != 3){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide 2 arguments: [character] [path to file]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    error("RETURN_UNKNOWN_ERROR", "program occured unknown problem");
    return RETURN_UNKNOWN_ERROR;
}