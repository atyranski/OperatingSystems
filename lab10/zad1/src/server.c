#include "configs.h"

int port;
char *socket_path;

int descriptor;
Client **clients;
Game **games;
int waiting_for_enemy = -1;

int getFreeRegisterSpot(Client **clients){
    for(int i=0; i<MAX_CLIENTS_REGISTERED; i++){
        if(clients[i] == NULL) return i;
    }
    return -1;
}

int getFreeGame(Game **games){
    for(int i=0; i<MAX_CLIENTS_REGISTERED/2; i++){
        if(games[i] == NULL) return i;
    }
    return -1;
}

// ---- Main program
int main(int argc, char **argv){

    // Validation of arguments
    if(argc != 3){
        error("INCORRECT_ARGUMENTS", "./bin/main [port] [socket path]");
        exit(1);
    }

    port = atoi(argv[1]);
    socket_path = argv[2];

    // Creating socket
    if((descriptor = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        error("SOCKET_ERROR", "cannot make a socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(33333);
    addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

    if(bind(descriptor, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1){
        error("BIND_ERROR", "cannot bind to addres");
        exit(1);
    }

    char nickname[CLIENT_NICK_LENGTH];
    Client *newClient = calloc(1, sizeof(Client));
    clients = calloc(MAX_CLIENTS_REGISTERED, sizeof(Client *));
    games = calloc(MAX_CLIENTS_REGISTERED/2, sizeof(Game *));


    while(true){
        if(read(descriptor, &nickname, CLIENT_NICK_LENGTH * sizeof(char)) == -1){
            error("READ_ERROR", "cannot read from socket");
            exit(1);
        }
        int free_register_spot;
        int free_game;
        if((free_register_spot = getFreeRegisterSpot(clients)) != -1) newClient->id = free_register_spot;
        else {
            // if server full
        }

        strcpy(&newClient->nick, &nickname);

        if(waiting_for_enemy == -1){
            newClient->symbol = 'X';
            free_game = getFreeGame(games);

            Game *game = calloc(1, sizeof(Game));
            game->first = newClient;

            games[free_game] = game;

            waiting_for_enemy = free_game;
        } else {
            newClient->symbol = 'O';
            games[waiting_for_enemy]->second = newClient;
            waiting_for_enemy = -1;
        }

        printf("Client registered: %d\t| %s\t| %c\n",
            newClient->id,
            newClient->nick,
            newClient->symbol);
    }

    shutdown(descriptor, SHUT_RDWR);
    close(descriptor);

    return RETURN_SUCCESS;
}