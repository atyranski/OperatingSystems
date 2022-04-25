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
int client_queue;
int server_queue;

void send_request(int destination_id, int recipent_id, Command type, const char *content){
    Request request;

    request.type = type;
    request.sender_id = client_queue;
    request.recipent_id = recipent_id;
    strcpy(request.content, content);


    if(msgsnd(destination_id, &request, REAL_REQUEST_SIZE, 0) == -1){
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

    printInfo("Client", "initializing client application");
    
    if((client_queue = msgget(IPC_PRIVATE, 0666)) == NULL) {
        error("COULDNT_CREATE_CLIENT", "msgget() couldn't create queue for new client");
        printf("Errno: %s\n", strerror(errno));
        return NULL;
    }

    if((server_queue = connect_server()) < 0) return server_queue;

    sprintf(message, "succesfully connected to server with id#%d", client_queue);
    printInfo("Client", message);


    sprintf(id_to_char, "%d", client_queue);
    send_request(server_queue, -1, CONNECT, id_to_char);


    return RETURN_SUCCESS;
}

// ---- Main program
int main(int argc, char **argv){
    int init_result;

    if((init_result = initialize()) != 0) return init_result;

    return RETURN_SUCCESS;
}