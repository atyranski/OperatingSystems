# --- Variables
CC = gcc
CFLAGS = -O -c -w
LIB_NAME = wcutils
LIB_PATH = src/wcutils.c

# ---- Commands to run
compile:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs libwcutils.a wcutils.o
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -lwcutils

clean:
	rm -f wcutils.o
	rm -f main
	rm -f out/raport2.txt