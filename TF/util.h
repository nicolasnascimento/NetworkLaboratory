#ifndef UTIL_H
#define UTIL_H

/// Gets the initial flags from the Command Line
/// This should be called right after the program initializes
void get_initial_flags(int argc, char** argv);

/// Always use this print, which is only enabled when verbose mode is enabled.
void d_printf(char* format, ...);


#endif // UTIL_H
