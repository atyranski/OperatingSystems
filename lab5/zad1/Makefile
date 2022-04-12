# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c
COMMANDS_PATH = commands/example_1.txt

recompile: clean compile_lib compile_main run

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

compile_main:
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -l$(LIB_NAME)

clean:
	clear
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f main

run:
	./main $(COMMANDS_PATH)