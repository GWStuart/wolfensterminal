#include <ncurses.h>

void setup_screen() {
    initscr();
    noecho();  // stop typed characters from showing up
    curs_set(0); // hide cursor
}

void clear_screen() {
    clear();
    refresh();
}

void render_line(int x, int y, int length, chtype character) {
    for (int i=0; i<length; i++) {
        mvaddch(y + i, x, character);
    }
}

void render_rect(int x, int y, int length, int width, chtype character) {
    for (int row=0; row<width; row++) {
        mvhline(y + row, x, character, length);
    }
}
