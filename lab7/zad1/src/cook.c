#include "configs.h"

int openSemaphore(){
    int descriptor = -1;

    if((descriptor = semget(key, SEMAPHORE_AMOUNT, 0)) == -1){
        error("COULDNT_OPEN_SEMAPHORE", "program occured problem with opening existing semaphore");
        return RETURN_COULDNT_OPEN_SEMAPHORE;
    }

    return descriptor;
}

int openSharedMemory(){
    int descriptor = -1;

    if((descriptor = shmget(key, 0, 0)) == -1){
        error("COULDNT_OPEN_SHARED_MEMORY", "program occured problem with creating shared memory");
        return RETURN_COULDNT_OPEN_SHARED_MEMORY;
    }

    return descriptor;
}

void handleInterrupt(){
    running = false;
    shmdt(pizza_house);
    exit(0);
}

int findFreeSpotInOven(){
    for(int i=0; i<OVEN_CAPACITY; i++){
        if(pizza_house->oven[i] == -1) return i;
    }

    return -1;
}

int findFreeSpotOnTable(){
    for(int i=0; i<TABLE_CAPACITY; i++){
        if(pizza_house->table[i] == -1) return i;
    }

    return -1;
}

int getDelayTime(int time){
    return (time + (rand() % 1000)) * 1000;
}

void routine(pid_t pid, int semaphores){
    int pizza = rand() % 10;

    int time_prepare = getDelayTime(TIME_PREPARE);
    int time_bake = getDelayTime(TIME_BAKE);
    int time_rest = getDelayTime(TIME_REST);
        
    bool is_place = false;
    int oven_spot = -1;
    int table_spot = -1;
    struct sembuf operation = {OVEN_ID, -1, 0};

    printPrepare(pid, pizza);
    usleep(time_prepare);

    while(!is_place){
        semop(semaphores, &operation, 1);

        if(pizza_house->oven_amount < OVEN_CAPACITY){
            is_place = true;
            oven_spot = findFreeSpotInOven();

            pizza_house->oven[oven_spot] = pizza;
            pizza_house->oven_amount++;
        }

        operation.sem_op = 1;
        semop(semaphores, &operation, 1);
    }

    printBake(pid, pizza, pizza_house->oven_amount);
    usleep(time_bake);
    is_place = false;

    while(!is_place){
        operation.sem_op = -1;
        operation.sem_num = TABLE_ID;
        semop(semaphores, &operation, 1);

        if(pizza_house->table_amount < TABLE_CAPACITY){
            is_place = true;
            table_spot = findFreeSpotOnTable();

            operation.sem_num = OVEN_ID;
            semop(semaphores, &operation, 1);

            pizza_house->oven[oven_spot] = -1;
            pizza_house->oven_amount--;

            operation.sem_op = 1;
            semop(semaphores, &operation, 1);

            pizza_house->table[table_spot] = pizza;
            pizza_house->table_amount++;
        }

        operation.sem_num = TABLE_ID;
        operation.sem_op = 1;
        semop(semaphores, &operation, 1);
    }

    printPullOut(pid, pizza, pizza_house->oven_amount, pizza_house->table_amount);
    usleep(time_rest);
}

// ---- Main program
int main(int argc, char **argv){
    pid_t pid = getpid();
    char message[100];

    createKey();

    int semaphores = openSemaphore();
    int shared_memory = openSharedMemory();
    if(semaphores < 0 || shared_memory < 0) return -1;

    pizza_house = shmat(shared_memory, NULL, 0);
    if(pizza_house == (void *) -1){
        error("COULDNT_OPEN_SHARED_MEMORY", "cannot reach data from shared memory");
        return RETURN_COULDNT_OPEN_SHARED_MEMORY;
    }

    sprintf(message, "cook with pid #%d starting work", pid);
    printInfo("START", message);

    signal(SIGINT, handleInterrupt);
    srand(time(NULL));

    while(running){
        routine(pid, semaphores);
    }


    return RETURN_SUCCESS;
}