#include "configs.h"

Client *client_list;
pthread_mutex_t mutex_clients = PTHREAD_MUTEX_INITIALIZER;

struct sockaddr_in socket_online;
struct sockaddr_un socket_local;

int descriptor_local;
int descriptor_online;
int port;
char *server_address;
int clients_amount;

void *handleRegister(void *args){
    printInfo("SERVER_START", "listening descriptors");
    listen(descriptor_local, 12);
    listen(descriptor_online, 12);

    while (true){
        char nickname[CLIENT_NICK_LENGTH];
        int client_descriptor = -1;
        ConnectionType connection_type = NONE;

        if ((client_descriptor = accept(descriptor_online, NULL, NULL)) != -1) {
            printOper("REGISTER", "connection type: local");
            connection_type = ONLINE;

        } else if ((client_descriptor = accept(descriptor_local, NULL, NULL)) != -1) {
            printOper("REGISTER", "connection type: online");
            connection_type = LOCAL;
        }

        if (connection_type != NONE) {
            int nickname_length = read(client_descriptor, nickname, CLIENT_NICK_LENGTH);
            nickname[nickname_length] = 0;

            pthread_mutex_lock(&mutex_clients);

            if (findClient(client_list, nickname) != NULL) {
                send(client_descriptor, "NICKNAME_TAKEN", strlen("NICKNAME_TAKEN") + 1, MSG_DONTWAIT);
                shutdown(client_descriptor, SHUT_RDWR);
                close(client_descriptor);

            } else if(clients_amount == MAX_CLIENTS_REGISTERED){
                error("SERVER_FULL", "cannot register any other client");
                send(client_descriptor, "SERVER_FULL", strlen("SERVER_FULL") + 1, MSG_DONTWAIT);
                shutdown(client_descriptor, SHUT_RDWR);
                close(client_descriptor);

            } else {
                char message[100];
                struct sockaddr client_address;

                addClient(&client_list, client_descriptor, connection_type, nickname, client_address);
                send(client_descriptor, "ACCEPTED", strlen("ACCEPTED") + 1, MSG_DONTWAIT);

                sprintf(message, 'successfully registered client with nickname: %s', nickname);
                printInfo("REGISTER", message);
            }

            clients_amount++;
            pthread_mutex_unlock(&mutex_clients);
        }
    }
    return NULL;
}

void pairClients(Client *client){
    char message[100];
    char response[RESPONSE_SIZE];
    Client *first;
    Client *second;

    pthread_mutex_lock(&mutex_clients);

    client = client_list;
    while (client != NULL) {
        if (client->enemy == NULL){
            if (first == NULL) first = client;
            else {
                second = client;
                break;
            }
        }
        client = client->next;
    }

    if (first != NULL && second != NULL){
        first->enemy = second->nickname;
        second->enemy = first->nickname;

        sprinft(response, "X: Starting game with %s\n", first->nickname);
        send(second->descriptor, response, strlen(response) + 1, MSG_DONTWAIT);

        sprinft(response, "O: Starting game with %s\n", second->nickname);
        send(first->descriptor, response, strlen(response) + 1, MSG_DONTWAIT);

        sprinft(message, "starting game between: %s vs %s", first->nickname, second->nickname);
        printInfo("GAME_START", message);
    }

    pthread_mutex_unlock(&mutex_clients);
}

