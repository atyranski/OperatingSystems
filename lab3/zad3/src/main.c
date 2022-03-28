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

// ZWERYFIKOWAC CZY WSZYSTKO JEST
char* getEntryType(unsigned char type_code){
    switch (type_code){
        case 4: return "dir";
        case 8: return "file";
        default:
            return "other";
        }
}

bool findInFile(char* word, FILE* file){
    char* buffer = BUFFER;
    bool result = false;
    int bytes_left = fread(buffer, sizeof(char), 3, file);
    
    while(bytes_left){
        if(strcmp(buffer, word) == 0) {
            result = true;
            printf("\033[1;31m%s\033[0m", buffer);

        } else printf("%s", buffer);

        bytes_left = fread(buffer, sizeof(char), 3, file);
    }

    return result;
}

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

void printDirectory(char* dir_path, char* word){
    // opendir() returns a pointer of DIR type. 
    DIR* dir = opendir(dir_path);

    // opendir returns NULL if couldn't open directory
    if (dir == NULL) {
        error("COUDNT_OPEN_DIRECTORY","Could not open current directory");
        return RETURN_COUDNT_OPEN_DIRECTORY;
    }

    char absolutePath[PATH_MAX];
    realpath(dir_path, absolutePath);

    printInfo("Processing", absolutePath);

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
        if(strcmp(getEntryType(entry->d_type), "file") == 0){
            char* filePath[PATH_MAX];
            sprintf(filePath, "%s/%s", dir_path, entry->d_name);

            FILE* file = getFileFromPath(filePath, "r+");
            
            if (findInFile(word, file)) {
               printf("> %s\n", entry->d_name); 
            }

            fclose(file);
        }
    }

    printf("\n");

    for(int i=0; i<dirAmount; i++) {
        char* nextPath[PATH_MAX];

        sprintf(nextPath, "%s/%s", dir_path, nextDirs[i]);
        printDirectory(nextPath, word);
    }
    
    free(nextDirs);

    if(closedir(dir) != 0){
        printf("%d\n", errno);
    }
}

// ---- Main program
int main(int argc, char **argv){
    
    printf("[Main] Execute version: %s\n\n", "opendir(), readdir() and stat functions");

    if(argc != 4){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide 1 argument: [catalog path]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    char* dirPath = argv[1];
    char* word = argv[2];
    int depth = atoi(argv[3]);

    if(argc == 4){
        printDirectory(dirPath, word);

        return RETURN_SUCCESS;
    }

    error("RETURN_UNKNOWN_ERROR", "program occured unknown problem");
    return RETURN_UNKNOWN_ERROR;
}