#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "render.h"

#include "config.h"

Player_info* init_player_info(int fazell_angle, int fazell_pos_x, int fazell_pos_y, int fazell_max_speed, int fazell_curr_speed);

void update_player_pos(Player_info* P_info);

void acceleration(Player_info* P_info);

void deceleration(Player_info* P_info);

#endif
