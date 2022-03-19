#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_UNKNOWN_ERROR -1
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -2
#define RETURN_COUDNT_OPEN_DIRECTORY -3
#define RETURN_INVALID_ENTRY_TYPECODE -4

// ZWERYFIKOWAC CZY WSZYSTKO JEST
char* getEntryType(unsigned char type_code){
    switch (type_code){
    case 1: return "fifo";
    case 2: return "char dev";
    case 4: return "dir";
    case 6: return "block dev";
    case 8: return "file";
    case 10: return "slink";
    case 12: return "sock";
    default:
        error("INVALID_ENTRY_TYPECODE", "program occured invalid type code for entry in directory");
        return NULL;
    }
}

struct dirent* getNextEntry(){

}

void printEntry(struct dirent* entry){
    char buffer[PATH_MAX];
    
    realpath(entry->d_name, buffer);

    printf("[%s]\t\tname: %s\n", getEntryType(entry->d_type), entry->d_name);
    printf("\033[0;34m%s\033[0m\n", buffer);
}

void printDirectory(char* dir_path){
    // opendir() returns a pointer of DIR type. 
    DIR* dir = opendir(dir_path);

    // opendir returns NULL if couldn't open directory
    if (dir == NULL) {
        error("COUDNT_OPEN_DIRECTORY","Could not open current directory");
        return RETURN_COUDNT_OPEN_DIRECTORY;
    }

    printInfo("Processing", dir_path);

    // Pointer for directory entry
    struct dirent *entry; 
    int dirAmount = 0;

    // for readdir()
    while ((entry = readdir(dir)) != NULL) {
        printEntry(entry);
        if( strcmp(getEntryType(entry->d_type), "dir") == 0 &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) dirAmount++;
    }

    printf("Subdirs found: %d\n\n", dirAmount);
    
    if(closedir(dir) != 0){
        printf("%d\n", errno);
    }
}

// ---- Main program
int main(int argc, char **argv){
    
    printf("[Main] Execute version: %s\n\n", "opendir(), readdir() and stat functions");

    if(argc == 2){
        printDirectory(argv[1]);


        return RETURN_SUCCESS;
    }

    if(argc != 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide 1 argument: [catalog path]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    error("RETURN_UNKNOWN_ERROR", "program occured unknown problem");
    return RETURN_UNKNOWN_ERROR;
}