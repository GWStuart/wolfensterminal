#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "render.h"
#include "input.h"

#include "config.h"

Player_info* init_player_info(int fazell_angle, int fazell_pos_x, int fazell_pos_y, int faz_curr_speed, int faz_max_speed);

void update_player_pos(Player_info* P_info);

void acceleration(Player_info* P_info, Inputs* inputs);

#endif
