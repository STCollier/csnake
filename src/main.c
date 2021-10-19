#include <stdio.h>
#include <SDL.h>

#define TARGET_FPS      50
#define MS_PER_UPDATE   1000 / 30
#define MS_PER_FRAME    1000 / TARGET_FPS
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
#define SNAKE_SPEED     8.0f

void initialize(void);
void terminate(int exit_code);
void process_input(void);
void update();
void render();

void draw_walls(void);
void draw_snake(void);
void spawn_snake(void);
void move_snake(void);
void change_direction(SDL_KeyCode new_direction);
void handle_collisions(void);
void spawn_food(void);
void draw_food(void);
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
  SDL_FPoint head;
  SDL_Rect snake[CELL_COUNT];
  int running;
  float dx;
  float dy;
  SDL_Rect food;
  int score;
  Game_State state;
} Game;

// initialize global structure to store game state
// and SDL renderer for use in all functions
Game game = {
  .running = 1,
  .dx = SNAKE_SPEED,
  .food = {
    .w = CELL_WIDTH, .h = CELL_HEIGHT
  },
  .state = NOT_PLAYING
};

int main() {
  // Initialize SDL and the relevant structures
  initialize();

  spawn_snake();

  // some vars to keep track of frame rate
  Uint32 previous = SDL_GetTicks();
  Uint32 lag;

  Uint32 previous_second = previous;
  int frame_count = 0;

  while (game.running) {
    Uint32 current = SDL_GetTicks();
    Uint32 elapsed = current - previous;
    previous = current;
    lag += elapsed;

    process_input();

    while (lag >= MS_PER_UPDATE) {
      update();
      lag -= MS_PER_UPDATE;
    }

    render();

    frame_count++;

      // time since work done
    Uint32 frame_end = SDL_GetTicks();

    // After every second, update the window title.
    if (frame_end - previous_second >= 1000) {
      char buffer[17];
      snprintf(buffer, 17, "Snake (FPS: %d)", frame_count);
      SDL_SetWindowTitle(game.window, buffer);
      frame_count = 0;
      previous_second = frame_end;
    }
    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    Uint32 frame_duration = frame_end - current;
    if (frame_duration < MS_PER_FRAME) {
      SDL_Delay(MS_PER_FRAME - frame_duration);
    }
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

void process_input(void) {
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
        spawn_food();
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
        break;
      case PLAYING:
        game.state = PAUSED;
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

void update() {
  move_snake();
}

void render() {
    // clear the screen with all black before drawing anything 
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    draw_food();
    draw_snake();
    draw_walls();

    SDL_RenderPresent(game.renderer);
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
    if (game.snake[i].w == 0) {
      break;
    }
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
  game.head.x = SNAKE_START_X;
  game.head.y = SNAKE_START_Y;
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

  game.head.x = game.head.x + game.dx;
  game.head.y = game.head.y + game.dy;

  // work out if the new x and y has moved into a new grid cell
  int old_x = (int)round(game.snake[0].x / CELL_WIDTH) * CELL_WIDTH;
  int new_x = (int)round(game.head.x / CELL_WIDTH) * CELL_WIDTH;
  int old_y = (int)round(game.snake[0].y / CELL_HEIGHT) * CELL_HEIGHT;
  int new_y = (int)round(game.head.y / CELL_HEIGHT) * CELL_HEIGHT;

  if (old_x == new_x && old_y == new_y) {
    // Snake head hasn't moved into a new grid cell
    return;
  }

  // shift elements to right to make room for new head
  for (int i = sizeof(game.snake)/sizeof(game.snake[0])-1; i >= 0; i--) {
    game.snake[i] = game.snake[i-1];
  }
  // insert new head position at the begining
  game.snake[0].x = new_x;
  game.snake[0].y = new_y;
  game.snake[0].w = CELL_WIDTH;
  game.snake[0].h = CELL_HEIGHT;

  // if the head touces the snake, make it grow by not removing the tail
  // and spawn the food in a new random location.
  if (game.food.x == game.snake[0].x && game.food.y == game.snake[0].y) {
    spawn_food();
    game.score++;
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
      game.dy = -SNAKE_SPEED;
  }
  // change the direction to down when the snake is not going up
  if (new_direction == SDLK_DOWN && !going_up) {
      game.dx = 0;
      game.dy = SNAKE_SPEED;
  }
  // change the direction to left when the snake is not going right
  if (new_direction == SDLK_LEFT && !going_right) {
      game.dx = -SNAKE_SPEED;
      game.dy = 0;
  }
  // change the direction to right when the snake is not going left
  if (new_direction == SDLK_RIGHT && !going_left) {
      game.dx = SNAKE_SPEED;
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
      return;
    }
  }
  // hit lift wall?
  if (game.snake[0].x < WALL_THICKNESS) {
    game.state = GAME_OVER;
    return;
  }
  // hit right wall?
  if (game.snake[0].x > SCREEN_WIDTH - WALL_THICKNESS - CELL_WIDTH) {
    game.state = GAME_OVER;
    return;
  }
  // hit top wall?
  if (game.snake[0].y < WALL_THICKNESS) {
    game.state = GAME_OVER;
    return;
  }
  // hit bottoom wall?
  if (game.snake[0].y > SCREEN_HEIGHT - WALL_THICKNESS - CELL_HEIGHT) {
    game.state = GAME_OVER;
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
  // orange
  SDL_SetRenderDrawColor(game.renderer, 255, 215, 0, 255);
  if (game.score % 2 == 1) {
    // red
    SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
    for (int i=0; i<10000; i++) {
      SDL_RenderFillRect(game.renderer, &game.food);
    }
  }
  SDL_RenderFillRect(game.renderer, &game.food);
}

void play_again(void) {
  game.dx = SNAKE_SPEED;
  game.dy = 0;
  game.score = 0;
  game.state = PLAYING;
  spawn_snake();
  spawn_food();
}
