# --- Variables
CC = gcc
OPTIMIZATION = 
OPTIMIZATION_LEVELS = -O0 -Os -O1
CFLAGS = -O -c -w
LIB_NAME = wcutils
LIB_PATH = src/wcutils.c
MAIN_PATH = src/main.c
MAIN_PATH_DYNAMIC = src/main_dynamic.c

SMALL_FILE_PATH = texts/small.txt
MEDIUM_FILE_PATH = texts/medium.txt
LARGE_FILE_PATH = texts/large.txt



# ---- Commands to run
# Static
recompile_static: clean_static compile_static run

execute_static: compile_static run

execute_static_optimized: compile_static run_optimized

# Shared
recompile_shared: clean_shared compile_static run

execute_shared: compile_shared run

execute_shared_optimized: compile_shared run_optimized

# Dynamic
recompile_dynamic: clean_dynamic compile_dynamic run

execute_dynamic: compile_dynamic run

execute_dynamic_optimized: compile_dynamic run_optimized

# Clean all
clean: clean_static clean_shared clean_dynamic



# ---- Support commands
# Static
compile_static:
	$(CC) $(CFLAGS) $(LIB_PATH)
	ar rcs libwcutils.a wcutils.o
	$(CC) -w $(MAIN_PATH) $(OPTIMIZATION) -o main -L. -lwcutils

clean_static:
	rm -f wcutils.o
	rm -f main
	rm -f out/raport2.txt

# Shared
compile_shared:
	$(CC) -fPIC -c -w $(LIB_PATH)
	$(CC) -shared $(LIB_NAME).o -o lib$(LIB_NAME).so
	$(CC) -c $(MAIN_PATH) $(OPTIMIZATION)
	$(CC) main.o -L. -l$(LIB_NAME) -Wl,-rpath=. -o main

clean_shared:
	rm -f wcutils.o
	rm -f main.o
	rm -f main
	rm -f libwcutils.a
	rm -f libwcutils.so
	rm -f out/raport2.txt

# Dynamic
compile_dynamic:
	$(CC) -fPIC -c -w $(LIB_PATH)
	$(CC) -shared $(LIB_NAME).o -o lib$(LIB_NAME).so
	$(CC) -w $(MAIN_PATH_DYNAMIC) -o main $(OPTIMIZATION) -ldl

clean_dynamic:
	rm -f libwcutils.so
	rm -f main
	rm -f wcutils.o
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

run_optimized:
	for OP in $(OPTIMIZATION_LEVELS); do make run OPTIMIZATION=$$OP; done