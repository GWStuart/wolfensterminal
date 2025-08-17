#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>

#define S_GUY 0
#define S_SGUN 1


typedef struct {int x;
    int y;
    bool isAngled;
    int angle;
    int distanceToPlayer;
    int spriteType;
    bool isExist;
} Sprite;

#endif
