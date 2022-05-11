# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c
CATCHER_PATH = src/catcher.c
SENDER_PATH = src/sender.c

recompile: clean compile_lib compile_sender compile_catcher

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

compile_sender:
	$(CC) -w $(SENDER_PATH) $(OPTIMIZATION) -o sender -L. -l$(LIB_NAME)

compile_catcher:
	$(CC) -w $(CATCHER_PATH) $(OPTIMIZATION) -o catcher -L. -l$(LIB_NAME)

clean:
	clear
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f sender
	rm -f catcher