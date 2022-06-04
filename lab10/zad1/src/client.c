#include "configs.h"

struct sockaddr_un socket_local;
struct sockaddr_in socket_online;
int connection_descriptor;

char response[RESPONSE_SIZE];

bool isPlaying = false;
char *nickname;
char *server_address;
int port;
char symbol;
ConnectionType connection_type;

ConnectionType getConnectionType(const char *type){
    if(strcmp("local", type) == 0) return LOCAL;
    if(strcmp("online", type) == 0) return ONLINE;
    
    return NONE;
}

void sendGameOver(void){
    if (isPlaying) write(connection_descriptor, "DISCONNECTION", strlen("DISCONNECTION") + 1);
}

void drawTable(const char *table){
    printf("\n\t%c\t|\t%c\t|\t%c\n", table[0], table[1], table[2]);
    printf("_________________________________________________\n\n");
    printf("\t%c\t|\t%c\t|\t%c\n", table[3], table[4], table[5]);
    printf("_________________________________________________\n\n");
    printf("\t%c\t|\t%c\t|\t%c\n\n", table[6], table[7], table[8]);
}

GameResult checkVictoryConditions(const char *table){
    GameResult result = NOT_SETTLED;

    for (int i = 0; i < 3; i++){
        if (table[i * 3] != ' ' && table[i * 3] == table[i * 3 + 1] && table[i * 3 + 1] == table[i * 3 + 2]){
            if (table[i * 3] == 'O') result = O_WIN;
            else result = X_WIN;
            break;
        }

        if (table[i] != ' ' && table[i] == table[i + 3] && table[i + 3] == table[i + 6]){
            if (table[i] == 'O') result = O_WIN;
            else result = X_WIN;
            break;
        }
    }

    if (table[0] == table[4] && table[4] == table[8]){
        if (table[0] == 'O') result = O_WIN;
        else if (table[0] == 'X') result = X_WIN;
    }

    if (table[2] == table[4] && table[4] == table[6]){
        if (table[4] == 'O') result = O_WIN;
        else if (table[0] == 'X') result = X_WIN;
    }

    if (result == NOT_SETTLED){
        result = DRAW;

        for (int i = 0; i < 9; i++){
            if (table[i] != 'X' && table[i] != 'O') {
                result = NOT_SETTLED;
                break;
            }
        }
    }

    return result;
}

void getMove(char *table){
    while (true){
        int area;
        print("Choose area (1-9): ");
        scanf("%d", &area);

        if (area < 1 || area > 9) {
            error("WRONG_AREA", "you've picked area outside of range");
            continue;
        }

        if (table[area - 1] == 'X' || table[area - 1] == 'O') {
            error("TAKEN_AREA", "you've picked area already taken");
            continue;
        }
        
        table[area - 1] = symbol;
        break;
    }
}

bool isEnd(char *table) {
    GameResult result = checkVictoryConditions(table);

    switch(result){
        case NOT_SETTLED:
            return false;
        case O_WIN:
            if(symbol == 'O') printInfo("VICTORY", "you've won");
            else error("DEFEAT", "you've lose");
            return true;
        case X_WIN:
            if(symbol == 'X') printInfo("VICTORY", "you've won");
            else error("DEFEAT", "you've lose");
            return true;
        case DRAW:
            printOper("DRAW", "none of you won");
            return true;
        default:
            error("UKNOWN_RESULT", "program received uknown result enum value");
            return false;
    }
}

