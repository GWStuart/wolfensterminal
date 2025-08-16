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
/*
void acceleration(Player_info* player, Inputs* inputs) {
    int target_speed = 0; //default is 0, will decelerate into 0 if theres no input
    if (inputs->forward || inputs->back || inputs->left || inputs->right) {
        player->curr_speed = 1;
        target_speed = player->max_speed; // Use a single max speed for simplicity
    }

    float accel_factor = 0;
    if(target_speed != 0){
        accel_factor = 1.10; 
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
*/
// This function replaces your entire existing function.
// It assumes you have a 'curr_speed' float in your Player_info struct.
void acceleration(Player_info* player, Inputs* inputs) {

    // --- 1. Determine the player's INTENDED direction vector ("wish_dir") ---
    float wish_dir_x = 0.0;
    float wish_dir_y = 0.0;

    if (inputs->forward) {
        wish_dir_x += cos(TO_RAD(player->angle));
        wish_dir_y += sin(TO_RAD(player->angle));
    }
    if (inputs->back) {
        wish_dir_x -= cos(TO_RAD(player->angle));
        wish_dir_y -= sin(TO_RAD(player->angle));
    }
    if (inputs->left) { // CORRECTED strafe logic
        wish_dir_x += -sin(TO_RAD(player->angle));
        wish_dir_y += cos(TO_RAD(player->angle));
    }
    if (inputs->right) { // CORRECTED strafe logic
        wish_dir_x += sin(TO_RAD(player->angle));
        wish_dir_y += -cos(TO_RAD(player->angle));
    }

    // --- 2. Normalize the direction vector to prevent diagonal speed boost ---
    float dir_length = sqrt(wish_dir_x * wish_dir_x + wish_dir_y * wish_dir_y);
    if (dir_length > 0.0) {
        wish_dir_x /= dir_length;
        wish_dir_y /= dir_length;
    }

    // --- 3. Handle smooth acceleration and deceleration ---
    // Determine the target speed. If any key is pressed, target is max_speed, otherwise it's 0.
    float target_speed = (dir_length > 0.0) ? player->max_speed : 0.0;
    
    // Use interpolation (lerp) for smooth speed changes.
    // A higher factor means faster acceleration.
    float accel_factor = 0.05; // Adjust this value to change how fast you accelerate
    player->curr_speed += (target_speed - player->curr_speed) * accel_factor;

    // --- 4. ALWAYS update the player's position using the final speed and direction ---
    // (Ideally, you would multiply this by a delta_time value for frame-rate independence)
    player->x += wish_dir_x * player->curr_speed;
    player->y += wish_dir_y * player->curr_speed;
}