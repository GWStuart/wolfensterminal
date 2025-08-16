#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static FILE* debug_file = NULL;

void debug_init() {
    debug_file = fopen("logs/debug.out", "w");
}

void debug_close() {
    if (debug_file) {
            fclose(debug_file);
            debug_file = NULL;
    }
}

void debug_print(const char *fmt, ...) {
    if (!debug_file) debug_init();


    // CODE BELOW MADE WITH HELP OF AI
    va_list args;
    va_start(args, fmt);
    vfprintf(debug_file, fmt, args);
    fprintf(debug_file, "\n");
    fflush(debug_file);
    va_end(args);
    // END OF CODE MADE WIHT HELP OF AI
}
