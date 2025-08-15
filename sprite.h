#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>

typedef struct {int x;
    int y;
    bool isAngled;
    int angle;
    int distanceToPlayer;
} Sprite;

#endif
