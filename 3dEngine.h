#ifndef D_ENGINE_H
#include <math.h>
#include "render.h"

#define D_ENGINE_H

struct Player {
    int x;
    int y;
    float angle;
};

void cast_ray(int (*map)[10], struct Player* player, int angle, int col, int rows);

#endif