int aliveCheck(int check_prev, Client *client){
    int check_now = getTime();
    char response[RESPONSE_SIZE];

    if (check_now - check_prev > CHECK_RESPONSE_INTERVAL){
        pthread_mutex_lock(&mutex_clients);
        client = client_list;

        while (client != NULL){
            if (client->enemy == NULL){
                int request_length = send(client->descriptor, "CHECK", strlen("CHECK") + 1, MSG_DONTWAIT);

                if (request_length == -1) client->check_attempts = CHECK_ATTEMPTS + 1;

                client->check_attempts++;

                if (recv(client->descriptor, response, RESPONSE_SIZE, MSG_DONTWAIT) > 0) {
                    response[strlen("ALIVE")] = 0;

                    if (strcmp(response, "ALIVE") == 0) client->check_attempts = 0;

                } else if (client->check_attempts > CHECK_ATTEMPTS) {
                    char message[100];

                    sprintf(message, "client with nickname %s is not responding - quiting connection", client->nickname);
                    printInfo("DISCONNECTING", message);
                    shutdown(client->descriptor, SHUT_RDWR);
                    close(client->descriptor);
                    removeClient(&client_list, client->nickname);
                    break;
                }
            }

            client = client->next;
        }

        pthead_mutex_unlock(&mutex_clients);
    }

    return getTime();
}

void *handleGames(void *args) {
    char response[RESPONSE_SIZE];
    int check_prev = getTime();
    int response_length;

    while (true) {
        pthread_mutex_lock(&mutex_clients);
        Client *client = client_list;

        while (client != NULL) {
            if (client->enemy != NULL) {
                response_length = recv(client->descriptor, response, RESPONSE_SIZE, MSG_DONTWAIT);

                if (response_length > 0) {
                    response[response_length] = 0;
                    Client *enemy = findClient(client_list, client->enemy);

                    if (strcmp(response, "GAME_END") == 0) client->enemy = NULL;
                    else if (strcmp(response, "DISCONNECTION") == 0) {
                        enemy->enemy = NULL;
                        shutdown(client->descriptor, SHUT_RDWR);
                        close(client->descriptor);
                        removeClient(&client_list, client->nickname);

                        send(enemy->descriptor, response, response_length, MSG_DONTWAIT);
                        break;

                    } else if (strcmp(response, "CHECK") == 0) client->check_attempts = 0;
                    else {
                        char message[100];

                        sprintf(message, "passing move from client %s to client %s", client->nickname, client->enemy);
                        send(enemy->descriptor, response, response_length, MSG_DONTWAIT);
                    }
                }
            }
            client = client->next;
        }
        pthread_mutex_unlock(&mutex_clients);

        pairClients(client);
        
        check_prev = aliveCheck(check_prev, client);
    }
    return NULL;
}

void handleQuit(){
    close(descriptor_online);
    close(descriptor_local);
}

int main(int argc, char **argv) {
    atexit(handleQuit);

     // Validation of arguments
    if(argc != 3){
        error("INCORRECT_ARGUMENTS", "./bin/main [-port-] [-socket path-]");
        exit(1);
    }

    client_list = NULL;

    if ((port = atoi(argv[1])) == 0){
        error("INCORRECT_CONNECTION_TYPE", "provide 'local' or 'online' as a connection type");
        exit(1);
    }

    server_address = argv[2];
    clients_amount = 0;

    if ((descriptor_online = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1){
        printf("Unable to create socket at port %d\n", port);
        return 0;
    }

    if ((descriptor_local = socket(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        printf("Unable to create unix socket\n");
        return 0;
    }

    unlink(server_address);
    strcpy(socket_local.sun_path, server_address);
    socket_local.sun_family = AF_LOCAL;

    if (bind(descriptor_local, (struct sockaddr *) &socket_local, sizeof(socket_local)) == -1){
        puts("Unable to bind local socket");
        return 0;
    }

    socket_online.sin_family = AF_INET;
    socket_online.sin_port = htons(port);
    socket_online.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(descriptor_online, (struct sockaddr *) &socket_online, sizeof(socket_online)) == -1){
        puts("Unable to bind web socket");
        return 0;
    }

    pthread_t listeningThread;
    pthread_t handlingThread;


    pthread_create(&listeningThread, NULL, handleRegister, NULL);

    pthread_create(&handlingThread, NULL, handleGames, NULL);

    pthread_join(listeningThread, NULL);
    pthread_join(handlingThread, NULL);

    return 0;
}