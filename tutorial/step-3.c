#include <stdio.h>
#include <SDL.h>

#define SCREEN_WIDTH   680
#define SCREEN_HEIGHT  400
#define WALL_THICKNESS  20
#define CELL_WIDTH      20
#define CELL_HEIGHT     20
#define CELL_COUNT     ((SCREEN_WIDTH-WALL_THICKNESS*2)*     \
                        (SCREEN_HEIGHT-WALL_THICKNESS*2))/  \
                        (CELL_WIDTH*CELL_HEIGHT)
#define SNAKE_START_X   200
#define SNAKE_START_Y   200

void initialize(void);
void terminate(int exit_code);
void handle_input(void);
void draw_walls(void);
void draw_snake(void);
void spawn_snake(void);

typedef struct {
  SDL_Renderer *renderer;
	SDL_Window *window;
  int running;
  SDL_Rect snake[CELL_COUNT];
} Game;

// initialize global structure to store game state
// and SDL renderer for use in all functions
Game game = {
  .running = 1,
};

int main() {
  // Initialize SDL and the relevant structures
  initialize();

  spawn_snake();

  // enter game loop
  while (game.running) {
    handle_input();

    // TODO: add update and draw functions
    // move_snake()
    // draw_food()

    draw_snake();
    draw_walls();

    SDL_RenderPresent(game.renderer);

    // wait 100 milliseconds before next iteration
    SDL_Delay(100);
  }

  // make sure program cleans up on exit
  terminate(EXIT_SUCCESS);
}

void initialize() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error: failed to initialize SDL: %s\n", SDL_GetError());
		terminate(EXIT_FAILURE);
  }

  // create the game window
  game.window = SDL_CreateWindow("Score: 0", 
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED, 
    SCREEN_WIDTH, 
    SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN
  );

	if (!game.window) {
		printf("error: failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		terminate(EXIT_FAILURE);
	}

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);

  if (!game.renderer) {
    printf("error: failed to create renderer: %s\n", SDL_GetError());
    terminate(EXIT_FAILURE);
  }
}

void terminate(int exit_code) {
  if (game.renderer) {
    SDL_DestroyRenderer(game.renderer);
	}
  if (game.window) {
    SDL_DestroyWindow(game.window);
  }
  SDL_Quit();
  exit(exit_code);
}

void handle_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
      game.running = 0;
    }
  }
}

void draw_walls(void) {
  // make the walls gray
  SDL_SetRenderDrawColor(game.renderer, 210, 209, 205, 255);

  SDL_Rect block = {
        .x = 0,
        .y = 0,
        .w = WALL_THICKNESS,
        .h = SCREEN_HEIGHT
  };

  // left wall
  SDL_RenderFillRect(game.renderer, &block);

  // right wall
  block.x = SCREEN_WIDTH - WALL_THICKNESS;
  SDL_RenderFillRect(game.renderer, &block);

  // top wall
  block.x = 0;
  block.w = SCREEN_WIDTH;
  block.h = WALL_THICKNESS;
  SDL_RenderFillRect(game.renderer, &block);

  // bottom wall
  block.y = SCREEN_HEIGHT - WALL_THICKNESS;
  SDL_RenderFillRect(game.renderer, &block);
}

void draw_snake(void) {
  // draw the snake head
  SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
  SDL_RenderFillRect(game.renderer, &game.snake[0]);

  // draw snake
  for (int i = 1; i < sizeof(game.snake)/sizeof(game.snake[0]); i++) {
    // only draw active elements (assigned a width)
    if (game.snake[i].w != 0) {
      // TODO: Make the snake red when it is dead
      // draw green cell
      SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
      SDL_RenderFillRect(game.renderer, &game.snake[i]);
      
      // create a black border around each snake body cell
      SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(game.renderer, &game.snake[i]);
    }
  }
}

void spawn_snake() {
  // make all snake parts empty
  for (int i = 0; i < sizeof(game.snake)/sizeof(game.snake[0]); i++) {
    game.snake[i].x = 0;
    game.snake[i].y = 0;
    game.snake[i].w = 0;
    game.snake[i].h = 0;
  }

  // the first element in the snake array is the head
  game.snake[0].x = SNAKE_START_X;
  game.snake[0].y = SNAKE_START_Y;
  game.snake[0].w = CELL_WIDTH;
  game.snake[0].h = CELL_HEIGHT;

  // create 4 more snake elements for the body
  for (int i = 1; i < 5; i++) {
    game.snake[i] = game.snake[0];
    game.snake[i].x = game.snake[0].x - (CELL_WIDTH * i);
  }
}
