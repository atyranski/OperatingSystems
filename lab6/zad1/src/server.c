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
int *clients_queues;
int client_next = 0;
int server_queue;


// requests
Request get_request(int expected_type){
    Request request;

    if((msgrcv(server_queue, &request, 128, expected_type, 0)) == -1){
        error("COULDNT_RECEIVE_MESSAGE", "msgrcv() couldn't received message");
        printf("Errno: %s\n", strerror(errno));
    }

    return request;
}

bool send_request(int recipent_id, Command type, const char *content){
    Request request;

    request.type = type;
    request.sender_id = server_queue;
    request.recipent_id = recipent_id;
    strcpy(request.content, content);


    if(msgsnd(recipent_id, &request, 128, 0) == -1){
        error("COULDNT_SEND_MESSAGE", "msgsnd() couldn't send message");
        printf("Errno: %s\n", strerror(errno));
        return false;
    }

    return true;
}

// server actions
void action_list_clients(int sender_id){
    char message[100];
    sprintf(message, "client id#%d (queue: #%d) requested a LIST operation", client_next, sender_id);
    printOper("LIST", message);
}

void action_send_all(int sender_id){

}

void action_send_one(int sender_id, int recipent_id, const char *content){

}

void action_stop(int sender_id){

}

void action_connect(int sender_id){
    char message[100];
    char id_to_char[4];

    sprintf(message, "client with queue_id #%d trying to connect to the server", sender_id);
    printOper("CONNECT", message);

    if(client_next >= MAX_CLIENTS){
        error("SERVER_IS_FULL", "server is full and cannot connect another client");
        send_request(sender_id, CONNECT, "full");
        return;
    }

    clients_queues[client_next] = sender_id;
    
    sprintf(id_to_char, "%d", client_next);
    if(!send_request(sender_id, CONNECT, id_to_char)) return;

    sprintf(message, "client id#%d (queue: #%d) connected to the server", client_next, sender_id);
    printInfo("CONNECT", message);

    client_next++;
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

    printInfo("INITIALIZATION", "initializing server application");
    
    clients_queues = calloc(MAX_CLIENTS, sizeof(int));
    for(int i=0; i<MAX_CLIENTS; i++) clients_queues[i] = -1;
    
    if((server_queue = create_server()) < 0) return server_queue;

    sprintf(message, "succesfully created server with id#%d", server_queue);
    printInfo("INITIALIZATION", message);

    return RETURN_SUCCESS;
}

// ---- Main program
int main(int argc, char **argv){
    int init_result;
    char message[100];

    if((init_result = initialize()) != 0) return init_result;

    while(running){
        Request request = get_request(EVERY_REQUEST_TYPE);

        if(&(request) != NULL){
            printf("\nsender_id: %d | recipent_id: %d | type: %d | content: %d \n", request.sender_id, request.recipent_id, request.type, request.content);

            switch(request.type){
                case LIST:
                    action_list_clients(request.sender_id); break;

                case ALL:
                    action_send_all(request.sender_id); break;

                case ONE:
                    action_send_one(request.sender_id, request.recipent_id, request.content); break;

                case STOP:
                    action_stop(request.sender_id); break;

                case CONNECT:
                    action_connect(request.sender_id); break;

                default:
                    sprintf(message, "server received a message with incorrect type from client id#%d to cliebt id#%d", request.sender_id, request.recipent_id);
                    error("INCORRECT_MESSAGE_TYPE", message);
            }
        }

        sleep(2);
    }

    return RETURN_SUCCESS;
}