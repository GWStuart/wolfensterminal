#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "render.h"

#define MAX_SPEED 100
#define MIN_SPEED 1
#define PI 3.14

typedef struct{
    int x;
    int y; 
    int angle;
    int speed;
} Player_info;

Player_info* init_player_info(int fazell_angle, int fazell_pos_x, int fazell_pos_y, float fazell_speed);

void update_player_pos(Player_info* P_info);

void acceleration(Player_info* P_info);

void deceleration(Player_info* P_info);

#endif
