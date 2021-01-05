#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <curses.h>
#include <vector>
#include <sstream>

#define ncout(message)                                    \
    {                                                     \
        getyx(root, xPos, yPos);                          \
        if (yPos == getmaxy(root))                        \
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

extern std::vector<std::string> commandHistory;
extern std::vector<std::string> commands;

void StartCurses();
void GetConsoleInput(std::string &out);
void GetUserInput(std::string &out);

extern WINDOW *root;
extern int xPos;
extern int yPos;

#endif