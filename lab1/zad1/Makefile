# --- Variables
CC = gcc
CFLAGS = -O -c -w
LIB_NAME = wcutils
LIB_PATH = src/wcutils.c

# ---- Commands to run
compile:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs libwcutils.a wcutils.o

clean:
	rm -f wcutils.o
	rm -f libwcutils.a