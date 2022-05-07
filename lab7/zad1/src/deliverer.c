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

int getDelayTime(int time){
    return (time + (rand() % 1000)) * 1000;
}

int getPizzaFromTable(){
    for(int i=0; i<TABLE_CAPACITY; i++){
        if(pizza_house->table[i] != -1) return i;
    }

    return -1;
}

void routine(pid_t pid, int semaphores){
    int time_delivery = getDelayTime(TIME_DELIVERY);
    int time_return = getDelayTime(TIME_RETURN);

    int pizza;
    int table_spot = -1;
        
    struct sembuf operation = {TABLE_ID, -1, 0};

    semop(semaphores, &operation, 1);
    table_spot = getPizzaFromTable();

    if(table_spot != -1){
        pizza = pizza_house->table[table_spot];
        pizza_house->table[table_spot] = -1;
        pizza_house->table_amount--;

        printPickUp(pid, pizza, pizza_house->table_amount);
    }

    operation.sem_op = 1;
    semop(semaphores, &operation, 1);
    
    if(table_spot != -1){
        usleep(time_delivery);
        printDeliver(pid, pizza);
        usleep(time_return);
    }

}

// ---- Main program
int main(int argc, char **argv){
    pid_t pid = getpid();
    char message[100];
    bool running = true;

    createKey();

    int semaphores = openSemaphore();
    int shared_memory = openSharedMemory();
    if(semaphores < 0 || shared_memory < 0) return -1;

    pizza_house = shmat(shared_memory, NULL, 0);
    if(pizza_house == (void *) -1){
        error("COULDNT_OPEN_SHARED_MEMORY", "cannot reach data from shared memory");
        return RETURN_COULDNT_OPEN_SHARED_MEMORY;
    }

    sprintf(message, "deliverer with pid #%d starting work", pid);
    printInfo("START", message);

    signal(SIGINT, handleInterrupt);
    srand(time(NULL));

    while(running){
        routine(pid, semaphores);
    }
    return RETURN_SUCCESS;
}