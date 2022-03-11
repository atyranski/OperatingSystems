CC = gcc
CFLAGS = -O -c -w
SRC_PATH = src/wcutils.c
CLEAN_EXT = o a so
BUILD_PATH = build/wcutils.o
OUTPUT_STATIC_PATH = out/libstaticwcutils.a
OUTPUT_SHARED_PATH = out/libsharedwcutils.so
# DEBUG = -ggdb

all: link

link: compile
	ar cr $(OUTPUT_STATIC_PATH) $(BUILD_PATH)
	gcc -shared -o $(OUTPUT_SHARED_PATH) $(BUILD_PATH)

compile:
	cd build && $(CC) $(CFLAGS) $(SRC_PATH) -I $(DEBUG)
	cd build && $(CC) $(CFLAGS) -fPIC $(SRC_PATH) -I. $(DEBUG)

clean:
	for file in $(CLEAN_EXT); do rm -f *.$$file; done	