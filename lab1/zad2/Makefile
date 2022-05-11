# --- Variables
CC = gcc
CFLAGS = -O -c -w
LIB_NAME = wcutils
LIB_PATH = src/wcutils.c
MAIN_PATH = src/main.c

SMALL_FILE_PATH = texts/small.txt
MEDIUM_FILE_PATH = texts/medium.txt
LARGE_FILE_PATH = texts/large.txt

# ---- Commands to run
execute: clean compile run

compile:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs libwcutils.a wcutils.o
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -lwcutils

clean:
	rm -f wcutils.o
	rm -f main
	rm -f libwcutils.a
	rm -f out/raport2.txt

run:
	@echo 
	@echo 
	@echo -------------- TEST_01 --------------
	@echo opers: create, wc_files, print_table
	@echo 
	./main 3 create_table 3 wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) $(SMALL_FILE_PATH) print_table

	@echo 
	@echo
	@echo -------------- TEST_02 --------------
	@echo opers: create, wc_files, print_table, remove, print_table
	@echo 
	./main 5 create_table 3 wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) $(SMALL_FILE_PATH) print_table remove_block 0 print_table

	@echo 
	@echo
	@echo -------------- TEST_03 --------------
	@echo opers: create, wc_files, print_table, remove, print_table, wc_files, print_table, remove_block, print_table
	@echo 
	./main 9 create_table 3 wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) $(SMALL_FILE_PATH) print_table remove_block 0 print_table wc_files $(LARGE_FILE_PATH) $(MEDIUM_FILE_PATH) print_table remove_block 1 print_table

	@echo 
	@echo
	@echo -------------- TEST_04 --------------
	@echo opers: create, wc_files, print_table
	@echo 
	./main 3 create_table 2 wc_files $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) $(LARGE_FILE_PATH) print_table