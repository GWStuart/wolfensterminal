#include <ncurses.h>


// clear the screen (your probably could've guessed that)
void clear_screen() {
    clear();
    refresh();
}

/*
Draw an ASCII vertical line at (x,y) and going down with the specified text
*/
void render_line(int x, int y, int length, chtype character) {
    for (int i=0; i<length; i++) {
        mvaddch(y + i, x, character);
    }
}

/*
Renders a rectangle with the top-left corner at the specified x and y
*/
void render_rect(int x, int y, int length, int width, chtype character) {
    for (int row=0; row<width; row++) {
        mvhline(y + row, x, character, length);
    }
}

int main() {
    // setup the screen
    initscr();
    noecho();  // stop typed characters from showing up
    curs_set(0); // hide cursor

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

