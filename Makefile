CFLAGS := $(shell sdl2-config --cflags)
CLIBS := $(shell sdl2-config --libs)

snake: clean
	cc -o build/snake src/main.c $(CFLAGS) $(CLIBS) -lSDL2_ttf -Wall -Wextra
	cp -r assets/* build/
debug: clean
	cc -o build/snake src/main.c $(CFLAGS) $(CLIBS) -g -lSDL2_ttf -Wall -Wextra
	cp -r assets/* build/
clean: 
	rm -rf build/snake*
