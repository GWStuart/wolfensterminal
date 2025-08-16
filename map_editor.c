#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_FILE_NAME 256

typedef struct {
    int width;
    int height;
    char** grid;
} Level;


typedef struct {
    char key;
    const char *name;
} EditorMode;

int get_filename(char *buf, size_t size) {
    if (fgets(buf, size, stdin) == NULL) return 0;
    // remove newline
    char *nl = strchr(buf, '\n');
    if (nl) *nl = '\0';
    return 1;
}

int save_level(const char* path, Level* level) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;

    for (int i = 0; i < level->height; i++) {
        for (int j = 0; j < level->width; j++) {
            fputc(level->grid[i][j], f);
        }
        fputc('\n', f);
    }

    fclose(f);
    return 1;
}

// Load a level from a file
Level* load_level(const char* path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    // Determine dimensions
    int width = 0, height = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') height++;
        else if (height == 0) width++;
    }
    if (width == 0 || height == 0) {
        fclose(f);
        return NULL;
    }

    rewind(f);

    Level* level = malloc(sizeof(Level));
    level->width = width;
    level->height = height;
    level->grid = malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        level->grid[i] = malloc(width * sizeof(char));
        for (int j = 0; j < width; j++) {
            int c = fgetc(f);
            if (c == EOF || c == '\n') break;
            level->grid[i][j] = (char)c;
        }
        fgetc(f); // consume newline
    }

    fclose(f);
    return level;
}

Level* create_level(int rows, int cols) {
    Level* level = malloc(sizeof(Level));
    level->width = cols;
    level->height = rows;

    level->grid = malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        level->grid[i] = malloc(cols * sizeof(char));
        for (int j = 0; j < cols; j++) {
            level->grid[i][j] = (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) ? '1' : '0';
        }
    }
    return level;
}


void draw_level(Level* level) {
    for (int i = 0; i < level->height; i++) {
        for (int j = 0; j < level->width; j++) {
            addch(level->grid[i][j]);
            addch(' ');
        }
        addch('\n');
    }
}

void draw_hud(Level* level, char editor_mode) {
    EditorMode modes[] = {
        {'1', "Floor"},
        {'2', "Wall"},
        {'3', "Door"},
        {'4', "Enemy"},
        {'5', "Gun"},
        {'6', "Trap"},
        {'x', "Insert / Move Player Character"}
    };
    int num_modes = 7;

    clear();
    draw_level(level);

    int normal_attr = A_BOLD;
    int highlight_attr = A_STANDOUT;

    printw("Map Element Mode (Press Key to Change Map Element Mode):\n");

    for (int i = 0; i < num_modes; i++) {
        if (editor_mode == modes[i].key) attron(highlight_attr);
        else attron(normal_attr);

        printw("%c: %s ", modes[i].key, modes[i].name);
        attroff(highlight_attr | normal_attr);
    }
    printw("\n");

    if (editor_mode == 'l') {
        char file_name[MAX_LENGTH_FILE_NAME];
        mvprintw(level->height + 2, 0, "Filename (Ctrl+G to exit): ");
        get_filename(file_name, sizeof(file_name)); // user-defined

        char path[MAX_LENGTH_FILE_NAME];
        snprintf(path, sizeof(path), "Levels/%s.txt", file_name);

        Level* loaded = load_level(path); // user-defined
        if (loaded) {
            // Free old grid
            for (int i = 0; i < level->height; i++) free(level->grid[i]);
            free(level->grid);

            *level = *loaded; // shallow copy of struct; grid pointer transferred
            free(loaded);     // free struct itself but not grid
            clear();
            draw_level(level);
            mvprintw(level->height + 1, 0, "File loaded from \"%s\" (press any key)", path);
            getch();
        } else {
            mvprintw(level->height + 1, 0, "File \"%s\" does not exist (press any key)", path);
            getch();
        }
    }

    // Save
    if (editor_mode == 's') {
        char file_name[MAX_LENGTH_FILE_NAME];
        mvprintw(level->height + 2, 0, "Filename (Ctrl+G to exit): ");
        get_filename(file_name, sizeof(file_name)); // user-defined

        char path[MAX_LENGTH_FILE_NAME];
        snprintf(path, sizeof(path), "Levels/%s.txt", file_name);

        if (save_level(path, level)) { // user-defined
            mvprintw(level->height + 1, 0, "File saved in %s (press any key)", path);
        } else {
            mvprintw(level->height + 1, 0, "Failed to save %s (press any key)", path);
        }
        getch();
    }
}

const char* map_elem_mode(int ch) {
    switch (ch) {
        case '1': return "1";  // floor
        case '2': return "2";  // wall
        case '3': return "3";  // door
        case '4': return "4";  // enemy
        case '5': return "5";  // gun
        case '6': return "6";  // trap
        case 'l': return "l";  // load
        case 's': return "s";  // save
        case 'x': return "x";  // player mode
        default:  return NULL; // invalid key
    }
}

void map_edit(Level *level) {
    char editor_mode = '0';
    bool dragging = false;
    bool drag_enabled = false;
    int last_x = -1, last_y = -1;

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_YELLOW);
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    printf("\033[?1003h\n"); // enable mouse tracking

    while (1) {
        clear();

        // Draw buttons panel without interfering with mouse
        int panel_y = level->height + 5;
        mvprintw(panel_y, 0, "| Press S to Save Game |");
        mvprintw(panel_y + 1, 0, "| Press L to Load Game |");
        mvprintw(panel_y + 2, 0, "| Press Q to Quit |");

        draw_hud(level, editor_mode);
        mvprintw(level->height + 3, 0, "Drag mode: %s (Press 'p' to toggle, highlight over char to overlap)", drag_enabled ? "ON" : "OFF");
        
        
        
        refresh();

        int event = getch();
        if (event == 'q') {
            break;
        } else if (event == 'p') {
            drag_enabled = !drag_enabled;
            continue;
        } else if (event == KEY_MOUSE) {
            MEVENT click;
            if (getmouse(&click) == OK) {
                // Calculate grid coordinates
                int mouse_x = click.x / 2;  // Each cell takes 2 screen columns
                int mouse_y = click.y;      // Each cell takes 1 screen row
                
                // Check if click is within the grid
                if (mouse_x >= 0 && mouse_x < level->width && 
                    mouse_y >= 0 && mouse_y < level->height) {
                    
                    if (click.bstate & BUTTON1_PRESSED) {
                        dragging = true;
                        // Handle immediate click (not just drag)
                        if (editor_mode == '4') {
                            // Clear existing enemy if placing new one
                            for (int i = 0; i < level->height; i++) {
                                for (int j = 0; j < level->width; j++) {
                                    if (level->grid[i][j] == '4') {
                                        level->grid[i][j] = '0';
                                    }
                                }
                            }
                        }
                        level->grid[mouse_y][mouse_x] = editor_mode;
                        last_x = mouse_x;
                        last_y = mouse_y;
                    }
                    
                    if (dragging && drag_enabled && 
                        (editor_mode >= '1' && editor_mode <= '6')) {
                        if (mouse_x != last_x || mouse_y != last_y) {
                            level->grid[mouse_y][mouse_x] = editor_mode;
                            last_x = mouse_x;
                            last_y = mouse_y;
                        }
                    }
                }
                
                if (click.bstate & BUTTON1_RELEASED) {
                    dragging = false;
                    last_x = -1;
                    last_y = -1;
                }
            }
        } else {
            const char* mode = map_elem_mode(event);
            if (mode) editor_mode = mode[0];
        }
    }

    printf("\033[?1003l\n"); // disable mouse tracking
}
