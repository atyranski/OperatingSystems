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
#define RETURN_COULDNT_PIPE -3
#define RETURN_COUDNT_FORK_PROCESS -4

typedef struct {
    char *name;
    char **comands;
    int commands_amount;
    int capacity;
} Component;

typedef struct {
    char **components_names;
    int names_amount;
    int capacity;
} Sequence;

typedef struct {
    Component **components;
    int components_amount;
    int secquences_amount;
    Sequence **sequences;
    int components_capacity;
    int secquences_capacity;
} Program;

int** getDescriptors(int amount){
    int **descriptors = calloc(amount, sizeof(int*));

    for(int i=0; i<amount; ++i){
        descriptors[i] = calloc(2, sizeof(int));

        if(pipe(descriptors[i]) == -1){
            error("COULDNT_PIPE", "program occured problem and couldnt make pipe");
            return RETURN_COULDNT_PIPE;
        }
    }

    // descriptors[amount-1] = calloc(2, sizeof(int));
    descriptors[amount-1][0] = STDIN_FILENO;
    descriptors[amount-1][1] = STDOUT_FILENO;

    return descriptors;
}

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
    printf("[commands]: \n");
    for(int i=0; i<component->commands_amount; i++) printf("\t> %s\n", component->comands[i]);
    printf("[amount]: %d\n", component->commands_amount);
    printf("[capacity]: %d\n\n", component->capacity);
}

void printSequence(Sequence *sequence){
    printCheck("Sequence");
    printf("[components_names]: \n");
    for(int i=0; i<sequence->names_amount; i++) printf("\t> %s\n", sequence->components_names[i]);
    printf("[amount]: %d\n", sequence->names_amount);
    printf("[capacity]: %d\n", sequence->capacity);
}

void printProgram(Program *program){
    // printComponent(program->components[0]);
    printInfo("Printing", "program");
    printf("[components]:\n\t> amount: %d\n\t> capacity: %d\n", program->components_amount, program->components_capacity);
    printf("[sequences]:\n\t> amount: %d\n\t> capacity: %d\n", program->secquences_amount, program->secquences_capacity);
    for(int i=0; i<program->components_amount; i++) printComponent(program->components[i]);
    for(int i=0; i<program->secquences_amount; i++) printSequence(program->sequences[i]);
}

void enlargeComponent(Component *component){
    component->capacity *= 2;
    component->comands = realloc(component->comands, component->capacity*50);
}

void enlargeSequence(Sequence *sequence){
    sequence->capacity *= 2;
    sequence->components_names = realloc(sequence->components_names, sequence->capacity*50);
}

void enlargeProgramComponents(Program *program){
    program->components_capacity *= 2;
    program->components = realloc(program->components, program->components_capacity);
}

void enlargeProgramSequences(Program *program){
    program->secquences_capacity *= 2;
    program->sequences = realloc(program->sequences, program->secquences_capacity);
}

void freeComponent(Component *component){
    free(component->name);
    free(component->comands);
    free(component);
}

void freeSequence(Sequence *sequence){
    free(sequence->components_names);
    free(sequence);
}

void freeProgram(Program *program){
    for(int i=0; i<program->components_amount; i++) freeComponent(program->components[i]);
    for(int i=0; i<program->secquences_amount; i++) freeSequence(program->sequences[i]);
}

void freeDescriptors(int **descriptors, int size){
    for(int i=0; i<size; i++) free(descriptors[i]);
    free(descriptors);
}

Component *parseComponent(char *line){
    printInfo("Parsing Component", line);

    Component *newComponent = calloc(1, sizeof(Component));
    // assumption that any component can handle max 3 commands of length 50 characters each
    newComponent->comands = calloc(3*50, sizeof(char));
    newComponent->capacity = 3;

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

        if(i == newComponent->capacity) enlargeComponent(newComponent);
        newComponent->commands_amount = i;
        newComponent->comands[i] = name;
        i++;
    }
    
    newComponent->commands_amount = i;

    // printComponent(newComponent);

    return newComponent;
}

Sequence *parseSequence(char *line){
    printInfo("Parsing Sequence", line);

    Sequence *newSequence = calloc(1, sizeof(Sequence));
    // assumption that any component can handle max 3 commands of length 50 characters each
    newSequence->components_names = calloc(3*50, sizeof(char));
    newSequence->capacity = 3;

    char *start, *end, *name;
    int lenght;

    int i=0;

    while(*line != 0){
        start = line;

        while (*line != 0 && *line != ' ') line++;

        line++;
        end = line;

        lenght = end - start-1;
        if(lenght == 0) continue;

        name = malloc(lenght * sizeof(char));
        for(int i=0; i<lenght; i++){
            name[i] = *(start + i);
        }

        if(i == newSequence->capacity) enlargeSequence(newSequence);
        newSequence->names_amount = i;
        newSequence->components_names[i] = name;
        i++;
        line++;
    }

    newSequence->names_amount = i;

    // printSequence(newSequence);

    return newSequence;
}

void readLines(char *text, Program *program){
    printInfo("Initialization", "reading lines");

    char *start, *end;
    bool isComponent = true;
    int index = 0;

    while(*text != 0){
        start = text;

        while(*text != 0 && (int) *text != 10) {
            text++;
        }

        text++;
        end = text;

        int lenght = end - start-1;
        char *line = calloc(lenght, sizeof(char));

        for(int i=0; i<lenght; i++){
            line[i] = *(start + i);
        }

        if(lenght == 0) {
            isComponent = false;
            index = 0;
            continue;
        }

        if(isComponent) {
            Component *newComponent = parseComponent(line);

            if(program->components_capacity == index) enlargeProgramComponents(program);

            program->components[index] = newComponent;
            program->components_amount++;

        } else {
            Sequence *newSequecnce = parseSequence(line);

            if(program->secquences_capacity == index) enlargeProgramSequences(program);

            program->sequences[index] = newSequecnce;
            program->secquences_amount++;

        }

        free(line);
        index++;
    }
    
}

