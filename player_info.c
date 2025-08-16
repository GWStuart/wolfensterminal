#include "player_info.h"
#include "config.h"


int fazell_angle = 0; //placeholder, will replace with actual angle given by fazell
int fazell_pos_x = 0;
int fazell_pos_y = 0;
int fazell_speed = 1;
bool keysPressed = true;

Player_info* init_player_info(int fazell_angle, int fazell_pos_x, int fazell_pos_y, int fazell_max_speed, int fazell_curr_speed ){
    //this just takes all of fazell's values, and updates the struct
    Player_info* P_info = malloc(sizeof(Player_info));
    P_info->y = fazell_pos_y;
    P_info->x = fazell_pos_x;
    P_info->angle= fazell_angle;
    P_info->max_speed = fazell_max_speed;
    P_info->curr_speed = fazell_curr_speed;

    return P_info;
}

void update_player_pos(Player_info* P_info){
    double angle_radians = ((P_info->angle) * M_PI) / 180;

    //calculates movement of x and y combined, with mouse movement(cos/sin)
    double delta_x = (P_info->speed)*cos(angle_radians);
    double delta_y = (P_info->speed)*sin(angle_radians);

    //updates player position
    P_info->x += delta_x;
    P_info->y += delta_y;

}

void acceleration(Player_info* player, Inputs* inputs){
    //max_speed should be updated with keys pressed. curr_speed should be initialised to be 0 at the start
    
    if (inputs->forward && player->curr_speed != player->max_speed && keysPressed) {
        player->curr_speed += 0.005;
	    player->x += player->curr_speed*cos(TO_RAD(player.angle));
	    player->y += player->curr_speed*sin(TO_RAD(player.angle));
    }
    if (inputs.back && player->curr_speed != player->max_speed && keysPressed) {
        player->curr_speed += 0.005;
	    player->x -= player->curr_speed*cos(TO_RAD(player.angle));
	    player->y -= player->curr_speed*sin(TO_RAD(player.angle));
    }
    if (inputs.left && player->curr_speed != player->max_speed && keysPressed) {
        player->curr_speed += 0.005;
	    player->x += player->curr_speed*sin(TO_RAD(player.angle));
	    player->y -= player->curr_speed*cos(TO_RAD(player.angle));
    }
    if (inputs.right && player->curr_speed != player->max_speed && keysPressed) {
        player->curr_speed += 0.005;
	    player->x -= player->curr_speed*sin(TO_RAD(player.angle));
	    player->y += player->curr_speed*cos(TO_RAD(player.angle));
    }
    
}

void deceleration(Player_info* P_info){
    //if moving, no inputs, decelerate, does not depend on any key presses
    if (P_info->curr_speed != 0 && !keysPressed){
        P_info->curr_speed -= 0.1; 
    }
   

}

/*  
    if (inputs.forward) {
	player.x += 5*cos(TO_RAD(player.angle));
	player.y += 5*sin(TO_RAD(player.angle));
    }
    if (inputs.back) {
	player.x -= 5*cos(TO_RAD(player.angle));
	player.y -= 5*sin(TO_RAD(player.angle));
    }
    if (inputs.left) {
	player.x += 5*sin(TO_RAD(player.angle));
	player.y -= 5*cos(TO_RAD(player.angle));
    }
    if (inputs.right) {
	player.x -= 5*sin(TO_RAD(player.angle));
	player.y += 5*cos(TO_RAD(player.angle));
    }
*/