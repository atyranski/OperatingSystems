# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
CLIENT_PATH = src/client.c
SERVER_PATH = src/server.c

recompile: clean compile_lib compile_client compile_server run_server

recompile_client:
	clear
	make compile_client
	./client

recompile_server:
	clear
	make compile_server
	./server

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

compile_client:
	$(CC) -w $(CLIENT_PATH) $(OPTIMIZATION) -o client -L. -l$(LIB_NAME)

compile_server:
	$(CC) -w $(SERVER_PATH) $(OPTIMIZATION) -o server -L. -l$(LIB_NAME)

clean:
	clear
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f client
	rm -f server

run_server:
	./server