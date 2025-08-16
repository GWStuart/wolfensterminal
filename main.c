#include <ncurses.h>
#include "player_info.h"
#include "render.h"
#include "map_editor.h"

int main() {
    // setup the screen
    setup_screen();

    //Player_info* player = init_player_info(0, 0, 0, 1);

    Level* level = create_level(10, 10);

    map_edit(level);

    endwin();
    return 0;
}

//to compile gcc -o map_editor main.c map_editor.c render.c render.h -lm -lncurses. NOTE: I think render.h has been changed in main.