void run(){
    write(connection_descriptor, nickname, strlen(nickname) + 1);
    int response_length = read(connection_descriptor, response, RESPONSE_SIZE);
    response[response_length] = 0;

    if (strcmp(response, "NICKNAME_TAKEN") == 0){
        error("NICKNAME_TAKEN", "nickname that you've provided is already taken by other client");
        exit(1);
    }

    if (strcmp(response, "SERVER_FULL") == 0){
        error("SERVER_FULL", "this server reached it's max of registered clients, try next time");
        exit(1);
    }

    if(strcmp(response, "ACCEPTED") == 0) printInfo("CONNECTED", "you're successfully registered to the server.");
    else {
        error("UNKNOWN_RESPONSE", "received an unknown response from the server, sorry");
        exit(1);
    }

    while (true){
        response_length = read(connection_descriptor, response, RESPONSE_SIZE);

        if (response_length < 1){
            error("DISCONNECTING", "server has closed the connection");
            exit(1);
        }

        response[response_length] = 0;

        if (strcmp(response, "CHECK") == 0) write(connection_descriptor, "ALIVE", strlen("ALIVE") + 1);
        else {
            isPlaying = true;

            char table[9] = "123456789";
            symbol = response[0];
            bool isFirst = symbol == 'O';
            printf("%s\n", response);

            while (true) {
                if (!isFirst){
                    printOper("ENEMY_MOVE", "other player has made a move:");
                    drawTable(table);

                    if (isEnd(table)){
                        write(connection_descriptor, "GAME_END", strlen("GAME_END") + 1);
                        isPlaying = false;
                        break;
                    }

                    getMove(table);
                    write(connection_descriptor, table, 9);

                    drawTable(table);

                    if (isEnd(table)){
                        write(connection_descriptor, "GAME_END", strlen("GAME_END") + 1);
                        isPlaying = false;
                        break;
                    }

                } else {
                    isFirst = false;
                    printOper("WAIT", "other player is making his/her move");
                }

                printOper("WAIT", "waiting for other player response");
                response_length = read(connection_descriptor, response, RESPONSE_SIZE);

                if (response_length < 1) {
                    error("DISCONNECTING", "server has closed the connection");
                    exit(1);
                }

                if (strcmp(response, "DISCONNECTION") == 0) {
                    printInfo("DISCONNECTION", "enemy has quit from the server");
                    isPlaying = false;
                    break;
                }

                strncpy(table, response, 9);
            }
        }
    }
}

void onlineConnection(){
    struct in_addr online_address;

    if ((connection_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        error("SOCKET_ERROR", "couldn't create online socket");
        exit(1);
    }

    if (inet_pton(AF_INET, server_address, &online_address) == 0){
        error("NETWORK_ADDRESS", "provided network address is invalid");
        exit(1);
    }

    socket_online.sin_port = htons(port);
    socket_online.sin_addr = online_address;
    socket_online.sin_family = AF_INET;

    if (connect(connection_descriptor, (struct sockaddr *) &socket_online, sizeof(socket_online)) == -1){
        error("ONLINE_CONNECTION", "couldn't connect to online socket");
        exit(1);
    }
}

void localConnection(){
    if ((connection_descriptor = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){
        error("SOCKET_ERROR", "couldn't create local socket");
        exit(1);
    }

    socket_local.sun_family = AF_LOCAL;
     strcpy(socket_local.sun_path, server_address);

    if (connect(connection_descriptor, (struct sockaddr *) &socket_local, sizeof(socket_local)) == -1){
        error("LOCAL_CONNECTION", "couldn't connect to local socket");
        exit(1);
    }
}

void connectToServer(){
    if (connection_type == ONLINE) onlineConnection();
    if (connection_type == LOCAL) localConnection();
}

// ---- Main program
int main(int argc, char **argv){
    atexit(sendGameOver);

    // Validation of arguments
    if(argc != 4 && argc != 5){
        error("INCORRECT_ARGUMENTS", "./bin/main <nickname(16)> ('local' | 'online') (<unix path> |<online address> <port>)");
        exit(1);
    }

    nickname = argv[1];
    if (strlen(nickname) > CLIENT_NICK_LENGTH) {
        error("NICKNAME_TOO_LONG", "your nickname is too long, max = 16");
        exit(1);
    }

    connection_type = getConnectionType(argv[2]);

    if(connection_type == NONE){
        error("INCORRECT_CONNECTION_TYPE", "provide 'local' or 'online' as a connection type");
        exit(1);
    }

    server_address = argv[3];

    if(connection_type == ONLINE) port = atoi(argv[4]);

    connectToServer(argv);
    run();

    return RETURN_SUCCESS;
}