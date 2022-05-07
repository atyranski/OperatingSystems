# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c
DELIVERER_PATH = src/deliverer.c
COOK_PATH = src/cook.c

recompile: clean compile_lib compile_main compile_deliverer compile_cook run

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH) -o libs/$(LIB_NAME).o
	ar rcs libs/lib$(LIB_NAME).a libs/$(LIB_NAME).o

compile_main:
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o bin/main -L./libs -l$(LIB_NAME)

compile_deliverer:
	$(CC) -w $(DELIVERER_PATH) $(OPTIMIZATION) -o bin/deliverer -L./libs -l$(LIB_NAME)

compile_cook:
	$(CC) -w $(COOK_PATH) $(OPTIMIZATION) -o bin/cook -L./libs -l$(LIB_NAME)

clean:
	clear
	rm -f libs/printutils.o
	rm -f libs/libprintutils.a
	rm -f bin/main
	rm -f bin/deliverer
	rm -f bin/cook

run:
	./bin/main 3 3