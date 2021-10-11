#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "snake.h"
#include "food.h"

typedef enum Game_State {
    QUIT = 0,
    PLAYING = 1,
    PAUSED = 2,
    GAME_OVER = 3
} Game_State;

typedef struct {
    SDL_Renderer *renderer;
	SDL_Window *window;
    Game_State state;
    Snake snake;
    Food food;
    int score;
} Game;

extern Game game;

void init(void);
void run(void);
void play(void);
void display_score(void);

#endif