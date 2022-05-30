#include "configs.h"

int port;
char *socket_path;

int descriptor;
Client **clients;
Game **games;
Server *server;
int waiting_for_enemy = -1;
struct epoll_event events[MAX_CLIENTS_REGISTERED];


// Utilities
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


// Server Initialization
int createLocalServer(char *socket_path){
    int descriptor;
    
    if ((descriptor = socket(AF_UNIX, "local", 0)) == -1) {
        error("ERROR", "local socket descriptor");
        exit(1);
    }

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, socket_path);

    if(bind(descriptor, (struct sockaddr*) &address, sizeof(address)) != 0){
        error("ERROR", "local server binding problem");
        exit(1);
    }

    if(remove(socket_path) == -1 && errno != ENOENT){
        error("ERROR", "removing existing local socket problem");
        exit(1);
    }

    return descriptor;
}

int createOnlineServer(int port){
    int descriptor;
    
    if ((descriptor = socket(AF_UNIX, "local", 0)) == -1) {
        error("ERROR", "local socket descriptor");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if(bind(descriptor, (struct sockaddr*) &address, sizeof(address)) != 0){
        error("ERROR", "online server binding problem");
        exit(1);
    }

    return descriptor;
}

Server *createServer(int port, char *socket_path){
    int local_descriptor = createLocalServer(socket_path);
    int online_descriptor = createOnlineServer(port);
    int epoll;

    listen(local_descriptor, MAX_CLIENTS_REGISTERED);
    listen(online_descriptor, MAX_CLIENTS_REGISTERED);

    epoll = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;

    event.data.fd = descriptor;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, local_descriptor, &event) < 0){
        error("ERROR", "adding local socket to event poll");
        exit(1);
    }

    event.data.fd = online_descriptor;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, online_descriptor, &event) < 0){
        error("ERROR", "adding online socket to event poll");
        exit(1);
    }

    Server *server = calloc(1, sizeof(Server));
    server->local = local_descriptor;
    server->online = online_descriptor;
    server->epoll = epoll;

    return server;
}


// Server Actions
void action_check(char request[MAX_REQUEST_SIZE], int clientID){
    
}

void shutdownServer(Server *server){
    printInfo("SERVER", "shutdown");
    close(server->local);
    close(server->online);
    close(server->epoll);
    free(server);
}

bool registerClient(Server *server, struct epoll_event *event){
    struct sockaddr_in address;
    struct epoll_event accept_event;

    if(event->data.fd != server->online && event->data.fd != server->local) return false;

    int client = accept(server->local, (struct sockaddr*) &address, sizeof(address));

    if(client < 0){
        error("REGISTER", "couldn't register client");
        return false;
    }

    accept_event.events = EPOLLIN;
    accept_event.data.fd = client;

    if(epoll_ctl(server->epoll, EPOLL_CTL_ADD, client, &accept_event) < 0){
        error("ADD ERROR", "couldn't add client to event poll");
        return false;
    }

    return true;
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

    server = createServer(port, socket_path);

    while (true){
        int events_amount = epoll_wait(server->epoll, events, MAX_CLIENTS_REGISTERED, -1);

        for(int i=0; i<events_amount; i++){

            if(registerClient(server, &events[i])){
                char message[100];

                sprintf(message, "client with id#%d successfully registered");
                printInfo("SERVER", message);

            } else{
                int clientID = events[i].data.fd;

                char request[MAX_REQUEST_SIZE];
                int length = read(clientID, request, MAX_CLIENTS_REGISTERED-1);
                
                if(length < 0){
                    error("REQUEST_ERROR", "couldn't read from server socket");
                    continue;
                }

                RequestType request_type;
                sscanf(request, "%d", &request_type);

                switch (request_type){
                    case REQ_MOVE:
                        printInfo("MOVE", "move request");
                        action_move(request, clientID);
                        break;
                    
                    case REQ_QUIT:
                        printInfo("MOVE", "move request");
                        action_quit(request, clientID);
                        break;
                    
                    case REQ_CHECK:
                        printInfo("MOVE", "move request");
                        action_check(request, clientID);
                        break;

                    default:
                        error("INCORRECT_TYPE", "server received and unknown type of request");
                        break;
                }
            }


        }
    }

    shutdownServer(server);
    

    // // Creating socket
    // if((descriptor = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    //     error("SOCKET_ERROR", "cannot make a socket");
    //     exit(1);
    // }

    // struct sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htobe16(33333);
    // addr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);

    // if(bind(descriptor, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1){
    //     error("BIND_ERROR", "cannot bind to addres");
    //     exit(1);
    // }

    // char nickname[CLIENT_NICK_LENGTH];
    // Client *newClient = calloc(1, sizeof(Client));
    // clients = calloc(MAX_CLIENTS_REGISTERED, sizeof(Client *));
    // games = calloc(MAX_CLIENTS_REGISTERED/2, sizeof(Game *));


    // while(true){
    //     if(read(descriptor, &nickname, CLIENT_NICK_LENGTH * sizeof(char)) == -1){
    //         error("READ_ERROR", "cannot read from socket");
    //         exit(1);
    //     }
    //     int clientID;
    //     int gameID;
    //     if((clientID = getFreeRegisterSpot(clients)) != -1) newClient->id = clientID;
    //     else {
    //         // if server full
    //     }

    //     strcpy(&newClient->nick, &nickname);

    //     if(waiting_for_enemy == -1){
    //         newClient->symbol = 'X';
    //         gameID = getFreeGame(games);

    //         Game *game = calloc(1, sizeof(Game));
    //         game->first = newClient;

    //         games[gameID] = game;

    //         waiting_for_enemy = gameID;
    //     } else {
    //         newClient->symbol = 'O';
    //         games[waiting_for_enemy]->second = newClient;
    //         waiting_for_enemy = -1;
    //     }

    //     printf("Client registered: id: %d\t| nickname: %s\t| symbol: %c\t| game: %d\n",
    //         newClient->id,
    //         newClient->nick,
    //         newClient->symbol,
    //         gameID);
    // }

    return RETURN_SUCCESS;
}