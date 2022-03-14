CC = gcc
CFLAGS = -O -c -w
REPORT_PATH = report.txt
LIB_NAME = wcutils
LIB_PATH = src/wcutils.c
MAIN_PATH = src/main.c

SMALL_FILE_PATH = texts/small.txt
MEDIUM_FILE_PATH = texts/medium.txt
LARGE_FILE_PATH = texts/large.txt

OP_0 = -O0
OP_s = -Os
OP_1 = -O1

# Commands to run
execute_static: compile_static run

execute_shared: compile_shared run

clean: clean_static clean_shared


# Support commands
compile_static:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs libwcutils.a wcutils.o
	$(CC) -w $(MAIN_PATH) -o main -L. -lwcutils

clean_static:
	rm -f wcutils.o
	rm -f main
	rm -f out/raport2.txt

compile_shared:
	$(CC) -fPIC -c -w $(LIB_PATH)
	$(CC) -shared $(LIB_NAME).o -o lib$(LIB_NAME).so
	$(CC) -c $(MAIN_PATH)
	$(CC) main.o -L. -l$(LIB_NAME) -Wl,-rpath=. -o main

clean_shared:
	rm -f wcutils.o
	rm -f main.o
	rm -f main
	rm -f libwcutils.a
	rm -f libwcutils.so
	rm -f out/raport2.txt

run:
	@echo ----------- TEST_01 --------
	./main 3 create_table 3 wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) $(SMALL_FILE_PATH) print_table

	@echo ----------- TEST_01 --------
	./main 5 create_table 3 wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) $(SMALL_FILE_PATH) print_table remove_block 0 print_table

	@echo ----------- TEST_02 --------
	./main 9 create_table 3 wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) $(SMALL_FILE_PATH) print_table remove_block 0 print_table wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) print_table remove_block 1 print_table