#ifndef DEBUG_H
#define DEBUG_H

// initialises a debug file at log/debug.out
void debug_init();

// closes the debug file
void debug_close();

// prints what you want to the log file
// if the log file doesn't exist it should create it.
// (supports using regular print format specifiers).
void debug_print(const char *fmt, ...);

#endif
