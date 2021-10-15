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
void move_snake(void);
void change_direction(SDL_KeyCode new_direction);

typedef struct {
  SDL_Renderer *renderer;
	SDL_Window *window;
  int running;
  SDL_Rect snake[CELL_COUNT];
  int dx;
  int dy;
} Game;

// initialize global structure to store game state
// and SDL renderer for use in all functions
Game game = {
  .running = 1,
  .snake = {0},
  .dx = CELL_WIDTH,
  .dy = 0
};

int main() {
  // Initialize SDL and the relevant structures
  initialize();

  spawn_snake();

  // enter game loop
  while (game.running) {
    // clear the screen with all black before drawing anything 
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    handle_input();
    move_snake();

        // TODO: add update and draw functions
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
    // change the snake direction
    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_UP 
                              || e.key.keysym.sym == SDLK_DOWN 
                              || e.key.keysym.sym == SDLK_LEFT
                              || e.key.keysym.sym == SDLK_RIGHT)) {
        change_direction(e.key.keysym.sym);
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
  // draw snake body
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
  // draw the snake head
  SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
  SDL_RenderFillRect(game.renderer, &game.snake[0]);

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

void move_snake(void) {
  // shift elements to right to make room for new head
  for (int i = sizeof(game.snake)/sizeof(game.snake[0])-1; i >= 0; i--) {
    game.snake[i] = game.snake[i-1];
  }

  // insert new head position at the begining
  game.snake[0].x = game.snake[1].x + game.dx;
  game.snake[0].y = game.snake[1].y + game.dy;
  game.snake[0].w = CELL_WIDTH;
  game.snake[0].h = CELL_HEIGHT;

  //remove the tail by finding the last inactive element in the Snake array
  //then zeroing out the one before it.
  for (int i = 5; i < sizeof(game.snake)/sizeof(game.snake[0]); i++) {
    if (game.snake[i].w == 0) {
      game.snake[i-1].x = 0;
      game.snake[i-1].y = 0;
      game.snake[i-1].w = 0;
      game.snake[i-1].h = 0;
      break;
    }
  }

  // TODO: 
  // if the Snake ate food don't remove tail and increase score
  // Check if the Snake is Dead
}

void change_direction(SDL_KeyCode new_direction) {
  // work out what direction the snake is going
  int going_up = game.dy == -CELL_HEIGHT;
  int going_down = game.dy == CELL_HEIGHT;
  int going_left = game.dx == -CELL_WIDTH;
  int going_right = game.dx == CELL_WIDTH;

  // change the direction to up when the snake is not going down
  if (new_direction == SDLK_UP && !going_down) {
      game.dx = 0;
      game.dy = -CELL_HEIGHT;
  }
  // change the direction to down when the snake is not going up
  if (new_direction == SDLK_DOWN && !going_up) {
      game.dx = 0;
      game.dy = CELL_HEIGHT;
  }
  // change the direction to left when the snake is not going right
  if (new_direction == SDLK_LEFT && !going_right) {
      game.dx = -CELL_WIDTH;
      game.dy = 0;
  }
  // change the direction to right when the snake is not going left
  if (new_direction == SDLK_RIGHT && !going_left) {
      game.dx = CELL_WIDTH;
      game.dy = 0;
  }
}