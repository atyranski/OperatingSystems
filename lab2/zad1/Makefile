# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
LIB_NAME = printutils
LIB_PATH = src/printutils.c
MAIN_HIGH_PATH = src/main_high.c
MAIN_LOW_PATH = src/main_low.c

# --- Text file paths
DEST_PATH = texts/destination.txt
LI_PATH = texts/lorem_ipsum.txt
DV_PATH = texts/duis_vel.txt
RAJ_PATH = texts/romeo_and_juliet.txt
F_PATH = texts/from.txt
T_PATH = texts/to.txt

recompile_low: clean compile_low run

recompile_high: clean compile_high run

compile_high:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o
	$(CC) -w $(MAIN_HIGH_PATH) $(OPTIMIZATION) -o main -L. -l$(LIB_NAME)

compile_low:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o
	$(CC) -w $(MAIN_LOW_PATH) $(OPTIMIZATION) -o main -L. -l$(LIB_NAME)

clean:
	clear
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f main

clean_raports:
	rm -f out/*.txt

run:
	./main $(LI_PATH) $(DEST_PATH)
	./main $(DV_PATH) $(DEST_PATH)
	./main $(RAJ_PATH) $(DEST_PATH)
	./main $(F_PATH) $(T_PATH)
	./main