#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -1
#define RETURN_COULDNT_OPEN_FILE -2

typedef struct {
    char *name;
    char **comands;
    int commands_amount;
} Component;

typedef struct {
    char **components_names;
    int names_amount;
} Sequence;

typedef struct {
    Component **components;
    Sequence **sequences;
    int secquences_amount;
} Program;


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

char *getFileContent(FILE *file){
    char * file_content;
    size_t file_size;

	fseek(file, 0, SEEK_END);
    file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
    
	file_content = malloc(sizeof(char)*(file_size+1));
	file_content[file_size] = 0;

	fread(file_content, sizeof(char), file_size, file);

    return file_content;
}

void printComponent(Component *component){
    printCheck("Component");
    printf("[name]: %s\n", component->name);
    printf("[commands]: \n\t> %s", component->comands[0]);
    if(component->comands[1] != NULL){
        printf("\n\t> %s", component->comands[1]);

        if(component->comands[2] != NULL){
            printf("\n\t> %s\n", component->comands[2]);
        } else {
            printf("\n");
        }

    } else {
        printf("\n");
    }
    printf("[commands_amount]: %d\n\n", component->commands_amount);
}

void printSequence(Sequence *sequence){
    printCheck("Sequence");
    printf("[components_names]: \n\t> %s", sequence->components_names[0]);
    if(sequence->components_names[1] != NULL){
        printf("\n\t> %s", sequence->components_names[1]);

        if(sequence->components_names[2] != NULL){
            printf("\n\t> %s\n", sequence->components_names[2]);
        } else {
            printf("\n");
        }

    } else {
        printf("\n");
    }
    printf("[names_amount]: %d\n\n", sequence->names_amount);
}

Component *parseComponent(char *line){
    printInfo("Parsing Component", line);

    Component *newComponent = malloc(sizeof(Component));
    // assumption that any component can handle max 3 commands of length 50 characters each
    newComponent->comands = calloc(3*50, sizeof(char));

    // Getting component name
    char *start, *end, *name;
    int lenght;
    start = line;

    while (*line != 0 && *line != ' ') line++;

    line++;
    end = line;

    lenght = end - start-1;
    name = malloc(lenght * sizeof(char));

    for(int i=0; i<lenght; i++){
        name[i] = *(start + i);
    }

    newComponent->name = name;
    line++;
    int i=0;

    while(*line != 0){
        line++;
        start = line;

        while (*line != 0 && *line != '|') line++;

        line++;
        end = line;

        lenght = end - start-1;
        name = malloc(lenght * sizeof(char));

        for(int i=0; i<lenght; i++){
            name[i] = *(start + i);
        }

        newComponent->comands[i] = name;
        i++;

    }

    newComponent->commands_amount = i;

    printComponent(newComponent);

    return newComponent;
}

Sequence *parseSequence(char *line){
    printInfo("Parsing Sequence", line);

    Sequence *newSequence = malloc(sizeof(Sequence));
    // assumption that any component can handle max 3 commands of length 50 characters each
    newSequence->components_names = calloc(3*50, sizeof(char));

    char *start, *end, *name;
    int lenght;

    int i=0;

    while(*line != 0){
        start = line;

        while (*line != 0 && *line != '|') line++;

        line++;
        end = line;

        lenght = end - start-1;
        name = malloc(lenght * sizeof(char));

        for(int i=0; i<lenght; i++){
            name[i] = *(start + i);
        }

        newSequence->components_names[i] = name;
        i++;

        line++;

    }

    newSequence->names_amount = i;

    printSequence(newSequence);

}


void readLines(char *text){
    printInfo("Initialization", "reading lines");

    char *start, *end;
    bool isComponent = true;

    while(*text != 0){
        start = text;

        while(*text != 0 && (int) *text != 10) {
            text++;
        }

        text++;
        end = text;

        int lenght = end - start-1;
        char *line = malloc(lenght * sizeof(char));

        for(int i=0; i<lenght; i++){
            line[i] = *(start + i);
        }

        if(lenght == 0) {
            isComponent = false;
            continue;
        }

        if(isComponent) parseComponent(line);
        else parseSequence(line);

        free(line);
    }
    
}

Program *parseFile(FILE *file){
    printInfo("Initialization", "start program");
    Program *program = malloc(sizeof(Program));
    char *file_content = getFileContent(file);

    // printf("%s\n", file_content);
    readLines(file_content);

    return program;
}

// ---- Main program
int main(int argc, char **argv){

    if(argc != 2){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide path to text file with commands to execute");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    FILE *file;

    Program *program;

    // Open file
    file = getFileFromPath(argv[1], "r+");
    if(file == NULL) return RETURN_COULDNT_OPEN_FILE;

    // Get data from file and put into structs
    program = parseFile(file);

    
    close(file);
    free(program);

    return RETURN_SUCCESS;
}