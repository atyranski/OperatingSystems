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

// ---- Client globals
bool running = true;
int client_queue;
int server_queue;
int client_id;

// client actions
void action_connect(const char *content){
    if(strcmp(content, "full") == 0){
        error("SERVER_IS_FULL", "server is full and cannot connect another client");
        running = false;
        return;
    }

    client_id = atoi(content);

    char message[100];
    sprintf(message, "successfully connected to the server and received id#%d", client_id);
    printInfo("CONNECT", message);
}

// requests
Request get_request(int expected_type){
    Request request;

    if((msgrcv(server_queue, &request, MAX_REQUEST_SIZE, expected_type, 0)) == -1){
        error("COULDNT_RECEIVE_MESSAGE", "msgrcv() couldn't received message");
        printf("Errno: %s\n", strerror(errno));
    }

    return request;
}

void send_request(int destination_id, int recipent_id, Command type, const char *content){
    Request request;

    request.type = type;
    request.sender_id = client_queue;
    request.recipent_id = recipent_id;
    strcpy(request.content, content);


    if(msgsnd(destination_id, &request, MAX_REQUEST_SIZE, 0) == -1){
        error("COULDNT_SEND_MESSAGE", "msgsnd() couldn't send message");
        printf("Errno: %s\n", strerror(errno));
    }
}

int connect_server(){
    key_t key;
    int queue_id;

    if((key = ftok(QUEUE_PATH, PROJ_ID)) == (key_t) -1){
        error("COULDNT_CREATE_KEY", "ftok() couldn't generate new key");
        printf("Errno: %s\n", strerror(errno));
        return RETURN_COULDNT_CREATE_KEY;
    }

    if((queue_id = msgget(key, IPC_EXCL | 0666)) == -1){
        error("COULDNT_OPEN_QUEUE", "msgget() couldn't open server queue");
        return RETURN_COULDNT_OPEN_QUEUE;
    }

    return queue_id;
}

int initialize(){
    char message[100];
    char id_to_char[4];

    printInfo("INITIALIZATION", "initializing client application");
    
    if((client_queue = msgget(IPC_PRIVATE, 0666)) == NULL) {
        error("COULDNT_CREATE_CLIENT", "msgget() couldn't create queue for new client");
        printf("Errno: %s\n", strerror(errno));
        return NULL;
    }

    if((server_queue = connect_server()) < 0) return server_queue;

    sprintf(message, "reached server and created queue with id#%d", client_queue);
    printOper("CONNECT", message);

    send_request(server_queue, -1, CONNECT, "");

    Request response;
    if(msgrcv(client_queue, &response, MAX_REQUEST_SIZE, CONNECT, 0) == -1){
        error("COULDNT_RECEIVE_MESSAGE", "msgrcv() couldn't received message");
        printf("Errno: %s\n", strerror(errno));
        return RETURN_COULDNT_RECEIVE_MESSAGE;
    }

    action_connect(response.content);

    return RETURN_SUCCESS;
}

// ---- Main program
int main(int argc, char **argv){
    int init_result;
    char message[100];

    if((init_result = initialize()) != 0) return init_result;

    // while(running){
    //         printf("%d\n",2);
    //     Request request = get_request(EVERY_REQUEST_TYPE);
    //         printf("%d\n",3);

    //     if(&(request) != NULL){
    //         printf("sender_id: %d | recipent_id: %d | type: %d | content: %d \n", request.sender_id, request.recipent_id, request.type, request.content);

    //         switch(request.type){
    //             case CONNECT:
    //                 action_connect(request.content); break;

    //             default:
    //                 sprintf(message, "server received a message with incorrect type from client id#%d to cliebt id#%d", request.sender_id, request.recipent_id);
    //                 error("INCORRECT_MESSAGE_TYPE", message);
    //         }
    //     }

    //     sleep(2);
    // }

    return RETURN_SUCCESS;
}