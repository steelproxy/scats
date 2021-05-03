#include "cursesmode.h"
#include "log.h"
#include <curses.h>
#include <vector>

using namespace std;

WINDOW* activeWindow;
int cursorY;
int cursorX;

bool checkPrintable(int test)
{
  if (test >= 32 && test <= 126)
    return true;
  return false;
}

void SetCursorPosition(WINDOW* targetWindow, int targetY, int targetX)
{
    activeWindow = targetWindow;
    cursorY = targetY;
    cursorX = targetX;
}

void RestoreCursorPosition()
{
    wmove(activeWindow, cursorY, cursorX);
    wrefresh(activeWindow);
}

bool isPrintStr(string str)
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