#ifndef WOLFENSTERMINAL_CONFIG_H
#define WOLFENSTERMINAL_CONFIG_H

/*
Define all structs and things like that here.
Makes things easier so that all files have access to this.

Also useful to define any macros that you need here

*/

#define MAX_SPEED 100
#define MIN_SPEED 1
#define PI 3.14

typedef struct{
    int x;
    int y; 
    float angle;
    int speed;
} Player_info;


#endif
