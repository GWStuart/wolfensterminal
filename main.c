#include <ncurses.h>
#include <unistd.h>
#include "player_info.h"
#include "render.h"

// add rendering stuff here
void render_screen(Player_info* player) {
    clear_screen();

    // Player_info* player = init_player_info(5, 5, 0, 1);

    // do the actual rendering
    //render_line(5, 10, 10, '@');
    //render_line(15, 15, 5, '@');
    //render_line(100, 30, 30, '@');

    render_rect(player->x, player->y, 4, 2, '#');

    refresh();
}

int main() {
    Player_info* player = init_player_info(0, 0, 0, 1);

    // setup the screen
    setup_screen();

    for(int i = 0; i < 10; i++) {  // move 10 steps
        clear_screen();

        update_player_pos(player);   // move player forward
        acceleration(player);
        render_screen(player);
        update_player_pos(player);   // move player forward

        refresh();
        usleep(100000);  // 0.1s per frame
    }

    for(int i = 0; i < 30; i++) {  // move 10 steps
        clear_screen();

        update_player_pos(player);   // move player forward
        deceleration(player);
        render_screen(player);
        update_player_pos(player);   // move player forward


        refresh();
        usleep(100000);  // 0.1s per frame
    }
    
    free(player);
    endwin();
    return 0;
}
