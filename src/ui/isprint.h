#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <curses.h>
#include <string>

bool isPrintKey(int test);
void ResizeTerminals();
bool isPrintStr(std::string);

#endif