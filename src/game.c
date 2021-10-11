#include "common.h"
#include "game.h"
#include "world.h"
#include "input.h"

Game game;

void init(void) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error: failed to initialize SDL: %s\n", SDL_GetError());
		exit(1);
  }

  game.window = SDL_CreateWindow("Snake", 
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED, 
    SCREEN_WIDTH, 
    SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN);

	if (!game.window) {
		printf("error: failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(2);
	}

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);

	if (!game.renderer) {
		printf("error: failed to create renderer: %s\n", SDL_GetError());
		exit(3);
	}
  play();
}

void run(void) {
  while (game.state != QUIT) {
    handle_input();
    move_snake();

    // Render game objects

    draw_world();
    draw_snake();
    draw_food();

    SDL_RenderPresent(game.renderer);
    SDL_Delay(100);
  }

  // not running so free up resources
  SDL_DestroyRenderer(game.renderer);
  SDL_DestroyWindow(game.window);
  SDL_Quit();
}

void play() {
  game.snake = snake_init();
  game.score = 0;
  game.state = PLAYING;
  display_score();
  spawn_food();
}

void display_score() {
  char buffer[20];
  snprintf(buffer, 20, "Score: %d", game.score);
  SDL_SetWindowTitle(game.window, buffer);
}