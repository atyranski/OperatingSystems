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

int count_fifo = 0;
int count_char_dev = 0;
int count_dir = 0;
int count_block_dev = 0;
int count_file = 0;
int count_slink = 0;
int count_sock = 0;

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

void countType(unsigned char type){
    switch (type){
        case 1: count_fifo++; break;
        case 2: count_char_dev++; break;
        case 4: count_dir++; break;
        case 6: count_block_dev++; break;
        case 8: count_file++; break;
        case 10: count_slink++; break;
        case 12: count_sock++; break;
        default:
            error("INVALID_ENTRY_TYPECODE", "program occured invalid type code for entry in directory");
            return NULL;
    }
}

void printTypeSummary(){
    printf("Type summary:\n");
    printf("fifo:\t\t%d\n", count_fifo);
    printf("char dev:\t%d\n", count_char_dev);
    printf("dir:\t\t%d\n", count_dir);
    printf("block dev:\t%d\n", count_block_dev);
    printf("file:\t\t%d\n", count_file);
    printf("slink:\t\t%d\n", count_slink);
    printf("sock:\t\t%d\n", count_sock);
    printf("----------------------\n");
}

void printEntry(struct dirent* entry){
    char absolutePath[PATH_MAX];
    struct tm* modificationTime;
    struct tm* accessTime;
    struct stat statistics;

    char* modification[40];
    char* access[40];

    realpath(entry->d_name, absolutePath);
    stat(absolutePath, &statistics);

    if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, ".." ) != 0){
        modificationTime = localtime((const time_t *) &statistics.st_mtime);
        accessTime = localtime((const time_t *) &statistics.st_atime);

        strftime(modification, 40, "modified:\t%d.%m.%Y", modificationTime);
        strftime(access, 40, "accessed:\t%d.%m.%Y", accessTime);

        printf("name:\t%s\ntype:\t%s\nsize:\t%ld\nhardlinks:\t%ld\n", entry->d_name, getEntryType(entry->d_type), statistics.st_size, statistics.st_nlink);
        printf("%s\n%s\n", modification, access);
        printf("\033[0;34m%s\033[0m\n", absolutePath);
    }
    
    countType(entry->d_type);
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
            strcmp(entry->d_name, "..") != 0) {
                dirAmount++;
            }
    }

    printf("Subdirs found: %d\n\n", dirAmount);
    
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
    }

    for(int i=0; i<dirAmount; i++) {
        char* nextPath[PATH_MAX];

        sprintf(nextPath, "%s/%s", dir_path, nextDirs[i]);

        // printf("%s\n", nextPath);

        printDirectory(nextPath);
    }
    
    free(nextDirs);

    if(closedir(dir) != 0){
        printf("%d\n", errno);
    }
}

// ---- Main program
int main(int argc, char **argv){
    
    printf("[Main] Execute version: %s\n\n", "opendir(), readdir() and stat functions");

    if(argc == 2){
        printDirectory(argv[1]);
        printTypeSummary();

        return RETURN_SUCCESS;
    }

    if(argc != 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide 1 argument: [catalog path]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    error("RETURN_UNKNOWN_ERROR", "program occured unknown problem");
    return RETURN_UNKNOWN_ERROR;
}