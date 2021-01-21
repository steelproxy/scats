#include <curses.h>
#include <vector>
#include "cursesmode.h"
#include "log.h"

using namespace std;

bool checkPrintable(int test)
{
    if (test >= 32 && test <= 126)
        return true;
    return false;
}

void StartCurses()
{
    root = initscr();     // start curses mode
    cbreak();             // don't require newline
    keypad(root, true);   // enable advanced key sequences
    noecho();             // don't echo commands, we'll control that
    scrollok(root, true); // enable scrolling
}

void GetConsoleInput(WINDOW* win, bool lineEdit, string &out)
{
    int charBuf = 0;
    int startingXPos, _xPos, _yPos;
    size_t outPos = 0;
    size_t commandHistoryIndex = (commandHistory.size() == 0) ? 0 : commandHistory.size() - 1;

    bool historyScrollUp = false;
    bool historyScrollDown = false;

    getyx(win, _yPos, startingXPos); //

    while (charBuf != '\n')
    {
        getyx(win, _yPos, _xPos);
        wrefresh(win);
        charBuf = wgetch(win);

        quickLog(VERBOSE, "startingxpos=" << startingXPos << " outpos=" << outPos << " _xPos=" << _xPos << " char='" << (isprint((char)charBuf) ? (char)charBuf : charBuf) << "'");

        if (checkPrintable(charBuf))
        {
            winsch(win, charBuf);         // insert character left of current cursor pos
            wmove(win, _yPos, _xPos + 1); // move cursor forward one
            outPos++;
        }
        else
        {
            switch (charBuf)
            {
            case KEY_BACKSPACE:
            {
                quickLog(VERBOSE, "got backspace.");
                if (out.length() >= 1 && _xPos != startingXPos) // if not at beginning
                {
                    mvwdelch(win, _yPos, _xPos - 1);           // move to and erase previous character
                    out.erase(out.begin() + (--outPos)); // erase character from string buffer
                    continue;
                }
                continue;
            }

            case KEY_LEFT:
            {
                quickLog(VERBOSE, "got left.");
                if (outPos <= 0) // if at beginning of string buffer
                {
                    continue;
                }
                outPos--;               // move back one character in string buffer
                wmove(win, _yPos, _xPos - 1); // move cursor back one
                continue;
            }

            case KEY_RIGHT:
            {
                quickLog(VERBOSE, "got right.");
                if (outPos >= out.length()) // if at end of string buffer
                {
                    continue;
                }
                outPos++;               // move forward one character in string buffer
                wmove(win, _yPos, _xPos + 1); // move cursor forward one
                continue;
            }

            case KEY_UP:
            {
                if(!lineEdit)
                {
                    continue;
                }

                quickLog(VERBOSE, "got up.");
                historyScrollDown = false;  // cancel first-time scroll down feature                
                if (commandHistory.empty()) // if command history is empty
                {
                    continue;
                }

                out.clear();                  // clear string buffer
                wmove(win, _yPos, startingXPos);    // move to beginning of line
                wclrtoeol(win);                   // clear to end of line

                if (historyScrollUp == false) // if first time scrolling up
                {
                    out = commandHistory.at(commandHistoryIndex); // set string buffer to current command in commandHistory
                    historyScrollUp = true;                       // disable first-time scroll up feature
                }
                else if (commandHistoryIndex != 0) // if not at beginning of history
                {
                    out = commandHistory.at(--commandHistoryIndex); // decrement index and set string buffer to previous command
                }
                else
                {
                    out = commandHistory.at(commandHistoryIndex); // set string buffer to current command in commandHistory
                }

                out.insert(out.begin(), '/');             // insert '/' denoting command
                waddstr(win, out.c_str());                      // print selected string
                wmove(win, _yPos, startingXPos + out.length()); // move cursor to end of string
                outPos = out.length();                    // update position in string buffer to end
                historyScrollDown = true;
                continue;
            }

            case KEY_DOWN:
            {
                if(lineEdit)
                {
                    continue;
                }

                quickLog(VERBOSE, "got down.");
                historyScrollUp = false;    // cancel first-time scroll up feature
                if (commandHistory.empty()) // if command history is empty
                {
                    continue;
                }

                out.clear();               // clear string buffer
                wmove(win, _yPos, startingXPos); // move to beginning of line
                wclrtoeol(win);                // clear to end of line

                if (historyScrollDown == false) // if at end of history and first-time
                {
                    out = commandHistory.at(commandHistoryIndex); // set string buffer to current command in commandHistory
                    historyScrollDown = true;                     // disable first-time scroll down feature
                }
                else if (commandHistoryIndex == commandHistory.size() - 1) // if at end
                {
                    out = "";
                }
                else
                {
                    out = commandHistory.at(++commandHistoryIndex);
                }

                if (out != "")
                {
                    out.insert(out.begin(), '/');
                }
                waddstr(win, out.c_str());
                wmove(win, _yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            case '\t':
            {
                if(!lineEdit)
                {
                    continue;
                }

                if (out.size() <= 1 || out.at(0) != '/')
                {
                    continue;
                }

                out.erase(out.begin());

                for (size_t commandIndex = 0; commandIndex < commandsLen; commandIndex++)
                {
                    string command = commands[commandIndex];
                    if (command.find(out, 0) != string::npos)
                    {
                        quickLog(VERBOSE, "found command matching query: " << out << "~=" << command);
                        out.clear();
                        wmove(win, _yPos, startingXPos);
                        wclrtoeol(win);
                        out += '/';
                        out += command;
                        waddstr(win, out.c_str());
                        wmove(win, _yPos, startingXPos + out.length());
                        outPos = out.length();
                        break;
                    }
                }
                continue;
            }

            case ctrl('a'):
            {
                quickLog(VERBOSE, "got ctrl+a.");
                wmove(win, _yPos, startingXPos);
                outPos = 0;
                continue;
            }

            case ctrl('e'):
            {
                quickLog(VERBOSE, "got ctrl+e.");
                wmove(win, _yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            case ctrl('k'):
            {
                quickLog(VERBOSE, "got ctrl+k.");
                wclrtoeol(win);
                out.erase(out.begin() + outPos, out.end());
                continue;
            }

            case KEY_ESCAPE:
            {
                quickLog(VERBOSE, "got escape.");
                continue;
            }

            default:
            {
                quickLog(VERBOSE, "got non-printable character: " << charBuf);
                continue;
            }
            }
        }

        if (charBuf != '\n')
        {
            out.insert(out.begin() + (outPos - 1), charBuf);
        }
    }

    wmove(win, _yPos, startingXPos + out.length());
    waddch(win, '\n');
    quickLog(VERBOSE, "verbatim: \"" << out << "\"");
}
