CFLAGS := $(shell sdl2-config --cflags)
CLIBS := $(shell sdl2-config --libs)

snake: clean
	cc -o build/snake src/main.c src/game.c $(CFLAGS) $(CLIBS)
debug: clean
	cc -o build/snake src/main.c src/game.c $(CFLAGS) $(CLIBS) -g
step-1: clean
	cc -o build/snake src/step-1/main.c $(CFLAGS) $(CLIBS) -g

clean: 
	rm -rf build/snake*
