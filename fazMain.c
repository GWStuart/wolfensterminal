//compile with:
//gcc -o fazMain fazMain.c 3dEngine.c render.c input.c player_info.c -lm -lncurses


#include "3dEngine.h"
#include "render.h"
#include "input.h"
#include "player_info.h"

#include <unistd.h>
#include <math.h>

#define TO_RAD(deg) (deg * (M_PI / 180.0f))

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

    Player_info player = {.x = 5*64, .y = 5*64, .angle = -90, .speed = 1.0f};
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

	for (int col = 0; col < cols; col++) {
	    float rayAngle = (player.angle - (FOV / 2.0f)) + ((float)col / cols) * FOV;
	    cast_ray(map, &player, rayAngle, col, rows);  // Pass exact screen column
	}


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
    wmove(stdscr, 0, 0);
    printw("%d", inputs.mouseX);
    player.angle += inputs.mouseX; //UNCOMMENT IF MOUSE IS WORKING AS INTENDED
    inputs.mouseX = 0;
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
    }
    if (inputs.tRight) {
	player.angle++;
    }
    refresh();
    napms(16);
	//player.y--;
	//sleep(1);
    }
    getch();
    return 0;
}
