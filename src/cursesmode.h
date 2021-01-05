#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <curses.h>
#include <vector>
#include <sstream>

#define ncout(message)                                    \
    {                                                     \
        getyx(root, curY, curX);                          \
        if (curY == getmaxy(root))                        \
            scroll(root);                                 \
        logger.stringBuilder.str(string());               \
        logger.stringBuilder << message;                  \
        printw("%s", logger.stringBuilder.str().c_str()); \
        refresh();                                        \
    }

#define ncoutln(message)        \
    {                           \
        ncout(message << endl); \
    }

void ngetstr(std::string &var);

extern std::vector<std::string> lastCommand;

#endif