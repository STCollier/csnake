#include <stdlib.h>
#include "game.h"

Game game;

void initialize(void) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error: failed to initialize SDL: %s\n", SDL_GetError());
		exit(1);
  }

  game.window = SDL_CreateWindow("Score: 0", 
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

  game.food.w = CELL_WIDTH;
  game.food.h = CELL_HEIGHT;
  game.snake = snake_init();
  game.score = 0;
  game.state = PLAYING;
  spawn_food();
}

void terminate(void) {
  if (game.renderer) {
    SDL_DestroyRenderer(game.renderer);
	}
  if (game.window) {
    SDL_DestroyWindow(game.window);
  }
  SDL_Quit();
}

void run(void) {
  initialize();

  // loop until game state is changed to QUIT
  while (game.state != QUIT) {
    handle_input();
    move_snake();

    draw_world();
    draw_snake();
    draw_food();

    SDL_RenderPresent(game.renderer);
    SDL_Delay(100);
  }

  // make sure program cleans up on exit
  atexit(terminate);
}

void handle_input(void) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
          game.state = QUIT;
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
          change_direction(SDLK_UP);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
          change_direction(SDLK_DOWN);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
          change_direction(SDLK_LEFT);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
          change_direction(SDLK_RIGHT);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
          switch (game.state)
          {
          case PAUSED:
              game.state = PLAYING;
              display_score();
              break;
          case PLAYING:
              game.state = PAUSED;
              SDL_SetWindowTitle(game.window, "PAUSED");
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

void move_snake(void) {
    if (game.state != PLAYING) {
        return;
    }

    SDL_Rect head = { 
        .x = game.snake.body[0].x + game.snake.dx, 
        .y = game.snake.body[0].y + game.snake.dy,
        .w = CELL_WIDTH,
        .h = CELL_HEIGHT
    };

    // shift elements to right to make room for new head
    for (int i = sizeof(game.snake.body)/sizeof(game.snake.body[0])-1; i >= 0; i--) {
        game.snake.body[i] = game.snake.body[i-1];
    }

    // insert new head position at begining
    game.snake.body[0] = head;

    // remove tail if not growing.
    if (!hit_food()) {
        for (int i = 0; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
            if (game.snake.body[i].x == 0) {
                game.snake.body[i-1].x = 0; 
                game.snake.body[i-1].y = 0;
                break;
            }
        }
    }

    if (is_dead()) {
        game.state = GAME_OVER;
    }
}

void play_again(void) {
  game.snake = snake_init();
  game.state = PLAYING;
  game.score = 0;
  spawn_food();
  display_score();
}

void display_score(void) {
  char buffer[20];
  snprintf(buffer, 20, "Score: %d", game.score);
  SDL_SetWindowTitle(game.window, buffer);
}

void draw_world(void) {
    // make background gray (walls)
    SDL_SetRenderDrawColor(game.renderer, 210, 209, 205, 255);
    SDL_RenderClear(game.renderer);

    // create block to draw playing area
    SDL_Rect block = {
        .x = WALL_WIDTH,
        .y = WALL_HEIGHT,
        .w = SCREEN_WIDTH - (WALL_WIDTH * 2),
        .h = SCREEN_HEIGHT - (WALL_HEIGHT * 2)
    };

    // make playing area black
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(game.renderer, &block);
}

void draw_snake(void) {
    // render the snake body
    for (int i = 1; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
        if (game.snake.body[i].x != 0) {
          SDL_Rect body = game.snake.body[i];
          // make each snake cell green or red when dead
          if (game.state == GAME_OVER) {
            SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
          } else {
            SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
          }
          SDL_RenderFillRect(game.renderer, &body);
          
          // create a black border around each snake cell
          SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
          SDL_RenderDrawRect(game.renderer, &body);
        }
    }

    // render head
    if (game.state != GAME_OVER) {
        SDL_Rect body = game.snake.body[0];
        SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
        SDL_RenderFillRect(game.renderer, &body);
    }
}

void draw_food(void) {
    // make the food red
    SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(game.renderer, &game.food);
}

void spawn_food(void) {
    // generate a random number in multiples of 10 along the x axis that fits between the left and right walls 
    game.food.x = (rand() % (((SCREEN_WIDTH - CELL_WIDTH - WALL_WIDTH)/CELL_WIDTH)+1)*CELL_WIDTH);
    // generate a random number in multiples of 10 along the y axis that fits between the top and bottom walls 
    game.food.y = (rand() % (((SCREEN_HEIGHT - CELL_HEIGHT - WALL_HEIGHT)/CELL_HEIGHT)+1)*CELL_HEIGHT);

    // if the random number generated is less than the left walls width
    // make the food spawn next to the left wall
    if (game.food.x < WALL_WIDTH) {
        game.food.x = WALL_WIDTH;
    }

    // if the random number generated is less than the top walls width
    // make the food spawn next to the top wall
    if (game.food.y < WALL_HEIGHT) {
        game.food.y = WALL_HEIGHT;
    }

    // only spawn the food if it does not touch the snake
    for (int i = 0; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
        if (game.snake.body[i].x == 0) {
          break;
        }
        if (game.snake.body[i].x == game.food.x && game.snake.body[i].y == game.food.y) {
            spawn_food();
            break;
        }
    }
}

Snake snake_init(void) {
    Snake snake = {
        .dx = CELL_WIDTH,
        .dy = 0,
        .body = {
            {200, 200, CELL_WIDTH, CELL_HEIGHT},
            {190, 200, CELL_WIDTH, CELL_HEIGHT},
            {180, 200, CELL_WIDTH, CELL_HEIGHT},
            {170, 200, CELL_WIDTH, CELL_HEIGHT},
            {160, 200, CELL_WIDTH, CELL_HEIGHT}
        }
    };
    return snake;
}

void change_direction(SDL_KeyCode new_direction) {
    int going_up = game.snake.dy == -CELL_HEIGHT;
    int going_down = game.snake.dy == CELL_HEIGHT;
    int going_left = game.snake.dx == -CELL_WIDTH;
    int going_right = game.snake.dx == CELL_WIDTH;

    if (new_direction == SDLK_UP && !going_down) {
        game.snake.dx = 0;
        game.snake.dy = -CELL_HEIGHT;
    }

    if (new_direction == SDLK_DOWN && !going_up) {
        game.snake.dx = 0;
        game.snake.dy = CELL_HEIGHT;
    }

    if (new_direction == SDLK_LEFT && !going_right) {
        game.snake.dx = -CELL_WIDTH;
        game.snake.dy = 0;
    }

    if (new_direction == SDLK_RIGHT && !going_left) {
        game.snake.dx = CELL_WIDTH;
        game.snake.dy = 0;
    }
}

int is_dead(void) {
    // hit snake?
    for (int i = 1; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
        if (game.snake.body[i].x == 0) {
            break;
        }
        if (game.snake.body[0].x == game.snake.body[i].x 
          && game.snake.body[0].y == game.snake.body[i].y) {
              return 1;
        }
    }

    // hit lift wall?
    if (game.snake.body[0].x < WALL_WIDTH) {
        return 1;
    }
    // hit right wall?
    if (game.snake.body[0].x > SCREEN_WIDTH - WALL_WIDTH - CELL_WIDTH) {
        return 1;
    }
    // hit top wall?
    if (game.snake.body[0].y < WALL_HEIGHT) {
        return 1;
    }
    // hit bottoom wall?
    if (game.snake.body[0].y > SCREEN_HEIGHT - WALL_HEIGHT - CELL_HEIGHT) {
        return 1;
    }
    return 0;
}

int hit_food(void) {
    if (game.food.x == game.snake.body[0].x && game.food.y == game.snake.body[0].y) {
        game.score++;
        spawn_food();
        display_score();
        return 1;
    }
    return 0;
}
