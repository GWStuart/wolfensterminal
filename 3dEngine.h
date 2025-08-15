#ifndef D_ENGINE_H
#include <math.h>
#include "render.h"
#include "player_info.h"
#include "sprite.h"

#define D_ENGINE_H

struct Player {
    int x;
    int y;
    float angle;
};

//float cast_ray(int (*map)[10], Player_info* player, int angle, int col, int rows);

float draw_all_stuff(int (*map)[10], Player_info* player, int cols, int rows, Sprite** sprites);
#endif

