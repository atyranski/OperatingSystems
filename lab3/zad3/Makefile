# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_PATH = src/main.c
TESTDIR_PATH = testDir

recompile: clean compile run

compile:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -l$(LIB_NAME) -lm

clean:
	clear
	make clear_out
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f main

clear_out:
	rm -f out/*.txt

run:
	./main $(TESTDIR_PATH) XDD 2