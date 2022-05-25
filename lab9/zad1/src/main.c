#include "configs.h"

int elves_amount;
pthread_t *elves_threads;

int elves_in_workshop[MAX_ELVES_WORKING];
int elves_in_workshop_amount = 0;

pthread_cond_t condition_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_elves = PTHREAD_COND_INITIALIZER;

pthread_mutex_t access = PTHREAD_MUTEX_INITIALIZER;

int getDelayTime(int time, int difference){
    return (time + (rand() % difference)) * difference;
}

void *elves_actions(void *args){
    Arguments *arguments = args;
    srand(time(NULL));

    printOper("ELF", "starting thread");

    while(true){
        int time_working = getDelayTime(TIME_WORKING, DIFFERENCE_WORKING);
        char message[1000];

        pthread_mutex_lock(arguments->access);

        while(arguments->elves_in_workshop_amount >= MAX_ELVES_WORKING){

            sprintf(message, "czeka na powrót elfów %d", arguments->id);
            printOper("ELF", message);

            pthread_cond_wait(arguments->condition_elves, arguments->access);
        }

        arguments->elves_in_workshop[*(arguments->elves_in_workshop_amount)] = arguments->id;
        arguments->elves_in_workshop_amount++;

        strcpy(message, "");
        sprintf(message, "czeka %d elfów na Mikołaja, %d", arguments->elves_in_workshop_amount, arguments->id);
        printOper("ELF", message);

        if(arguments->elves_in_workshop_amount >= MAX_ELVES_WORKING){
            strcpy(message, "");
            sprintf(message, "wybudzam Mikołaja, %d", arguments->id);
            printOper("ELF", message);

            pthread_cond_broadcast(arguments->condition_santa);
        }

        pthread_cond_wait(arguments->condition_elves, arguments->access);

        strcpy(message, "");
        sprintf(message, "Mikołaj rozwiązuje problem %d", arguments->id);
        printOper("ELF", message);

        pthread_mutex_unlock(arguments->access);
    }
}

void *santa_actions(void *args){
    Arguments *arguments = args;
    srand(time(NULL));

    printInfo("SANTA", "starting thread");

    while(true){
        while(arguments->elves_in_workshop_amount < MAX_ELVES_WORKING){
            pthread_cond_wait(arguments->condition_santa, arguments->access);
        }

        printInfo("SANTA", "budzę się");
        int time_sleeping = getDelayTime(TIME_SLEEPING, DIFFERENCE_SLEEPING);

        if(arguments->elves_in_workshop_amount >= MAX_ELVES_WORKING){
            char message[100];
            int time_solving = getDelayTime(TIME_SOLVING, DIFFERENCE_SOLVING);

            sprintf(message, "rozwiązuje problemy elfów %d %d %d", arguments->elves_in_workshop[0],
            arguments->elves_in_workshop[1],
            arguments->elves_in_workshop[2]);

            printInfo("SANTA", message);

            usleep(time_solving);

            arguments->elves_in_workshop_amount = 0;

            pthread_cond_broadcast(arguments->condition_elves);
        }

        printInfo("SANTA", "zasypiam");

        usleep(time_sleeping);
    }
}

pthread_t createElfThread(int id){
    pthread_t thread;

    Arguments *arguments = calloc(1, sizeof(Arguments));
    arguments->id = id;
    arguments->elves_amount = elves_amount;
    arguments->elves_in_workshop = elves_in_workshop;
    arguments->elves_in_workshop_amount = elves_in_workshop_amount;
    arguments->condition_santa = &condition_santa;
    arguments->condition_elves = &condition_elves;
    arguments->access = &access;

    if(pthread_create(&thread, NULL, elves_actions, arguments) != 0){
        error("COULDNT_CREATE_THREAD", "program occured problem with creating new thread for elf");
        return RETURN_COULDNT_CREATE_THREAD;
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
        elves_threads[i] = createElfThread(i);
    }

    // Waiting for all threads (elves) to finish their job
    pthread_join(create_thread(0, santa_actions), NULL);

    // Stopping elves threads
    for (int i = 0; i < elves_amount; ++i) {
        pthread_cancel(elves_threads[i]);
    }

    // Waiting for all threads to finish.
    for (int i = 0; i < elves_amount; ++i) {
        void* return_value = NULL;

        if (pthread_join(elves_threads[i], &return_value) != 0){
            error("COULDNT_JOIN_ELF", "program occured problem with getting return value from elf");
            return RETURN_COULDNT_JOIN_ELF;

        } else if (return_value != PTHREAD_CANCELED) {
            fprintf(stderr, "The thread didn't finish by cancelling.\n");

        }
    }

    close(elves_threads);

    return RETURN_SUCCESS;
}