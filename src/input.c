#include "input.h"
#include "game.h"
#include "snake.h"

void handle_input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
          game.state = QUIT;
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
          change_direction(UP);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
          change_direction(DOWN);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LEFT) {
          change_direction(LEFT);
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RIGHT) {
          change_direction(RIGHT);
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
              play();
              break;
          default:
              break;
          }
      }
    }
}
