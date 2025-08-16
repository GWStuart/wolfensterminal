//compile with:
//gcc -o fazMain fazMain.c 3dEngine.c render.c input.c player_info.c -lm -lncurses


#include "3dEngine.h"
#include "render.h"
#include "input.h"
#include "player_info.h"
#include "sprite.h"
#include "debug/debug.h"

#include <unistd.h>
#include <math.h>
#include <stdbool.h>

#define TO_RAD(deg) (deg * (M_PI / 180.0f))
#define TO_DEG(rad) (rad * (180.0f / M_PI))

    int map[10][10] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 4}, 
	{3, 3, 3, 3, 3, 3, 3, 3, 3, 3}
    };

int main()
{
    //NEED TO MAKE THIS CODE NOT HARD CODED AND ACTUALLY WORK FOR ALL SCENARIOS
    debug_init();
    Sprite* sprites = malloc(sizeof(Sprite) * 1);
    sprites[0].x = 5*64;
    sprites[0].y = 3*64;
    sprites[0].isAngled = false;
    InputDeviceStuff iDS = open_devices();
    Inputs inputs = {.forward = false,
	.back = false,
	.left = false,
	.right = false,
	.tRight = false,
	.tLeft = false,
	.mouseX = 0};
    setup_screen();
    clear_screen();
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);

    Player_info player = {.x = 5*64, .y = 5*64, .angle = -90, .curr_speed = 0, .max_speed = 5};
    //-90 is top
    //int map[10][10] = {
    //    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    //};
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    //wmove(stdscr, 0, 0);
    while (1) {
	getmaxyx(stdscr, rows, cols);

	clear_screen();

	float FOV = 70.0f; // degrees
	float FOV_RAD = FOV * (M_PI / 180.0f);

	draw_all_stuff(map, &player, cols, rows, &sprites);  // Pass exact screen column


//
//    for (int i = cols/2; i > -cols/2; i -= cols/70) {
//	cast_ray(map, &player, player.angle + i);
//    }
    //for (int i = -cols/2; i < cols/2; i++) {
    //    cast_ray(map, &player, player.angle + i);
    //}
    refresh();
    //player.angle++;
    //inputs.mouseX = 0;
    detect_input(&iDS, &inputs);
    debug_print("mouseX: %d\n", inputs.mouseX);
    player.angle += inputs.mouseX; //UNCOMMENT IF MOUSE IS WORKING AS INTENDED
    if (player.angle < -180) {
	player.angle += 360;
    } else if (player.angle > 180) {
	player.angle -= 360;
    }
    inputs.mouseX = 0;
    //acceleration(&player, &inputs);
    
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
    
    

    if (inputs.tLeft) {
	player.angle--;
	if (player.angle < -180) {
	    player.angle = 180;
	}
    }
    if (inputs.tRight) {
	player.angle++;
	if (player.angle > 180) {
	    player.angle = -180;
	}
    }
    refresh();
    napms(16);
	//player.y--;
	//sleep(1);
    }
    getch();
    return 0;
}
