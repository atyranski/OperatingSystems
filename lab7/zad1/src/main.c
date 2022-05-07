#include "configs.h"

pid_t *child_pids;
int child_amount;

int createSemaphores(){
    int descriptor = -1;

    printInfo("INITIALIZE", "semaphores");

    if((descriptor = semget(key, SEMAPHORE_AMOUNT, IPC_CREAT | IPC_EXCL | 0666)) == -1){
        
        if((descriptor = semget(key, SEMAPHORE_AMOUNT, 0666)) != -1){
            
            // usuniecie i wygenerowanie semafora od nowa
            semctl(descriptor, 0, IPC_RMID);
            descriptor = semget(descriptor, SEMAPHORE_AMOUNT, IPC_CREAT | IPC_EXCL | 0666);

        }
    }

    if(descriptor == -1){
        error("COULDNT_CREATE_SEMAPHORE", "program occured problem with creating semaphore");
        return RETURN_COULDNT_CREATE_SEMAPHORE;
    }

    printOper("SUCCESS", "semaphores initialized");

    return descriptor;
}

int createSharedMemory(){
    int descriptor = -1;
    size_t size = sizeof(PizzaHouse);

    printInfo("INITIALIZE", "shared memory");

    if((descriptor = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666)) == -1){
        
        if((descriptor = shmget(key, 0, 0666)) != -1){
            
            // usuniecie i wygenerowanie semafora od nowa
            shmctl(descriptor, IPC_RMID, NULL);
            descriptor = shmget(descriptor, size, IPC_CREAT | IPC_EXCL | 0666);

        }
    }

    if(descriptor == -1){
        error("COULDNT_CREATE_SHARED_MEMORY", "program occured problem with creating shared memory");
        return RETURN_COULDNT_CREATE_SHARED_MEMORY;
    }

    printOper("SUCCESS", "shared memory initialized");


    return descriptor;
}

void initializePizzaHouse(PizzaHouse *object){
    object->oven_amount = 0;
    object->table_amount = 0;

    for(int i=0; i<OVEN_CAPACITY; i++) object->oven[i] = -1;
    for(int i=0; i<TABLE_CAPACITY; i++) object->table[i] = -1;
}

void handleInterrupt(){
    for(int i=0; i<child_amount; i++) kill(child_pids[i], SIGINT);
    exit(0);
}

// ---- Main program
int main(int argc, char **argv){

    if(argc != 3){
        error("INCORRECT_ARGUMENT_AMOUNT", "[exec_path] [cooks_amount] [deliverers_amount]");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    int cook_amount = atoi(argv[1]);
    int deliverer_amount = atoi(argv[2]);
    if(cook_amount < 0 || deliverer_amount < 0){
        error("INCORRECT_ARGUMENT", "number of cooks and deliverers need to be positive");
        return RETURN_INCORRECT_ARGUMENT;
    }

    createKey();
    if (key == -1) return -1;

    int semaphores = createSemaphores();
    int shared_memory = createSharedMemory();
    if(semaphores < 0 || shared_memory < 0) return -1;

    pizza_house = shmat(shared_memory, NULL, 0);
    initializePizzaHouse(pizza_house);

    union semun arg;
    arg.val = 1;

    semctl(semaphores, OVEN_ID, SETVAL, arg);
    semctl(semaphores, TABLE_ID, SETVAL, arg);

    pid_t pid;
    child_amount = cook_amount * deliverer_amount;
    child_pids = calloc(child_amount, sizeof(pid_t));

    for(int i=0; i<cook_amount; i++){
        if((pid = fork()) == 0){


            char *args[] = {"./bin/cook",NULL};
            execvp(args[0],args);

        } else {
            child_pids[i] = pid;
        }
    }

    for(int i=0; i<deliverer_amount; i++){
        if((pid = fork()) == 0){

            char *args[] = {"./bin/deliverer", NULL};
            execvp(args[0],args);

        } else {
            child_pids[cook_amount + i] = pid;
        }
    }

    // main program waiting for termination
    // cook and deliverer child processes are going on
    signal(SIGINT, handleInterrupt);

    // after interupt main program wait untill every
    // child process stops
    int status = 0;
    while ((wait(&status)) > 0);

    free(child_pids);       // free alocated memory
    shmdt(pizza_house);     // disconnect shared memory segment
    semctl(semaphores, 0, IPC_RMID);    // remove semaphore
    shmctl(shared_memory, IPC_RMID, NULL); // remove shared memory segment

    return RETURN_SUCCESS;
}