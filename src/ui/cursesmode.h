#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <string>
#include <curses.h>

void SetCursorPosition(WINDOW* activeWindow, int targetY, int targetX);
void RestoreCursorPosition();
bool checkPrintable(int test);
void ResizeTerminals();
bool isPrintStr(std::string);

#endif