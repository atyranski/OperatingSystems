# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
SERVER_PATH = src/server.c
CLIENT_PATH = src/client.c

# --- Iput values
PORT = 50000
SOCKET_PATH = socket
CLIENT_NICK = Bolek
CONN_TYPE = online
ADDRESS = 127.0.0.1

recompile: clean compile_lib compile_server compile_client server_run

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH) -o libs/$(LIB_NAME).o
	ar rcs libs/lib$(LIB_NAME).a libs/$(LIB_NAME).o

compile_server:
	$(CC) -w $(SERVER_PATH) $(OPTIMIZATION) -o bin/server -L./libs -l$(LIB_NAME) -lpthread

compile_client:
	$(CC) -w $(CLIENT_PATH) $(OPTIMIZATION) -o bin/client -L./libs -l$(LIB_NAME) -lpthread

clean:
	clear
	rm -f libs/printutils.o
	rm -f libs/libprintutils.a
	rm -f bin/main

server_run:
	clear
	./bin/server $(PORT) $(SOCKET_PATH)

client_run:
	clear
	./bin/client $(CLIENT_NICK) $(CONN_TYPE) $(ADDRESS) $(PORT)