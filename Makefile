PROG_NAME := snake
BUILD_DIR := build
SRC_DIRS := src
CFLAGS := $(shell sdl2-config --cflags)
CLIBS := $(shell sdl2-config --libs)

snake: clean
	$(CC) -o $(BUILD_DIR)/$(PROG_NAME) $(SRC_DIRS)/*.c $(CFLAGS) $(CLIBS)
debug: clean
	$(CC) -o $(BUILD_DIR)/$(PROG_NAME) $(SRC_DIRS)/*.c $(CFLAGS) $(CLIBS) -g

clean: 
	rm -rf build/$(PROG_NAME)*
