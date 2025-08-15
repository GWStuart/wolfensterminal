#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_SPEED 100
#define MIN_SPEED 1
#define PI 3.14

int fazell_angle = 0; //placeholder, will replace with actual angle given by stdin
int fazell_pos_x = 0;
int fazell_pos_y = 0;
int fazell_speed = 1;

typedef struct{
    int position_x;
    int position_y; 
    int angle_degrees;
    int speed;
} Player_info;

Player_info* init_player_info(int fazell_angle, int fazell_pos_x, int fazell_pos_y, int fazell_speed){
    //this just takes all of fazell's values, and updates the struct
    Player_info* P_info = malloc(sizeof(Player_info));
    P_info->position_y = fazell_pos_y;
    P_info->position_x = fazell_pos_x;
    P_info->angle_degrees = fazell_angle;
    P_info->speed = fazell_speed;

    return P_info;
}

void update_player_pos(Player_info* P_info){
    double angle_radians = ((P_info->angle_degrees) * PI) / 180;

    //calculates movement of x and y combined, with mouse movement(cos/sin)
    double delta_x = (P_info->position_x)*cos(angle_radians);
    double delta_y = (P_info->position_y)*sin(angle_radians);

    //updates player position
    P_info->position_x += delta_x;
    P_info->position_y += delta_y;

}

void acceleration(Player_info* P_info){
    
    for(int i = P_info->speed; i<MAX_SPEED; i++){
        P_info->speed += 10; //increases by 10 each iteration it is not at max speed
    }
    
}

void deceleration(Player_info* P_info){
    for (int i = P_info->speed; i>MIN_SPEED; i--){
        P_info->speed -= 10;
    } 
}

int main(void){
    Player_info* P_info = init_player_info(fazell_angle, fazell_pos_x, fazell_pos_y, fazell_speed);
    update_player_pos(P_info);

}