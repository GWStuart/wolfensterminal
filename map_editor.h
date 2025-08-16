#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#include <ncurses.h>
#include <time.h>

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

Level* create_level(int rows, int cols);

void draw_level(Level* level);

void draw_hud(Level* level, char editor_mode);

const char* map_elem_mode(int ch);

void map_edit(Level* level);

int get_filename(char* buffer, int buffer_size); 

Level* load_level(const char* path);             

int save_level(const char* path, Level* level); 

#endif