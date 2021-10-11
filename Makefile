snake: clean
	cc -o build/snake src/*.c -I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib -lSDL2
debug: clean
	cc -o build/snake -g src/*.c -I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib -lSDL2
clean: 
	rm -rf build/snake*
