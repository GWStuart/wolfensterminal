#include <ncurses.h>
#include <time.h>

typedef struct {
    int width;
    int height;
    char** grid;
} Level;

typedef struct {
    char key;        // '0', '1', etc.
    const char *name; // Description
} EditorMode;

char** create_level(int x_pos, int y_pos) {
    Level* level = malloc(sizeof(level));

    level->grid = malloc(level->height * sizeof(char *));
    for (int i = 0; i < level->height; i++) {
        level->grid[i] = malloc(level->width * sizeof(char));
        for (int j = 0; j < level->width; j++) {
            if (i == 0 || i == level->height - 1 || j == 0 || j == level->width - 1) {
                level->grid[i][j] = '1';
            } else {
                level->grid[i][j] = '0'; 
            }
        }
    }
    return level;
}

void draw_level(int rows, int cols, char** level[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            addch(level[i][j]);
            addch(' '); 
        }
        addch('\n');
    }
    addch('\n');
}

void draw_hud(int rows, int cols, char** level[rows][cols], const char* editor_mode) {

    EditorMode modes[] = {
        {"1", "Floor"},
        {"2", "Wall"},
        {"3", "Door"},
        {"4", "Enemy"},
        {"5", "Gun"},
        {"6", "Trap"},
        {"0", "Insert / Move Player Character"}
    };

    int num_modes = sizeof(modes) / sizeof(modes[0]);


    clear();
    draw_level(rows, cols, level);

    int normal_attr = A_BOLD;
    int highlight_attr = A_STANDOUT;

    printw("Map Element (Press Key to Change Map Element ): \n");

    for (int i = 0; i < num_modes; i++) {
        int is_active = strcmp(editor_mode, modes[i].key) == 0;

        if (is_active) {
            attron(highlight_attr);
        } else {
            attron(normal_attr);
        }

        printw("%s: %s", modes[i].key, modes[i].name);

        if (i == num_modes - 1 || strcmp(modes[i+1].key, "0") == 0) {
            printw("\n");
        } else {
            printw(" ");
        }

        if (is_active) {
            attroff(highlight_attr);
        } else {
            attroff(normal_attr);
        }
    }
}

