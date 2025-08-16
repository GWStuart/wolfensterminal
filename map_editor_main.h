#ifndef MAP_EDITOR_MAIN_H
#define MAP_EDITOR_MAIN_H

#include <ncurses.h>
#include "player_info.h"
#include "render.h"
#include "map_editor.h"

void render_screen(Player_info* player);

void main_editor(void);

#endif