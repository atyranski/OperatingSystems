#include "configs.h"

Client *client;

// ---- Main program
int main(int argc, char **argv){

    int descriptor;

    if((descriptor = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        error("SOCKET_ERROR", "cannot make a socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(33333);
    addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

    if(connect(descriptor, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1){
        error("CONNECT_ERROR", "cannot connect to addres");
        exit(1);
    }

    client = calloc(1, sizeof(Client));

    printf("Enter nickname: ");
    scanf("%s", &(client->nick));

    if(write(descriptor, &client->nick, sizeof(Client)) == -1){
        error("WRITE_ERROR", "cannot write to socket");
        exit(1);
    }

    shutdown(descriptor, SHUT_RDWR);
    close(descriptor);

    return RETURN_SUCCESS;
}