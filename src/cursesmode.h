#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <curses.h>

void SetCursorPosition(WINDOW* activeWindow, int targetY, int targetX);
void RestoreCursorPosition();

///
/// @brief Gets user input, with command history and line editing.
/// @param out Reference to string that will be filled with user input.
bool checkPrintable(int test);


#endif