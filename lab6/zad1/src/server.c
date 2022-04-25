#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "printutils.h"
#include "configs.h"
#include <errno.h>
#include <string.h>
#include <fcntl.h>

// ---- Server globals
bool running = true;
int * clients_queues;
int server_queue;

// server actions
void action_list_clients(int sender_id){
    char message[100];
    sprintf(message, "client id#%d requested a LIST operation", sender_id);
    printOper("LIST", message);
}

void action_send_all(int sender_id){

}

void action_send_one(int sender_id, int recipent_id, const char *content){

}

void action_stop(int sender_id){

}

void action_connect(int sender_id){

}

// requests
Request *get_request(){
    Request *request;

    if((msgrcv(server_queue, &request, REAL_REQUEST_SIZE, -6, 0)) == -1){
        error("COULDNT_RECEIVE_MESSAGE", "msgrcv() couldn't received message");
        printf("Errno: %s\n", strerror(errno));
        return NULL;
    }

    return request;
}

// initializing server
int create_server(){
    key_t key;
    int queue_id;

    if((key = ftok(QUEUE_PATH, PROJ_ID)) == (key_t) -1){
        error("COULDNT_CREATE_KEY", "ftok() couldn't generate new key");
        printf("Errno: %s\n", strerror(errno));
        return RETURN_COULDNT_CREATE_KEY;
    }

    if((queue_id = msgget(key, IPC_CREAT | 0666)) == -1){
        error("COULDNT_OPEN_QUEUE", "msgget() couldn't open queue (probably already exist)");
        return RETURN_COULDNT_OPEN_QUEUE;
    }

    return queue_id;
}

int initialize(){
    char message[100];

    printInfo("Server", "initializing server application");
    
    clients_queues = calloc(MAX_CLIENTS, sizeof(int));
    for(int i=0; i<MAX_CLIENTS; i++) clients_queues[i] = -1;
    
    if((server_queue = create_server()) < 0) return server_queue;

    sprintf(message, "succesfully created server with id#%d", server_queue);
    printInfo("Server", message);

    return RETURN_SUCCESS;
}

// ---- Main program
int main(int argc, char **argv){
    int init_result;
    char message[100];

    if((init_result = initialize()) != 0) return init_result;

    while(running){
        printf("1");
        Request *request = get_request();
                printf("2");

        if(request != NULL){
            printf("sender_id: %d | recipent_id: %d | type: %d | content: %d \n", request->sender_id, request->recipent_id, request->type, request->content);

            switch(request->type){
                case LIST:
                    action_list_clients(request->sender_id); break;

                case ALL:
                    action_send_all(request->sender_id); break;

                case ONE:
                    action_send_one(request->sender_id, request->recipent_id, request->content); break;

                case STOP:
                    action_stop(request->sender_id); break;

                case CONNECT:
                    action_connect(request->sender_id); break;

                default:
                    sprintf(message, "server received a message with incorrect type from client id#%d to cliebt id#%d", request->sender_id, request->recipent_id);
                    error("INCORRECT_MESSAGE_TYPE", message);
            }
        }

        sleep(2);
    }

    return RETURN_SUCCESS;
}