#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_SPEED 100
#define MIN_SPEED 1
#define PI 3.14

int stdin_angle = 0; //placeholder, will replace with actual angle given by stdin
double stdin_distance = 0;

typedef struct{
    int postion_x;
    int position_y; 
    int angle_degrees;
    int speed;
} Player_info;

Player_info* init_player_info(){
    Player_info* P_info = malloc(sizeof(Player_info));
    P_info->position_y = 0;
    P_info->postion_x = 0;
    P_info->angle_degrees = 0;
    P_info->speed = 0;

    return P_info;
}

void move_player_direction(Player_info* P_info, int stdin_angle, double stdin_distance){
    P_info->angle_degrees = stdin_angle;
    double angle_radians = ((P_info->angle_degrees) * PI) / 180;

    double delta_x = stdin_distance*cos(angle_radians);
    double delta_y = stdin_distance*sin(angle_radians);

    P_info->postion_x += delta_x;
    P_info->position_y += delta_y;
    


}

int main(void){
    Player_info* P_info = init_player_info();
    move_player_direction(P_info, stdin_angle, stdin_distance);

}