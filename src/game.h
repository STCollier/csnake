#ifndef GAME_H
#define GAME_H

#define SCREEN_WIDTH   400
#define SCREEN_HEIGHT  400
#define WALL_WIDTH      20
#define WALL_HEIGHT     20
#define CELL_WIDTH      20
#define CELL_HEIGHT     20
#define CELL_COUNT     ((SCREEN_WIDTH-WALL_WIDTH*2)*     \
                        (SCREEN_HEIGHT-WALL_HEIGHT*2))/  \
                        (CELL_WIDTH*CELL_HEIGHT)

#include <SDL.h>

typedef enum Game_State {
    QUIT = 0,
    PLAYING = 1,
    PAUSED = 2,
    GAME_OVER = 3
} Game_State;

typedef struct Snake {
    SDL_Rect body[CELL_COUNT];
    int dx;
    int dy;
} Snake;

typedef struct {
    SDL_Renderer *renderer;
	SDL_Window *window;
    Game_State state;
    Snake snake;
    SDL_Rect food;
    int score;
} Game;

extern Game game;

void initialize(void);
void terminate(void);
void run(void);
void play_again(void);
void display_score(void);
void handle_input(void); 
void draw_world(void);
void draw_food(void);
void spawn_food(void);
Snake snake_init(void);
void move_snake(void);
void draw_snake(void);
void change_direction(SDL_KeyCode new_direction);
int is_dead(void);
int hit_food(void);

#endif