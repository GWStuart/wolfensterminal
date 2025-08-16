#include <ncurses.h>
#include <unistd.h>
#include "player_info.h"
#include "render.h"
#include <stdbool.h>

// add rendering stuff here
void render_screen() {
    clear_screen();

    // do the actual rendering
    for (int i=0; i<14; i++) {
        render_player(5 + i, 2, i, 50);
    }

    refresh();
}

int main() {
    // setup the screen
    setup_screen();

    render_screen();

    getch();

    endwin();
    return 0;
}
