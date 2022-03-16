# --- Utility variables
CC = gcc
CFLAGS = -c -w
OPTIMIZATION = 

# --- Programs source paths
MAIN_LIB_NAME = main_lib
MAIN_LIB_PATH = src/main_lib.c

MAIN_SYS_NAME = main_sys
MAIN_SYS_PATH = src/main_sys.c

PRINT_LIB_NAME = printutils
PRINT_LIB_PATH = libs/printutils.c

# --- File paths
LOREM_PATH = texts/lorem_ipsum.txt
DUIS_PATH = texts/duis_vel.txt
RAJ_PATH = texts/romeo_and_juliet.txt

DEST_PATH = texts/destination.txt

# --- Commands to run
execute_lib: clean compile_lib_main run

compile_lib_main:
	$(CC) $(CFLAGS) $(MAIN_LIB_PATH) $(OPTIMIZATION) -o main

clean:
	rm -f main

run:
	@echo
	@echo -------------- TEST_01 --------------
	./main $(LOREM_PATH)  $(DEST_PATH)  $(DEST_PATH)