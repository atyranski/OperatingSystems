#include "configs.h"

const char *nickname;
ConnectionType connection_type;
const char *socket_path;
int port;
const char *server_address;
int connection_descriptor;
struct sockaddr_un local_address;
struct sockaddr_in online_address;

// Utility
ConnectionType getConnectionType(const char *type){
    if(strcmp("local", type) == 0) return CONN_LOCAL;
    if(strcmp("online", type) == 0) return CONN_ONLINE;
    
    return CONN_NONE;
}


// Action handlers
void handleQuit(void){
    printInfo("DISCONNECTING", "disconecting from the server");
    write(connection_descriptor, "QUIT", strlen("QUIT") + 1);
}


// Server connection
int localConnection(){
    int descriptor;

    if((descriptor = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){
        error("SOCKET_CONNECTION", "couldn't create local socket");
        exit(1);
    }

    local_address.sun_family = AF_LOCAL;
    strcpy(local_address.sun_path, server_address);

    if(connect(descriptor, (struct sockaddr*) &local_address, sizeof(local_address)) == -1){
        error("CONNECT_FAILED", "couldn't connect to local server");
        exit(1);
    }

    return descriptor;
}

int onlineConnection(){
    int descriptor;

    if((descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        error("SOCKET_CONNECTION", "couldn't create online socket");
        exit(1);
    }

    struct in_addr address;
    if(inet_pton(AF_INET, server_address, &address) == 0){
        error("INVALID_ADDRESS", "couldn't connect to provided network address");
        exit(1);
    }

    online_address.sin_port = htons(port);
    online_address.sin_addr = address;
    online_address.sin_family = AF_INET;

    if(connect(descriptor, (struct sockaddr*) &online_address, sizeof(online_address)) == -1){
        error("CONNECT_FAILED", "couldn't connect to online server");
        exit(1);
    }

    return descriptor;
}

void connectToServer(ConnectionType type){
    if(type == CONN_LOCAL) connection_descriptor = localConnection();
    if(type == CONN_ONLINE) connection_descriptor = onlineConnection();
}


// Routine
void run(){
    char response[RESPONSE_SIZE];
    int response_length;

    write(connection_descriptor, nickname, strlen(nickname) + 1);
    
    response_length = read(connection_descriptor, response, RESPONSE_SIZE);
    response[response_length] = 0;

    if(strcmp(response, "NICKNAME_TAKEN") == 0){
        error("REGISTER_DECLINED", "provided nickname is already taken, try another one");
        exit(1);
    }

    if(strcmp(response, "SERVER_FULL") == 0){
        error("REGISTER_DECLINED", "this server reached it's max of registered clients, try next time");
        exit(1);
    }

    if(strcmp(response, "ACCEPTED") == 0){
        printInfo("CONNECTED", "you're successfully registered to the server.");
    } else {
        error("UNKNOWN_RESPONSE", "received an unknown response from the server, sorry");
        exit(1);
    }

    while(true){
        strcpy(response, "");

        response_length = read(connection_descriptor, response, RESPONSE_SIZE);

        if(response_length < 1) {
            printOper("DISCONNECTING", "server closed connections");
            exit(1);
        }

        if(strcmp(response, "CHECK") == 0){
            write(connection_descriptor, "ALIVE", strlen("ALIVE") + 1);
            continue;
        }

        

    }
}

// ---- Main program
int main(int argc, char **argv){
    atexit(handleQuit);

    // Validation of arguments
    if(argc != 4 && argc != 5){
        error("INCORRECT_ARGUMENTS", "./bin/main <nickname(16)> ('local' | 'online') (<unix path> |<online address> <port>)");
        exit(1);
    }

    nickname = argv[1];
    connection_type = getConnectionType(argv[2]);

    if(connection_type == CONN_NONE){
        error("INCORRECT_CONNECTION_TYPE", "provide 'local' or 'online' as a connection type");
        exit(1);
    }

    if(connection_type == CONN_ONLINE){
        server_address = argv[3];
        port = atoi(argv[4]);

    } else {
        socket_path = argv[3];
    }

    printf("nickname: %s\t| connection: %d\t| address: %s:%d\t| sock_path: %d\n",
        nickname,
        connection_type,
        server_address, port,
        socket_path);

    connectToServer(connection_type);
    run();

    return RETURN_SUCCESS;
}