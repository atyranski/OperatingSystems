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
	rm -f printutils.o
	rm -f libprintutils.a
	rm -f main

test_01:
	@echo ---- TEST 01 ---
	./main $(TESTDIR_PATH) placek 2

test_02:
	@echo ---- TEST 02 ---
	./main $(TESTDIR_PATH) Lorem 2

test_03:
	@echo ---- TEST 03 ---
	./main $(TESTDIR_PATH) XDD 2

test_04:
	@echo ---- TEST 04 ---
	./main $(TESTDIR_PATH) AAA 1

run: test_01 test_02 test_03 test_04
	