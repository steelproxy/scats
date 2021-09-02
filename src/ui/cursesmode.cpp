#include "cursesmode.h"
#include "../log/log.h"
#include <curses.h>
#include <vector>

WINDOW *activeWindow;
int cursorY;
int cursorX;

bool checkPrintable(int test)
{
    if (test >= 32 && test <= 126)
        return true;
    return false;
}

bool isPrintStr(std::string str)
{
    for (size_t index = 0; index < str.length(); index++)
    {
        if (!checkPrintable(str.at(index)))
        {
            return false;
        }
    }
    return true;
}