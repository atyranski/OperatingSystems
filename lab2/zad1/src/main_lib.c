#include <stdio.h>

#define RETURN_CODE_SUCCESS 0;

// ---- Main program
int main(int argc, char **argv){

    printf("[Main] Execute version: %s\n\n", "lib");
    if(argc == 1){
        printf("Provide file paths from console input\n");
    }

    if(argc == 2){
        // error("INCORRECT_ARGUMENT_AMOUNT", "provided path to 1 file - provide 2 file paths or don't provide any as a program argument");
    }

    if(argc == 3){
        for(int i=0; i<argc; i++){
            printf("%d: %s\n", i, argv[i]);
        }
    }

    if(argc > 3){
        // error("INCORRECT_ARGUMENT_AMOUNT", "provided too many parameters as a program arguments");
    }

    return RETURN_CODE_SUCCESS;
}