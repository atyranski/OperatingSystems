#include "configs.h"

Client *client;
ConnectionType connection;
int port;
char *socket_path;

ConnectionType getConnectionType(const char *type){
    if(strcmp("local", type) == 0) return CONN_LOCAL;
    if(strcmp("online", type) == 0) return CONN_ONLINE;
    
    return CONN_NONE;
}

// ---- Main program
int main(int argc, char **argv){

    // Validation of arguments
    if(argc != 3){
        error("INCORRECT_ARGUMENTS", "./bin/main <nickname(16)> ('local' | 'online') (<socket path> | <port>)");
        exit(1);
    }

    strcpy(client->nick, argv[1]);
    connection = getConnectionType(argv[2]);

    if(connection == CONN_NONE){
        error("INCORRECT_CONNECTION_TYPE", "provide 'local' or 'online' as a connection type");
        exit(1);
    }
    
    client->connection = connection;

    if(connection == CONN_LOCAL){
        strcpy(socket_path, argv[3]);
    } else {
        port = atoi(argv[3]);
    }

    return RETURN_SUCCESS;
}