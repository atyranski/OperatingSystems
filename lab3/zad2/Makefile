# --- Variables
CC = gcc
CFLAGS = -O -c -w
OPTIMIZATION = 

# --- Program source paths
MAIN_PATH = src/main.c

recompile: clean compile run

compile:
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -lm

clean:
	clear
	rm -f main

run:
	./main 5 10
