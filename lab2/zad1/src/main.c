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

FILE* getFileFromPath(char* prompt_message, char* mode, char* path_provided){
    
    // Variables
    FILE* file;
    char* path[1000];
    char* feedback[1000];

    // Setting path (as a user input or function argument)
    if(path_provided == NULL){
        printPrompt("Input", prompt_message);
        scanf("%s", path);
    } else {
        strcpy(path, path_provided);
    }

    // Checking if file exists
    if(access(path, F_OK ) != 0) {
        error("COULDNT_OPEN_FILE", "file don't exist or don't have permission");
        return NULL;
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

int copyFromSourceToDestination(FILE* file_from, FILE* file_to){
   
    char* buffer = BUFFER;
    int bytes_left = fread(buffer, sizeof(char), 1, file_from);
    bool wasWhitespace = false;

    while(bytes_left){
        if( (wasWhitespace || strcmp(buffer, "\n")) &&
            (wasWhitespace || strcmp(buffer, " ")) &&
            (wasWhitespace || strcmp(buffer, "\t")) &&
            (wasWhitespace || strcmp(buffer, "\v")) &&
            (wasWhitespace || strcmp(buffer, "\f"))) {
            printf("%s",buffer);
            fwrite (buffer, sizeof(char), 1, file_to);
        }

        if( strcmp(buffer, "\n") &&
            strcmp(buffer, " ") &&
            strcmp(buffer, "\t") &&
            strcmp(buffer, "\v") &&
            strcmp(buffer, "\f")) wasWhitespace = true;
        else wasWhitespace = false;

        bytes_left = fread(buffer, sizeof(char), 1, file_from);
    }

    fwrite("-----------------------\n", sizeof(char), 24, file_to);

    return RETURN_SUCCESS;
}

// ---- Main program
int main(int argc, char **argv){
    
    printf("[Main] Execute version: %s\n\n", "high-level");
    if(argc == 1){

        // Variables for files
        FILE* file_from;
        FILE* file_to;
        int result;

        // Open first(source) file
        file_from = getFileFromPath("Enter destination file path", "r+", NULL);
        if(file_from == NULL) return RETURN_COULDNT_OPEN_FILE;

        // Open second(destination) file
        file_to = getFileFromPath("Enter destination file path", "a+", NULL);
        if(file_to == NULL) return RETURN_COULDNT_OPEN_FILE;

        result = copyFromSourceToDestination(file_from, file_to);

        // Closing and saving changes to particular files
        fclose(file_from);
        fclose(file_to);

        return RETURN_SUCCESS;
    }

    if(argc == 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provided path to 1 file - provide 2 file paths or don't provide any as a program argument");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    if(argc == 3){
        // Variables for files
        FILE* file_from;
        FILE* file_to;
        int result;

        // Open first(source) file
        file_from = getFileFromPath("Enter destination file path", "r+", argv[1]);
        if(file_from == NULL) return RETURN_COULDNT_OPEN_FILE;

        // Open second(destination) file
        file_to = getFileFromPath("Enter destination file path", "a+", argv[2]);
        if(file_to == NULL) return RETURN_COULDNT_OPEN_FILE;

        result = copyFromSourceToDestination(file_from, file_to);

        // Closing and saving changes to particular files
        fclose(file_from);
        fclose(file_to);

        return RETURN_SUCCESS;
    }

    if(argc > 3){
        error("INCORRECT_ARGUMENT_AMOUNT", "provided too many parameters as a program arguments");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    error("RETURN_UNKNOWN_ERROR", "program occured unknown problem");
    return RETURN_UNKNOWN_ERROR;
}