#include <stdlib.h>
#include "food.h"
#include "game.h"

void draw_food(void) {
    if (game.state == GAME_OVER) {
        return;
    }
    SDL_Rect block = {
        .w = CELL_WIDTH, .h = CELL_HEIGHT,
        .x = game.food.x,
        .y = game.food.y
    };
    // make food red
    SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(game.renderer, &block);
}

void spawn_food(void) {
    game.food.x = (rand() % (((SCREEN_WIDTH - CELL_WIDTH - WALL_WIDTH)/CELL_WIDTH)+1)*CELL_WIDTH);
    game.food.y = (rand() % (((SCREEN_HEIGHT - CELL_HEIGHT - WALL_HEIGHT)/CELL_HEIGHT)+1)*CELL_HEIGHT);

    if (game.food.x < WALL_WIDTH) {
        game.food.x = WALL_WIDTH;
    }

    if (game.food.y < WALL_HEIGHT) {
        game.food.y = WALL_HEIGHT;
    }

    for (int i = 0; i < sizeof(game.snake.body)/sizeof(game.snake.body[0]); i++) {
        if (game.snake.body[i].x == game.food.x && game.snake.body[i].y == game.food.y) {
            spawn_food();
            break;
        }
    }
}
