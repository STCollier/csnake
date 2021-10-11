#ifndef FOOD_H
#define FOOD_H

#include <SDL.h>

typedef struct Food {
    float x, y;
} Food;

void draw_food(void);
void spawn_food(void);

#endif