# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c

# --- Iput values
THREADS = 16
MODE_BLOCKS = blocks
MODE_NUMBERS = numbers
IMAGE_INPUT_PATH = img/barbara.ascii.pgm
IMAGE_OUTPUT_PATH = img/negative.ascii.pgm

recompile: clean compile_lib compile_main run

compile_lib:
	$(CC) $(CFLAGS) $(LIB_PATH) -o libs/$(LIB_NAME).o
	ar rcs libs/lib$(LIB_NAME).a libs/$(LIB_NAME).o

compile_main:
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o bin/main -L./libs -l$(LIB_NAME) -pthread

clean:
	clear
	rm -f libs/printutils.o
	rm -f libs/libprintutils.a
	rm -f bin/main
	rm -f $(IMAGE_OUTPUT_PATH)

run:
	clear
	./bin/main $(THREADS) $(MODE_NUMBERS) $(IMAGE_INPUT_PATH) $(IMAGE_OUTPUT_PATH)