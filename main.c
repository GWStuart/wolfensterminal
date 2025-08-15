#include <ncurses.h>
#include "render.h"

int main() {
    // setup the screen
    setup_screen();

    clear_screen();

    // some examples of rendering stuff
    render_line(5, 10, 10, '@');
    render_line(15, 15, 5, '@');
    render_line(100, 30, 30, '@');
    render_rect(80, 5, 50, 12, '#');

    refresh();

    getch(); // stall until a key has been pressed

    endwin();

    return 0;
}
