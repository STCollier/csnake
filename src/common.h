#ifndef COMMON_H
#define COMMON_H

#define SCREEN_WIDTH   400
#define SCREEN_HEIGHT  400
#define WALL_WIDTH      20
#define WALL_HEIGHT     20
#define CELL_WIDTH      20
#define CELL_HEIGHT     20
#define CELL_COUNT     ((SCREEN_WIDTH-WALL_WIDTH*2)*     \
                        (SCREEN_HEIGHT-WALL_HEIGHT*2))/  \
                        (CELL_WIDTH*CELL_HEIGHT)


#endif
