#include <SDL.h>
#include "world.h"
#include "game.h"

void draw_world() {
    // make background gray (walls)
    SDL_SetRenderDrawColor(game.renderer, 210, 209, 205, 255); // gray
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