#include <ncurses.h>
#include <unistd.h>
#include "player_info.h"
#include "render.h"

// add rendering stuff here
void render_screen() {
    clear_screen();

    // do the actual rendering
    for (int i=0; i<32; i++) {
        render_line_texture(5 + i, 2, 32, '@', i);
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
