#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <curses.h>
#include <vector>
#include <sstream>

#define KEY_ESCAPE 27
#define ctrl(x) ((x)&0x1f)

///
/// @brief Macro for formatting and printing message to root ncurses window.
/// @param message Message to be printed to root ncurses window.
#define ncout(message)                                    \
    {                                                     \
        int xPos, yPos;                                   \
        (void) xPos;                                      \
        getyx(root, xPos, yPos);                          \
        if (yPos == getmaxy(root))                        \
            scroll(root);                                 \
        logger.stringBuilder.str(string());               \
        logger.stringBuilder << message;                  \
        printw("%s", logger.stringBuilder.str().c_str()); \
        refresh();                                        \
    }

///
/// @brief Macro for formatting and printing line to root ncurses window.
/// @param line Line to be printed to root ncurses window.
#define ncoutln(line)        \
    {                           \
        ncout(line << endl); \
    }

extern std::vector<std::string> commandHistory;
extern const std::string commands[];
extern const size_t commandsLen;

///
/// @brief Sets up and starts curses mode.
///
void StartCurses();

/// 
/// @brief Gets user input, with command history and line editing.
/// @param out Reference to string that will be filled with user input.
void GetConsoleInput(WINDOW* win, std::string &out);

///
/// @brief Gets user input with line editing.
///
void GetUserInput(std::string &out);

extern WINDOW *root;

extern std::vector<std::string> commandHistory;

#endif