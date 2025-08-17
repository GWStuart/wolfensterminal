#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include <stdbool.h>

#include "config.h"

// does some ncurses setup
// (see render.c for details)
void setup_screen();

// clear the screen (your probably could've guessed that)
void clear_screen();

/*
Draw an ASCII vertical line at (x,y) and going down with the specified text
*/
void render_line(int x, int y, int length, chtype character, int colour);

/*
Renders a rectangle with the top-left corner at the specified x and y
*/
void render_rect(int x, int y, int length, int width, chtype character);

/*
Renders a line with the specified texture
(still a work in progress)

pos specifies the position along the texture (should be an integer from 0 - 32)
textureNumber specifies which texture to use (just pass 1 for now)
*/
void render_line_texture(int x, int y, int length, int pos, int textureNumber, bool darker, float scaleFactor, float scaleOffset);

void render_player(int x, int y, int column, int height);

void render_shotgun(int x, int y, int column, int height);

void draw_hand_gun(Player_info* player, int rows, int cols);
#endif
