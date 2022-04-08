# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c
EXEC_FUNCTION_PATH = src/exec_function.c
COMMANDS_PATH = commands/example_1.txt

recompile: clean compile_lib compile_main compile_exec_function run

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

compile_main:
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -l$(LIB_NAME)

compile_exec_function:
	$(CC) -w $(EXEC_FUNCTION_PATH) $(OPTIMIZATION) -o exec_function -L. -l$(LIB_NAME)

clean:
	clear
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f main
	rm -f exec_function

run:
	./main $(COMMANDS_PATH)