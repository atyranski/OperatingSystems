#include "configs.h"

int elves_amount;
pthread_t *elves_threads;

int elves_complaining[MAX_ELVES_COMPLAINING];
int elves_complaining_amount = 0;

pthread_cond_t condition_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_elves = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_solved = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_access = PTHREAD_MUTEX_INITIALIZER;


double randomGenerator(){
    srand(time(NULL));
    return (double) rand() / (double) RAND_MAX;
}

int getDelayTime(int min, int max){
    return (randomGenerator() * (max - min) + min) * 1000;
}

void *elves_actions(void *args){
    Arguments *arguments = args;

    printOper("ELF", "starting thread");

    while(true){
        int time_working = getDelayTime(2000, 5000);
        int time_solving = getDelayTime(1000, 2000);
        char message[1000];

        // printf("WORKING: %d\n", time_working);

        usleep(time_working);

        pthread_mutex_lock(arguments->mutex_access);

        while(*(arguments->elves_complaining_amount) >= MAX_ELVES_COMPLAINING){

            sprintf(message, "czeka na powrót elfów %d", arguments->id);
            printOper("ELF", message);

            pthread_cond_wait(arguments->condition_elves, arguments->mutex_access);
        }

        arguments->elves_complaining[*(arguments->elves_complaining_amount)] = arguments->id;
        (*arguments->elves_complaining_amount)++;

        strcpy(message, "");
        sprintf(message, "czeka %d elfów na Mikołaja, %d", *arguments->elves_complaining_amount, arguments->id);
        printOper("ELF", message);

        if(*(arguments->elves_complaining_amount) >= MAX_ELVES_COMPLAINING){
            strcpy(message, "");
            sprintf(message, "wybudzam Mikołaja, %d", arguments->id);
            printOper("ELF", message);

            pthread_cond_broadcast(arguments->condition_santa);
        }

        pthread_cond_wait(arguments->condition_solved, arguments->mutex_access);

        strcpy(message, "");
        sprintf(message, "Mikołaj rozwiązuje problem %d", arguments->id);
        printOper("ELF", message);

        usleep(time_solving);

        pthread_cond_wait(arguments->condition_elves, arguments->mutex_access);

        pthread_mutex_unlock(arguments->mutex_access);
    }
}

void *santa_actions(void *args){
    Arguments *arguments = args;

    printInfo("SANTA", "starting thread");

    while(true){
        while(*(arguments->elves_complaining_amount) < MAX_ELVES_COMPLAINING){
            pthread_cond_wait(arguments->condition_santa, arguments->mutex_access);
        }

        printInfo("SANTA", "budzę się");
        int time_sleeping = getDelayTime(2000, 3000);
        int time_solving = getDelayTime(1000, 2000);
        char message[100];

        sprintf(message, "rozwiązuje problemy elfów %d %d %d", arguments->elves_complaining[0],
        arguments->elves_complaining[1],
        arguments->elves_complaining[2]);

        printInfo("SANTA", message);

        pthread_cond_broadcast(arguments->condition_solved);
        usleep(time_solving);

        printInfo("SANTA", "zasypiam");

        usleep(time_sleeping);

        pthread_cond_broadcast(arguments->condition_elves);
        *arguments->elves_complaining_amount = 0;
    }
}

pthread_t createThread(int id, bool isSanta){
    pthread_t thread;

    Arguments *arguments = calloc(1, sizeof(Arguments));
    arguments->id = id;
    arguments->elves_complaining = elves_complaining;
    arguments->elves_complaining_amount = &elves_complaining_amount;
    arguments->condition_santa = &condition_santa;
    arguments->condition_elves = &condition_elves;
    arguments->condition_solved = &condition_solved;
    arguments->mutex_access = &mutex_access;


    if(isSanta){
        if(pthread_create(&thread, NULL, santa_actions, arguments) != 0){
            error("COULDNT_CREATE_THREAD", "program occured problem with creating new thread for elf");
            return RETURN_COULDNT_CREATE_THREAD;
        }
    } else {
        if(pthread_create(&thread, NULL, elves_actions, arguments) != 0){
            error("COULDNT_CREATE_THREAD", "program occured problem with creating new thread for elf");
            return RETURN_COULDNT_CREATE_THREAD;
        }
    }


    return thread;
}

// ---- Main program
int main(int argc, char **argv){


    // Validation of arguments
    if(argc != 2){
        error("INCORRECT_ARGUMENTS", "./bin/main [elves-amount]");
        return RETURN_INCORRECT_ARGUMENTS;
    }

    // Some initializations
    elves_amount = atoi(argv[1]);
    elves_threads = calloc(elves_amount, sizeof(pthread_t));

    // Creating elves
    for (int i = 0; i < elves_amount; ++i){
        elves_threads[i] = createThread(i, false);
    }

    // Waiting for all threads (elves) to finish their job
    pthread_join(createThread(10, true), NULL);

    // Stopping elves threads
    for (int i = 0; i < elves_amount; i++) {
        pthread_cancel(elves_threads[i]);
    }

    free(elves_threads);

    return RETURN_SUCCESS;
}