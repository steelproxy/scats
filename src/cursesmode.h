#ifndef CURSESMODE_H
#define CURSESMODE_H

#include <curses.h>
#include <vector>
#include <sstream>

#define KEY_ESCAPE 27
#define KEY_DELETE 330
#define ctrl(x) ((x)&0x1f)

///
/// @brief Macro for formatting and printing message to root ncurses window.
/// @param message Message to be printed to root ncurses window.
#define ncOutCmd(message)                                                \
    {                                                                    \
        std::ostringstream stringBuilder;\
        wmove(wCommandLine, 0, 0);                                       \
        wclrtoeol(wCommandLine);                                         \
        stringBuilder.str(string());                              \
        stringBuilder << message;                                 \
        wprintw(wCommandLine, "%s", stringBuilder.str().c_str()); \
        wrefresh(wCommandLine);                                          \
    }

#define ncOutUsr(message)                                              \
    {                                                                  \
        std::ostringstream stringBuilder;                                            \
        stringBuilder.str(string());                            \
        stringBuilder << message;                               \
        wprintw(wUserLog, "%s\n", stringBuilder.str().c_str()); \
        wrefresh(wUserLog);                                            \
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
void GetConsoleInput(bool lineEdit, std::string &out);

bool checkPrintable(int test);

extern WINDOW *wRoot;
extern WINDOW *wStatusLine;
extern WINDOW *wUserLog;
extern WINDOW *wCommandLine;

extern std::vector<std::string> commandHistory;

#endif