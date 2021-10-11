#ifndef SNAKE_H
#define SNAKE_H

#include "common.h"

enum Direction {UP = 1, DOWN, LEFT, RIGHT};

typedef struct BodyPart {
    int x, y, visible;
} BodyPart;

typedef struct Snake {
    BodyPart body[CELL_COUNT];
    int dx;
    int dy;
} Snake;

Snake snake_init(void);
void move_snake(void);
void draw_snake(void);
void change_direction(enum Direction new_direction);
int is_dead(void);
int hit_food(void);

#endif