# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c

# --- Text file paths
DEST_PATH = texts/destination.txt
DV_PATH = texts/duis_vel.txt
LI_PATH = texts/lorem_ipsum.txt
RAJ_PATH = texts/romeo_and_juliet.txt

recompile: clean compile run

compile:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -l$(LIB_NAME)

clean:
	clear
	rm -f printutils.o
	rm -f main

run:
	./main $(DEST_PATH) $(DV_PATH)