CC = gcc
CFLAGS = -O -c -w
REPORT_PATH = report.txt
LIB_PATH = src/wcutils.c
MAIN_PATH = src/main.c
CLEAN_EXT = o a so

SMALL_FILE_PATH = texts/small.txt
MEDIUM_FILE_PATH = texts/medium.txt
LARGE_FILE_PATH = texts/large.txt

static:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs libwcutils.a wcutils.o
	$(CC) -w $(MAIN_PATH) -o main -L. -lwcutils

shared:
	$(CC) -fPIC -c $(LIB_PATH)
	$(CC) -shared -fPIC -o libwcutils.so wcutils.o
	$(CC) $(MAIN_PATH) -lwcutils -L ./ -o main

clean:
	for file in $(CLEAN_EXT); do rm -f *.$$file; done

run:
	@echo ----TEST------
	./main 3 create_table 10 wc_files $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) print_table