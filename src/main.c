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
void handle_collisions(void);
void spawn_food(void);
void draw_food(void);
void update_title(void);
void play_again(void);

typedef enum Game_State {
    NOT_PLAYING = 0,
    PLAYING = 1,
    PAUSED = 2,
    GAME_OVER = 3
} Game_State;

typedef struct {
  SDL_Renderer *renderer;
	SDL_Window *window;
  SDL_Rect snake[CELL_COUNT];
  int running;
  int dx;
  int dy;
  SDL_Rect food;
  int score;
  Game_State state;
} Game;

// initialize global structure to store game state
// and SDL renderer for use in all functions
Game game = {
  .running = 1,
  .dx = CELL_WIDTH,
  .food = {
    .w = CELL_WIDTH, .h = CELL_HEIGHT
  },
  .state = NOT_PLAYING
};

int main() {
  // Initialize SDL and the relevant structures
  initialize();

  spawn_snake();
  spawn_food();
  update_title();

  // enter game loop
  while (game.running) {
    // clear the screen with all black before drawing anything 
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    handle_input();
    move_snake();

    draw_food();
    draw_snake();
    draw_walls();

    SDL_RenderPresent(game.renderer);
    // wait 100 milliseconds before next iteration
    SDL_Delay(100);
  }

  // make sure program cleans up on exit
  terminate(EXIT_SUCCESS);
}

void initialize(void) {
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

void handle_input(void) {
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

      // start the game when first arrow key is pressed
      if (game.state == NOT_PLAYING) {
        game.state = PLAYING;
      }  
      change_direction(e.key.keysym.sym);
    }

    // handle pause/unpause
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
      switch (game.state)
      {
      case PAUSED:
        game.state = PLAYING;
        spawn_food();
        update_title();
        break;
      case PLAYING:
        game.state = PAUSED;
        update_title();
        break;
      case GAME_OVER:
        play_again();
        break;
      default:
        break;
      }
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
      // Make the snake red when it's dead
      if (game.state == GAME_OVER) {
        SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
      } else {
        // draw green cell
        SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
      }
      SDL_RenderFillRect(game.renderer, &game.snake[i]);

      // create a black border around each snake body cell
      SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(game.renderer, &game.snake[i]);
    }
  }
  // draw the snake head
  if (game.state == GAME_OVER) {
    SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
  } else {
    // draw green cell
    SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
  }
  SDL_RenderFillRect(game.renderer, &game.snake[0]);
}

void spawn_snake(void) {
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
  // only move snake when state is PLAYING
  if (game.state != PLAYING) {
    return;
  }
  // shift elements to right to make room for new head
  for (int i = sizeof(game.snake)/sizeof(game.snake[0])-1; i >= 0; i--) {
    game.snake[i] = game.snake[i-1];
  }
  // insert new head position at the begining
  game.snake[0].x = game.snake[1].x + game.dx;
  game.snake[0].y = game.snake[1].y + game.dy;
  game.snake[0].w = CELL_WIDTH;
  game.snake[0].h = CELL_HEIGHT;

  // if the head touces the snake, make it grow by not removing the tail
  // and spawn the food in a new random location.
  if (game.food.x == game.snake[0].x && game.food.y == game.snake[0].y) {
    spawn_food();
    game.score++;
    update_title();
  } else {
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
  }
  handle_collisions();
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

void handle_collisions(void) {
  // hit snake?
  for (int i = 1; i < sizeof(game.snake)/sizeof(game.snake[0]); i++) {
    // exit loop when at the end of the active elements of the snake body
    if (game.snake[i].w == 0) {
      break;
    }
    // check the head has not run into active body elements
    if (game.snake[0].x == game.snake[i].x && game.snake[0].y == game.snake[i].y) {
      game.state = GAME_OVER;
      update_title();
      return;
    }
  }
  // hit lift wall?
  if (game.snake[0].x < WALL_THICKNESS) {
    game.state = GAME_OVER;
    update_title();
    return;
  }
  // hit right wall?
  if (game.snake[0].x > SCREEN_WIDTH - WALL_THICKNESS - CELL_WIDTH) {
    game.state = GAME_OVER;
    update_title();
    return;
  }
  // hit top wall?
  if (game.snake[0].y < WALL_THICKNESS) {
    game.state = GAME_OVER;
    update_title();
    return;
  }
  // hit bottoom wall?
  if (game.snake[0].y > SCREEN_HEIGHT - WALL_THICKNESS - CELL_HEIGHT) {
    game.state = GAME_OVER;
    update_title();
    return;
  }
}

void spawn_food() {
  // generate a random number in multiples of 10 along the x axis that fits between the left and right walls 
  game.food.x = (rand() % (((SCREEN_WIDTH - CELL_WIDTH - WALL_THICKNESS)/CELL_WIDTH)+1)*CELL_WIDTH);
  // generate a random number in multiples of 10 along the y axis that fits between the top and bottom walls 
  game.food.y = (rand() % (((SCREEN_HEIGHT - CELL_HEIGHT - WALL_THICKNESS)/CELL_HEIGHT)+1)*CELL_HEIGHT);

  // if the random number generated is less than the thickness of the left wall,
  // make the food spawn next to the left wall
  if (game.food.x < WALL_THICKNESS) {
    game.food.x = WALL_THICKNESS;
  }

  // if the random number generated is less than the thickness of the top wall,
  // make the food spawn next to the top wall
  if (game.food.y < WALL_THICKNESS) {
    game.food.y = WALL_THICKNESS;
  }

  // only spawn the food if it does not touch the snake
  for (int i = 0; i < sizeof(game.snake)/sizeof(game.snake[0]); i++) {
    // exit loop when at the end of the active elements of the snake body
    if (game.snake[i].w == 0) {
      break;
    }
    if (game.snake[i].x == game.food.x && game.snake[i].y == game.food.y) {
      spawn_food();
      break;
    }
  }
}

void draw_food() {
  // make the food red
  SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(game.renderer, &game.food);
}

void update_title(void) {
  if (game.state == NOT_PLAYING) {
    SDL_SetWindowTitle(game.window, "PRESS ANY ARROW KEY TO START");
  }
  if (game.state == PLAYING) {
    char buffer[20];
    snprintf(buffer, 20, "SCORE: %d", game.score);
    SDL_SetWindowTitle(game.window, buffer);
  }
  if (game.state == GAME_OVER) {
    char buffer[60];
    snprintf(buffer, 60, "GAME OVER - YOU SCORED: %d - PRESS SPACE TO PLAY AGAIN", game.score);
    SDL_SetWindowTitle(game.window, buffer);
  }
  if (game.state == PAUSED) {
    char buffer[31];
    snprintf(buffer, 31, "PAUSED - PRESS SPACE TO RESUME");
    SDL_SetWindowTitle(game.window, buffer);
  }
}

void play_again(void) {
  game.dx = CELL_WIDTH;
  game.dy = 0;
  game.score = 0;
  game.state = PLAYING;
  spawn_snake();
  spawn_food();
  update_title();
}
