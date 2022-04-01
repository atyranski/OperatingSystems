#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_UNKNOWN_ERROR -1
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2
#define RETURN_COUDNT_OPEN_DIRECTORY -3
#define RETURN_INVALID_ENTRY_TYPECODE -4

#define _BUFFER_SIZE 256
static char BUFFER[_BUFFER_SIZE];

FILE* getFileFromPath(char* path, char* mode){
    
    // Variables
    FILE* file;

    // Checking if file exists
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

    return file;
}

// ZWERYFIKOWAC CZY WSZYSTKO JEST
char* getEntryType(unsigned char type_code){
    switch (type_code){
        case 4: return "dir";
        case 8: return "file";
        default:
            return "other";
        }
}

bool findInFile(char* word, char* file_path){
    FILE* file_traverse = getFileFromPath(file_path, "r+");
    FILE* file_search = getFileFromPath(file_path, "r+");
    bool result = false;
    int stringLenght = strlen(word);

    char* no_use_buff = BUFFER;
    char* buffer = BUFFER;

    int bytes_left = fread(no_use_buff, sizeof(char), 1, file_traverse);
    int no_use_int = fread(buffer, sizeof(char), stringLenght, file_search);

    while (bytes_left){
        if(strcmp(buffer, word) == 0) {
            result = true;
        }

        bytes_left = fread(no_use_buff, sizeof(char), 1, file_traverse);

        fpos_t position;
        fgetpos(file_traverse, &position);
        fsetpos(file_search, &position);
        no_use_int = fread(buffer, sizeof(char), stringLenght, file_search);
    }

    fclose(file_traverse);
    fclose(file_search);

    return result;
}

void printDirectory(char* dir_path, char* word,
                    int curr_depth, int max_depth){
    // opendir() returns a pointer of DIR type. 
    DIR* dir = opendir(dir_path);

    // opendir returns NULL if couldn't open directory
    if (dir == NULL) {
        error("COUDNT_OPEN_DIRECTORY","Could not open current directory");
        return RETURN_COUDNT_OPEN_DIRECTORY;
    }

    printInfo("Processing", dir_path);
    printf("depth: %d\n", curr_depth);

    // Pointer for directory entry
    struct dirent *entry; 
    int dirAmount = 0;

    // for readdir()
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp(getEntryType(entry->d_type), "dir") == 0 &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
                dirAmount++;
            }
    }

    // printf("Subdirs found: %d\n\n", dirAmount);
    
    char** nextDirs = calloc(dirAmount, sizeof(char*));
    int index = 0;

    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL) {
        if( strcmp(getEntryType(entry->d_type), "dir") == 0 &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
                nextDirs[index] = entry->d_name;
                index++;
            }
        if(strcmp(getEntryType(entry->d_type), "file") == 0 &&
            strstr(entry->d_name, ".txt") != NULL){
            char* filePath[PATH_MAX];
            sprintf(filePath, "%s/%s", dir_path, entry->d_name);

            if (findInFile(word, filePath)) {
               printf("> %s\n", entry->d_name); 
            }
        }
    }

    printf("\n");

    if(curr_depth < max_depth){
        for(int i=0; i<dirAmount; i++) {
            pid_t pid_next = fork();

            if(pid_next == 0){
                char* nextPath[PATH_MAX];

                sprintf(nextPath, "%s/%s", dir_path, nextDirs[i]);
                printf("\033[0;34mBelow catalog PID number:\033[0m #%d\n", getpid());
                printDirectory(nextPath, word, curr_depth+1, max_depth);

                return RETURN_SUCCESS;
            }

            while (wait(NULL) > 0);

        }
    }
    
    free(nextDirs);

    if(closedir(dir) != 0){
        printf("%d\n", errno);
    }
}

// ---- Main program
int main(int argc, char **argv){

    if(argc != 4){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide 1 argument: [catalog path]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    char* dirPath = argv[1];
    char* word = argv[2];
    int depth = atoi(argv[3]);

    printf("\n\n\n########\nSearching for '%s' in %s\n########\n", word, dirPath);

    if(argc == 4){
        pid_t pid = fork();

        if(pid == 0 ) {
            printf("\033[0;34mBelow catalog PID number:\033[0m #%d\n", getpid());
            printDirectory(dirPath, word, 1, depth);

            return RETURN_SUCCESS;
        }

        while (wait(NULL) > 0);

        return RETURN_SUCCESS;
    }



    error("RETURN_UNKNOWN_ERROR", "program occured unknown problem");
    return RETURN_UNKNOWN_ERROR;
}