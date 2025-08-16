#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LEVEL_DIR "Levels/"
#define MAX_LENGTH_FILE_NAME 256
#define LEVEL_DIR "Levels/"
#define LEVEL_EXT ".txt"
#define MAX_PATH_LENGTH (sizeof(LEVEL_DIR) - 1 + MAX_LENGTH_FILE_NAME + sizeof(LEVEL_EXT) - 1 + 1)

// ... (Level and EditorMode structs remain the same) ...
typedef struct {
    int width;
    int height;
    char** grid;
} Level;

typedef struct {
    char key;
    const char *name;
    short colour;
} EditorMode;


// --- NEW NCURSES-SAFE INPUT FUNCTION ---
// This function replaces get_filename and the problematic fgets call.
void get_string_from_user(int y, int x, const char* prompt, char* buf, int size) {
    echo(); // Turn on echoing of characters
    curs_set(1); // Show the cursor
    mvprintw(y, x, "%s", prompt);
    getnstr(buf, size - 1); // ncurses function to get a string safely
    noecho(); // Turn echoing off again
    curs_set(0); // Hide the cursor
}


// ... (save_level, load_level, create_level, draw_level remain the same) ...
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

Level* load_level(const char* path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

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
        fgetc(f);
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

void draw_level(Level* level, EditorMode* modes, int num_modes) {
    for (int i = 0; i < level->height; i++) {
        for (int j = 0; j < level->width; j++) {
            char tile = level->grid[i][j];
            short colour = 0; // default no color

            // find color for this tile
            for (int m = 0; m < num_modes; m++) {
                if (modes[m].key == tile) {
                    colour = modes[m].colour;
                    break;
                }
            }

            if (colour > 0) attron(COLOR_PAIR(colour));
            addch(tile);
            attroff(COLOR_PAIR(colour));

            addch(' ');
        }
        addch('\n');
    }
}


// --- REFACTORED draw_hud ---
// This function now ONLY draws. No more input handling.
void draw_hud(Level* level, char editor_mode, EditorMode* modes, int num_modes) {
    // EditorMode modes[] = {
    //     {'1', "Floor", 1}, {'2', "Wall", 2}, {'3', "Door", 3}, {'4', "Enemy", 4}, {'5', "Gun", 5}, {'6', "Trap", 6}, {'X', "Player Start", 7}
    // };
    // int num_modes = sizeof(modes) / sizeof(modes[0]);

    draw_level(level, modes, num_modes);

    int normal_attr = A_BOLD;
    int highlight_attr = A_STANDOUT;

    mvprintw(level->height + 1, 0, "Map Element Mode (Press Key to Change):\n");

    for (int i = 0; i < num_modes; i++) {
        if (editor_mode == modes[i].key) attron(highlight_attr);
        else attron(normal_attr);

        attron(COLOR_PAIR(modes[i].colour));
        printw("%c: %s ", modes[i].key, modes[i].name);
        attroff(COLOR_PAIR(modes[i].colour));

        attroff(highlight_attr | normal_attr);
    }
    printw("\n");
}

// ... (map_elem_mode remains the same) ...
const char* map_elem_mode(int ch) {
    switch (ch) {
        case '1': return "1"; case '2': return "2"; case '3': return "3";
        case '4': return "4"; case '5': return "5"; case '6': return "6";
        case 'x': return "x";
        default:  return NULL;
    }
}


// --- REFACTORED map_edit ---
// All input logic is now handled here.
void map_edit(Level *level) {
    char editor_mode = '1'; // Start with a valid mode
    bool dragging = false;
    bool drag_enabled = false;
    int last_x = -1, last_y = -1;

    start_color();
    init_pair(1, COLOR_BLACK,   COLOR_WHITE);  // Floor
    init_pair(2, COLOR_BLACK,     COLOR_RED);  // Wall
    init_pair(3, COLOR_BLACK,  COLOR_YELLOW);  // Door
    init_pair(4, COLOR_BLACK,   COLOR_GREEN);  // Enemy
    init_pair(5, COLOR_BLACK,    COLOR_CYAN);  // Gun
    init_pair(6, COLOR_BLACK, COLOR_MAGENTA);  // Trap
    init_pair(7, COLOR_BLACK,    COLOR_BLUE);  // Player Start

    EditorMode modes[] = {
        {'1', "Floor", 1}, 
        {'2', "Wall", 2}, 
        {'3', "Door", 3}, 
        {'4', "Enemy", 4},
        {'5', "Gun", 5}, 
        {'6', "Trap", 6}, 
        {'x', "Player Start", 7}
    };
    int num_modes = sizeof(modes) / sizeof(modes[0]);

    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    printf("\033[?1003h\n"); // enable mouse tracking
    curs_set(0);
    noecho();

    while (1) {
        clear();
        draw_hud(level, editor_mode, modes, num_modes);  // <-- pass modes in

        // Draw static UI elements
        int panel_y = level->height + 5;
        mvprintw(panel_y, 0,     "| (S) Save | (L) Load | (Q) Quit |");
        mvprintw(panel_y + 1, 0, "| (P) Toggle Drag: %s      |", drag_enabled ? "ON " : "OFF");
        
        refresh();

        int event = getch();

        if (event == 'q' || event == 'Q') {
            break;
        } else if (event == 'p' || event == 'P') {
            drag_enabled = !drag_enabled;
        } else if (event == 's' || event == 'S') {
            char file_name[MAX_LENGTH_FILE_NAME] = {0};
            char path[MAX_PATH_LENGTH];

            get_string_from_user(level->height + 3, 0, "Save Filename ('q' + Enter to Quit): ", file_name, sizeof(file_name));
            if (strlen(file_name) > 0) {
                if (strcmp(file_name, "q") == 0) continue;
                snprintf(path, sizeof(path), LEVEL_DIR "%s" LEVEL_EXT, file_name);
                if (save_level(path, level)) {
                    mvprintw(level->height + 4, 0, "Saved to \"%s\". Press any key.", path);
                } else {
                    mvprintw(level->height + 4, 0, "Failed to save to \"%s\". Press any key.", path);
                }
                getch();
            }
        } else if (event == 'l' || event == 'L') {
            char file_name[MAX_LENGTH_FILE_NAME] = {0};
            char path[MAX_PATH_LENGTH];

            get_string_from_user(level->height + 3, 0, "Load Filename ('q' + Enter to Quit): ", file_name, sizeof(file_name));
            if (strlen(file_name) > 0) {
                if (strcmp(file_name, "q") == 0) continue;
                snprintf(path, sizeof(path), LEVEL_DIR "%s" LEVEL_EXT, file_name);
                Level* loaded = load_level(path);
                if (loaded) {
                    for (int i = 0; i < level->height; i++) free(level->grid[i]);
                    free(level->grid);
                    *level = *loaded;
                    free(loaded);
                    mvprintw(level->height + 4, 0, "Loaded from \"%s\". Press any key to view level.", path);
                } else {
                    mvprintw(level->height + 4, 0, "File not found: \"%s\". Press any key.", path);
                }
                getch();
            }
        } else if (event == KEY_MOUSE) {
            MEVENT click;
            if (getmouse(&click) == OK) {
                int mouse_x = click.x / 2;
                int mouse_y = click.y;
                if (mouse_x >= 0 && mouse_x < level->width && mouse_y >= 0 && mouse_y < level->height) {
                    if (click.bstate & BUTTON1_PRESSED) {
                        dragging = true;
                        level->grid[mouse_y][mouse_x] = editor_mode;
                        last_x = mouse_x;
                        last_y = mouse_y;
                    }
                    if (dragging && drag_enabled && (mouse_x != last_x || mouse_y != last_y)) {
                        level->grid[mouse_y][mouse_x] = editor_mode;
                        last_x = mouse_x;
                        last_y = mouse_y;
                    }
                }
                if (click.bstate & BUTTON1_RELEASED) dragging = false;
            }
        } else {
            const char* mode = map_elem_mode(event);
            if (mode) editor_mode = mode[0];
        }
    }

    printf("\033[?1003l\n"); // disable mouse tracking

    // char editor_mode = '1'; // Start with a valid mode
    // bool dragging = false;
    // bool drag_enabled = false;
    // int last_x = -1, last_y = -1;

    // start_color();
    // init_pair(1, COLOR_WHITE, COLOR_BLACK);  // Floor
    // init_pair(2, COLOR_RED, COLOR_BLACK);    // Wall
    // init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Door
    // init_pair(4, COLOR_GREEN, COLOR_BLACK);  // Enemy
    // init_pair(5, COLOR_CYAN, COLOR_BLACK);   // Gun
    // init_pair(6, COLOR_MAGENTA, COLOR_BLACK);// Trap
    // init_pair(7, COLOR_BLUE, COLOR_BLACK);   // Player Start

    // init_pair(1, COLOR_BLUE, COLOR_YELLOW);
    // keypad(stdscr, TRUE);
    // mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    // printf("\033[?1003h\n"); // enable mouse tracking
    // curs_set(0); // Hide cursor by default
    // noecho(); // Don't echo typed characters by default

    // while (1) {
    //     clear();
    //     draw_hud(level, editor_mode, modes, num_modes);

    //     // Draw static UI elements
    //     int panel_y = level->height + 5;
    //     mvprintw(panel_y, 0,     "| (S) Save | (L) Load | (Q) Quit |");
    //     mvprintw(panel_y + 1, 0, "| (P) Toggle Drag: %s      |", drag_enabled ? "ON " : "OFF");
        
    //     refresh();

    //     int event = getch();

    //     // --- Handle all events here in the main loop ---
    //     if (event == 'q' || event == 'Q') {
    //         break;
    //     } else if (event == 'p' || event == 'P') {
    //         drag_enabled = !drag_enabled;
    //     } else if (event == 's' || event == 'S') {
    //         char file_name[MAX_LENGTH_FILE_NAME] = {0};
    //         char path[MAX_PATH_LENGTH];
            
    //         // Get filename using our new safe function
    //         get_string_from_user(level->height + 3, 0, "Save Filename ('q' + Enter to Quit): ", file_name, sizeof(file_name));
            
    //         if (strlen(file_name) > 0) {
    //             if (strcmp(file_name, "q") == 0) {
    //                 continue;    
    //             }
    //             snprintf(path, sizeof(path), LEVEL_DIR "%s" LEVEL_EXT, file_name);
    //             if (save_level(path, level)) {
    //                 mvprintw(level->height + 4, 0, "Saved to \"%s\". Press any key.", path);
    //             } else {
    //                 mvprintw(level->height + 4, 0, "Failed to save to \"%s\". Press any key.", path);
    //             }
    //             getch(); // Pause to show the message
    //         }
    //     } else if (event == 'l' || event == 'L') {
    //         char file_name[MAX_LENGTH_FILE_NAME] = {0};
    //         char path[MAX_PATH_LENGTH];

    //         get_string_from_user(level->height + 3, 0, "Load Filename ('q' + Enter to Quit): ", file_name, sizeof(file_name));

    //         if (strlen(file_name) > 0) {
    //             if (strcmp(file_name, "q") == 0) {
    //                 continue;    
    //             }
    //             snprintf(path, sizeof(path), LEVEL_DIR "%s" LEVEL_EXT, file_name);
    //             Level* loaded = load_level(path);
    //             if (loaded) {
    //                 for (int i = 0; i < level->height; i++) free(level->grid[i]);
    //                 free(level->grid);
    //                 *level = *loaded;
    //                 free(loaded);
    //                 mvprintw(level->height + 4, 0, "Loaded from \"%s\". Press any key to view level.", path);
    //             } else {
    //                 mvprintw(level->height + 4, 0, "File not found: \"%s\". Press any key.", path);
    //             }
    //             getch(); // Pause to show the message
    //         }
    //     } else if (event == KEY_MOUSE) {
    //         MEVENT click;
    //         if (getmouse(&click) == OK) {
    //             int mouse_x = click.x / 2;
    //             int mouse_y = click.y;
                
    //             if (mouse_x >= 0 && mouse_x < level->width && mouse_y >= 0 && mouse_y < level->height) {
    //                 if (click.bstate & BUTTON1_PRESSED) {
    //                     dragging = true;
    //                     level->grid[mouse_y][mouse_x] = editor_mode;
    //                     last_x = mouse_x;
    //                     last_y = mouse_y;
    //                 }
                    
    //                 if (dragging && drag_enabled && (mouse_x != last_x || mouse_y != last_y)) {
    //                     level->grid[mouse_y][mouse_x] = editor_mode;
    //                     last_x = mouse_x;
    //                     last_y = mouse_y;
    //                 }
    //             }
                
    //             if (click.bstate & BUTTON1_RELEASED) {
    //                 dragging = false;
    //             }
    //         }
    //     } else {
    //         const char* mode = map_elem_mode(event);
    //         if (mode) editor_mode = mode[0];
    //     }
    // }

    // printf("\033[?1003l\n"); // disable mouse tracking
}