Program *parseProgram(FILE *file){
    printInfo("Initialization", "start program");
    Program *program = calloc(1, sizeof(Program));
    program->components_capacity = 3;
    program->components_amount = 0;
    program->components = calloc(program->components_capacity, sizeof(Component*));
    program->secquences_capacity = 3;
    program->secquences_amount = 0;
    program->sequences = calloc(program->secquences_capacity, sizeof(Sequence*));

    char *file_content = getFileContent(file);

    // printf("%s\n", file_content);
    readLines(file_content, program);

    // printProgram(program);

    return program;
}

int getCommandAmount(Sequence *sequence, Component **components, int components_amount){
    int result = 0;

    for(int i=0; i<sequence->names_amount; i++){
        for(int j=0; j<components_amount; j++){
            if(strcmp(sequence->components_names[i], components[j]->name) == 0) {
                // printf("\t>%s\t%d\n", sequence->components_names[i], components[j]->commands_amount);
                result += components[j]->commands_amount;
            }
        }
    }

    return result;
}

char **getCurrentCommand(Sequence *sequence, Component **components, int components_amount, int position){
    int n = 0;
    char **command = calloc(2, sizeof(char*));
    // Command name max length 15, args max length 34
    command[0] = calloc(15, sizeof(char));
    command[1] = calloc(35, sizeof(char));
    char *start, *end;
    int lenght;

    for(int i=0; i<sequence->names_amount; i++){
        for(int j=0; j<components_amount; j++){
            if(strcmp(sequence->components_names[i], components[j]->name) == 0) {
                for(int k=0; k<components[j]->commands_amount; k++){
                    if(n == position) {
                        char *text = components[j]->comands[k];

                        // Setting command name
                        start = text;

                        while(*text != 0 && *text != ' ') {
                            text++;
                        }

                        text++;
                        end = text;

                        lenght = end - start-1;

                        for(int i=0; i<lenght; i++){
                            command[0][i] = *(start + i);
                        }

                        // Setting args
                        start = text;

                        while(*text != 0 && *text != ' ') {
                            text++;
                        }

                        text++;
                        end = text;

                        lenght = end - start-1;

                        for(int i=0; i<lenght; i++){
                            command[1][i] = *(start + i);
                        }

                        return command;
                    } else n++;
                }
            }
        }
    }

    return command;
}

void executeProgram(Program *program){

    // processing every sequence in program
    for(int i=0; i<program->secquences_amount; i++){
        printf("\nSequence: %d\n", i+1);
        Sequence *sequence = program->sequences[i];
        // printSequence(sequence);
         
        // getting current command component amount
        int command_amount = getCommandAmount(sequence, program->components, program->components_amount);
        // printf("commands: %d\n", command_amount);

        // generating new set of descriptors that fits amount of our components in command
        int **descriptors = getDescriptors(command_amount);

        int position = 0;
        char **command_current;

        // processing every component in sequence
        for(int j=0; j<command_amount; j++){

            // getting new component from command based on the position
            command_current = getCurrentCommand(sequence, program->components, program->components_amount,  position);
            // printf("Current command: [name] %s\t[args] %s\n", command_current[0], command_current[1]);

            // getting position of the previously used descriptor
            int prev_position = (position + command_amount - 1) % command_amount;

            int pid = fork();

            // handling issues during the fork operation
            if(pid == -1){
                error("COUDNT_FORK_PROCESS", "program occured problem with creating a new process");
                return RETURN_COUDNT_FORK_PROCESS;
            }

            // entering child process
            if(pid == 0){
                // printf("index: %d prev: %d\n", position, prev_position);
                // printf("Current command: [name] %s\t[args] %s\n", command_current[0], command_current[1]);

                // closing unused descriptors in this child proces 
                for(int k=0; k<command_amount; ++k){
                    if(k != position && k != prev_position){
                        if (descriptors[k][0] != STDIN_FILENO) close(descriptors[k][0]);
                        if (descriptors[k][1] != STDOUT_FILENO) close(descriptors[k][1]);
                    }
                }
                close(descriptors[position][0]);
                close(descriptors[prev_position][1]);
                // printf("index: %d prev: %d\n", position, prev_position);

                // duplicate the standard i/o descriptors to descriptors at our positions
                dup2(descriptors[prev_position][0], STDIN_FILENO);
                dup2(descriptors[position][1], STDOUT_FILENO);
                
                // executing command
                char *args[] = {command_current[0], command_current[1], NULL};
                execvp(command_current[0], args);
                
                return RETURN_SUCCESS;
            }

            position++;
        }

        // closing decsriptors for the parent process to avoid stopping execution of program
        for (int p=0; p<command_amount; ++p){
            if(descriptors[p][0] != STDIN_FILENO) close(descriptors[p][0]);
            if(descriptors[p][1] != STDOUT_FILENO) close(descriptors[p][1]);
        }

        // waiting for all child processes to finish
        int status = 0;
        while ((wait(&status)) > 0);

        // free all descriptors and command for this sequence
        freeDescriptors(descriptors, command_amount);
        free(command_current);

    }
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
    program = parseProgram(file);

    executeProgram(program);
    
    // Close files and free memory
    close(file);
    freeProgram(program);

    return RETURN_SUCCESS;
}