#include <curses.h>
#include <vector>
#include "cursesmode.h"
#include "log.h"

using namespace std;

extern Log logger;

vector<string> lastCommand;

void ngetstr(string &var)
{
    int startingX;
    int currentY;
    int currentX;
    int var_buf = 0;
    size_t pos = 0;
    size_t lastCommandIndex = lastCommand.size() - 1;

    getyx(root, currentY, startingX);

    while (var_buf != '\n')
    {
        getyx(root, currentY, currentX);
        refresh();
        var_buf = getch();

        if (isprint(var_buf))
        {
            insch(var_buf); // insert character left of current cursor pos
            move(currentY, currentX + 1); // move cursor forward one
            pos++;
        }
        else if (var_buf == KEY_BACKSPACE)
        {
            if (var.length() >= 1 && currentX != startingX)
            {
                mvdelch(currentY, currentX - 1);
                var.erase(var.begin() + (--pos));
                continue;
            }
            continue;
        }
        else if (var_buf == KEY_LEFT)
        {
            if (pos <= 0)
                continue;
            pos--;
            move(currentY, currentX - 1);
            continue;
        }
        else if (var_buf == KEY_RIGHT)
        {
            if (pos >= var.length())
                continue;
            pos++;
            move(currentY, currentX + 1);
            continue;
        }
        else if(var_buf == KEY_UP)
        {
            if(lastCommand.empty())
                continue;
            var.clear();
            move(currentY, startingX);
            clrtoeol();
            var = lastCommand.at(lastCommandIndex);
            addstr(var.c_str());
            move(currentY, startingX + var.length());
            pos = var.length();
            if(lastCommandIndex > 0 && !lastCommand.empty())
                lastCommandIndex--;
            continue;
        }
        else if(var_buf == KEY_DOWN)
        {
            if(lastCommand.empty())
                continue;
            var.clear();
            move(currentY, startingX);
            clrtoeol();
            var = lastCommand.at(lastCommandIndex);
            addstr(var.c_str());
            move(currentY, startingX + var.length());
            pos = var.length();
            if(lastCommandIndex < lastCommand.size() - 1 && !lastCommand.empty())
                lastCommandIndex++;
            continue;
        }
        quickLog(VERBOSE, "startingx=" << startingX << " pos=" << pos << " currentx=" << currentX << " char='" << (char) var_buf << "'");
        
        if(var_buf != '\n')
            var.insert(var.begin() + (pos-1), var_buf);
    } 

    move(currentY, startingX + var.length());
    addch('\n');
    quickLog(VERBOSE, "Verbatim: \"" << var << "\"");
}