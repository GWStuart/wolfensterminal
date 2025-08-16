#include <ncurses.h>
#include "player_info.h"
#include "render.h"
#include "map_editor.h"


// add rendering stuff here
void render_screen(Player_info* player) {
    clear_screen();

    // Player_info* player = init_player_info(5, 5, 0, 1);

    // do the actual rendering
    //render_line(5, 10, 10, '@');
    //render_line(15, 15, 5, '@');
    //render_line(100, 30, 30, '@');

    render_rect(player->position_x, player->position_y, 4, 2, '#');

    refresh();
}

int main() {
    // setup the screen
    setup_screen();

    Player_info* player = init_player_info(0, 0, 0, 1);

    Level* level = create_level(20, 20);

    map_edit(level);



    for (int i = 0; i < 20; i++) {
        clear_screen();

        update_player_pos(player);
        acceleration(player);

        draw_level(level);         // draw the edited map
        render_screen(player);     // draw the player on top

        refresh();
    }

    // Clean up
    free(player);
    for (int i = 0; i < level->height; i++) free(level->grid[i]);
    free(level->grid);
    free(level);

    endwin();
    return 0;
}
