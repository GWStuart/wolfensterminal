#include "player_info.h"
#include "config.h"


int fazell_angle = 0; //placeholder, will replace with actual values given by fazell
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
    
    // In your Player_info struct, replace curr_speed with velocity:
// float velocity_x;
// float velocity_y;

void acceleration(Player_info* player, Inputs* inputs) {
    
    // 1. Determine the player's desired direction based on input.
    // This creates a "wish direction" vector.

    if (inputs->forward) {
        player->x += cos(TO_RAD(player->angle));
        player->y += sin(TO_RAD(player->angle));
    }
    if (inputs->back) {
        player->x -= cos(TO_RAD(player->angle));
        player->y -= sin(TO_RAD(player->angle));
    }
    if (inputs->left) { // Corrected strafe logic
        player->x += -sin(TO_RAD(player->angle));
        player->y += cos(TO_RAD(player->angle));
    }
    if (inputs->right) { // Corrected strafe logic
        player->x += sin(TO_RAD(player->angle));
        player->y += -cos(TO_RAD(player->angle));
    }

    // Normalize the wish_dir vector if it's not zero, so diagonal movement isn't faster.
    // (This step is important for quality movement)

    // 2. Determine the target speed.
    int target_speed = 0; //default is 0, will decelerate into 0 if theres no input
    if (inputs->forward || inputs->back || inputs->left || inputs->right) {
        int target_speed = player->max_speed; // Use a single max speed for simplicity
    }

    // 3. Calculate the target velocity vector.
    int target_velocity_x = player->x * target_speed;
    int target_velocity_y = player->y * target_speed;

    // 4. Accelerate the player's CURRENT velocity towards the TARGET velocity.
    // This uses interpolation (lerp) for smooth acceleration/deceleration.
    // A higher 'acceleration_factor' means faster acceleration.
    float accel_factor = 1.05; 
    player->curr_speed_x += (target_velocity_x - player->curr_speed_x) * accel_factor;
    player->curr_speed_y += (target_velocity_y - player->curr_speed_y) * accel_factor;

    // 5. ALWAYS update the player's position using the current velocity.
    // This is the fix for the critical deceleration bug.
    // (Ideally, multiply by delta_time for frame-rate independence)
    player->x += player->curr_speed_x;
    player->y += player->curr_speed_y;
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