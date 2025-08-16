#include "player_info.h"
#include "config.h"
#include "input.h"
#define TO_RAD(deg) (deg * (M_PI / 180.0f))
#define TO_DEG(rad) (rad * (180.0f / M_PI))

int fazell_angle = 0; //placeholder, will replace with actual values given by fazell
int fazell_pos_x = 0;
int fazell_pos_y = 0;
int fazell_speed = 1;
bool keysPressed = true;

Player_info* init_player_info(int fazell_angle, int fazell_pos_x, int fazell_pos_y, 
    int faz_curr_speed, int faz_max_speed){
    //this just takes all of fazell's values, and updates the struct
    Player_info* P_info = malloc(sizeof(Player_info));
    P_info->y = fazell_pos_y;
    P_info->x = fazell_pos_x;
    P_info->angle= fazell_angle;
    P_info->curr_speed = faz_curr_speed;
    P_info->max_speed = faz_max_speed;

    return P_info;
}
/*
void update_player_pos(Player_info* P_info){
    double angle_radians = ((P_info->angle) * M_PI) / 180;

    //calculates movement of x and y combined, with mouse movement(cos/sin)
    double delta_x = (P_info->speed)*cos(angle_radians);
    double delta_y = (P_info->speed)*sin(angle_radians);

    //updates player position
    P_info->x += delta_x;
    P_info->y += delta_y;

}
*/

void acceleration(Player_info* player, Inputs* inputs) {
    int target_speed = 0; //default is 0, will decelerate into 0 if theres no input
    if (inputs->forward || inputs->back || inputs->left || inputs->right) {
        player->curr_speed = 1;
        target_speed = player->max_speed; // Use a single max speed for simplicity
    }

    float accel_factor = 0;
    if(target_speed != 0){
        accel_factor = 1.05; 
    } 
    else{
        accel_factor = 0.001;
    }
   

    if (inputs->forward) {
	    player->x += player->curr_speed*cos(TO_RAD(player->angle))*accel_factor;
	    player->y += player->curr_speed*sin(TO_RAD(player->angle))*accel_factor;
    }
    if (inputs->back) {
	    player->x -= player->curr_speed*cos(TO_RAD(player->angle))*accel_factor;
	    player->y -= player->curr_speed*sin(TO_RAD(player->angle))*accel_factor;
    }
    if (inputs->left) {
	    player->x += player->curr_speed*sin(TO_RAD(player->angle))*accel_factor;
	    player->y -= player->curr_speed*cos(TO_RAD(player->angle))*accel_factor;
    }
    if (inputs->right) {
	    player->x -= player->curr_speed*sin(TO_RAD(player->angle))*accel_factor;
	    player->y += player->curr_speed*cos(TO_RAD(player->angle))*accel_factor;
    }
}