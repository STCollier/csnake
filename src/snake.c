#include <SDL.h>
#include "snake.h"
#include "game.h"
#include "food.h"

Snake snake_init() {
    Snake snake = {
        .dx = CELL_WIDTH,
        .dy = 0,
        .body = {
            {200, 200, 1},
            {190, 200, 1},
            {180, 200, 1},
            {170, 200, 1},
            {160, 200, 1}
        }
    };
    return snake;
}

void move_snake(void) {
    if (game.state != PLAYING) {
        return;
    }

    BodyPart head = { 
        .x = game.snake.body[0].x + game.snake.dx, 
        .y = game.snake.body[0].y + game.snake.dy,
        .visible = 1
    };

    // shift elements to right
    for (int i = sizeof(game.snake.body)/sizeof(game.snake.body[0])-1; i >= 0; i--) {
        game.snake.body[i] = game.snake.body[i-1];
    }

    // insert new head position at begining
    game.snake.body[0] = head;

    // remove tail if not growing.
    if (!hit_food()) {
        for (int i = 0; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
            if (game.snake.body[i].visible == 0) {
                game.snake.body[i-1].x = 0; 
                game.snake.body[i-1].y = 0;
                game.snake.body[i-1].visible = 0;
                break;
            }
        }
    }

    if (is_dead()) {
        game.state = GAME_OVER;
    }
}

void draw_snake(void) {

    // render the snake body
    for (int i = 1; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
        if (game.snake.body[i].visible == 1) {
          SDL_Rect block = {
              .w = CELL_WIDTH, .h = CELL_HEIGHT,
              .x = game.snake.body[i].x,
              .y = game.snake.body[i].y
          };
          // make each snake cell green or red when dead
          if (game.state == GAME_OVER) {
            SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
          } else {
            SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
          }
          SDL_RenderFillRect(game.renderer, &block);
          
          // create a black border around each snake cell
          SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
          SDL_RenderDrawRect(game.renderer, &block);
        }
    }

    // render head
    if (game.state != GAME_OVER) {
        SDL_Rect block = {
        .w = CELL_WIDTH, .h = CELL_HEIGHT,
        .x = game.snake.body[0].x,
        .y = game.snake.body[0].y
        };
        SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
        SDL_RenderFillRect(game.renderer, &block);
    }
}

void change_direction(enum Direction new_direction) {
    int going_up = game.snake.dy == -CELL_HEIGHT;
    int going_down = game.snake.dy == CELL_HEIGHT;
    int going_left = game.snake.dx == -CELL_WIDTH;
    int going_right = game.snake.dx == CELL_WIDTH;

    if (new_direction == UP && !going_down) {
        game.snake.dx = 0;
        game.snake.dy = -CELL_HEIGHT;
    }

    if (new_direction == DOWN && !going_up) {
        game.snake.dx = 0;
        game.snake.dy = CELL_HEIGHT;
    }

    if (new_direction == LEFT && !going_right) {
        game.snake.dx = -CELL_WIDTH;
        game.snake.dy = 0;
    }

    if (new_direction == RIGHT && !going_left) {
        game.snake.dx = CELL_WIDTH;
        game.snake.dy = 0;
    }
}

int is_dead() {
    // hit snake?
    for (int i = 1; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
        if (!game.snake.body[i].visible) {
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
