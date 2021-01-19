#include <curses.h>
#include <vector>
#include "cursesmode.h"
#include "log.h"

using namespace std;

vector<string> commandHistory;

bool isCoolBro(int test)
{
    
}

void StartCurses()
{
    root = initscr();     // start curses mode
    cbreak();             // don't require newline
    keypad(root, true);   // enable advanced key sequences
    noecho();             // don't echo commands, we'll control that
    scrollok(root, true); // enable scrolling
}

void GetConsoleInput(string &out)
{
    int charBuf = 0;
    int startingXPos, _xPos, _yPos;
    size_t outPos = 0;
    size_t commandHistoryIndex = (commandHistory.size() == 0) ? 0 : commandHistory.size() - 1;

    bool historyScrollUp = false;
    bool historyScrollDown = false;

    getyx(root, _yPos, startingXPos); //

    while (charBuf != '\n')
    {
        getyx(root, _yPos, _xPos);
        refresh();
        charBuf = getch();

        quickLog(VERBOSE, "startingxpos=" << startingXPos << " outpos=" << outPos << " _xPos=" << _xPos << " char='" << (isprint((char) charBuf) ? (char) charBuf : charBuf) << "'");

        if (isprint(charBuf))
        {
            insch(charBuf);       // insert character left of current cursor pos
            move(_yPos, _xPos + 1); // move cursor forward one
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
                    mvdelch(_yPos, _xPos - 1);             // move to and erase previous character
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
                outPos--;             // move back one character in string buffer
                move(_yPos, _xPos - 1); // move cursor back one
                continue;
            }

            case KEY_RIGHT:
            {
                quickLog(VERBOSE, "got right.");
                if (outPos >= out.length()) // if at end of string buffer
                {
                    continue;
                }
                outPos++;             // move forward one character in string buffer
                move(_yPos, _xPos + 1); // move cursor forward one
                continue;
            }

            case KEY_UP:
            {
                quickLog(VERBOSE, "got up.");
                historyScrollDown = false;  // cancel first-time scroll down feature
                if (commandHistory.empty()) // if command history is empty
                {
                    continue;
                }
                out.clear();                  // clear string buffer
                move(_yPos, startingXPos);     // move to beginning of line
                clrtoeol();                   // clear to end of line
                if (historyScrollUp == false) // if first time scrolling up
                {
                    out = commandHistory.at(commandHistoryIndex); // set string buffer to current command in commandHistory
                    historyScrollUp = true;                       // disable first-time scroll up feature
                }
                else if (commandHistoryIndex != 0)                  // if not at beginning of history
                {
                    out = commandHistory.at(--commandHistoryIndex); // decrement index and set string buffer to previous command
                }
                else
                {
                    out = commandHistory.at(commandHistoryIndex); // set string buffer to current command in commandHistory
                }
                addstr(out.c_str());                              // print selected string
                move(_yPos, startingXPos + out.length());          // move cursor to end of string
                outPos = out.length();                            // update position in string buffer to end
                historyScrollDown = true;
                continue;
            }

            case KEY_DOWN:
            {
                quickLog(VERBOSE, "got down.");
                historyScrollUp = false;    // cancel first-time scroll up feature
                if (commandHistory.empty()) // if command history is empty
                {
                    continue;
                }
                // if (commandHistoryIndex == commandHistory.size()) // if
                // {
                //     out.clear();
                //     move(_yPos, startingXPos);
                //     clrtoeol();
                //     out = "";
                //     addstr(out.c_str());
                //     move(_yPos, startingXPos + out.length());
                //     outPos = out.length();
                //     continue;
                // }
                out.clear();              // clear string buffer
                move(_yPos, startingXPos); // move to beginning of line
                clrtoeol();               // clear to end of line

                if (commandHistoryIndex == commandHistory.size() - 1 && historyScrollDown == false) // if at end of history and first-time
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
                addstr(out.c_str());
                move(_yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            case '\t':
            {
                for (string command : commandHistory)
                {
                    if (command.find(out, 0) != string::npos)
                    {
                        quickLog(VERBOSE, "found command in history matching query: " << out << "~=" << command);
                        out.clear();
                        move(_yPos, startingXPos);
                        clrtoeol();
                        out = command;
                        addstr(out.c_str());
                        move(_yPos, startingXPos + out.length());
                        outPos = out.length();
                        continue;
                    }
                }
                for(size_t commandIndex = 0; commandIndex < commandsLen; commandIndex++)
                {
                    string command = commands[commandIndex];
                    if (command.find(out, 0) != string::npos)
                    {
                        quickLog(VERBOSE, "found command matching query: " << out << "~=" << command);
                        out.clear();
                        move(_yPos, startingXPos);
                        clrtoeol();
                        out = command;
                        addstr(out.c_str());
                        move(_yPos, startingXPos + out.length());
                        outPos = out.length();
                        continue;
                    }
                }
                continue;
            }
            }
        }

        if (charBuf != '\n')
        {
            out.insert(out.begin() + (outPos - 1), charBuf);
        }
    }

    move(_yPos, startingXPos + out.length());
    addch('\n');
    quickLog(VERBOSE, "verbatim: \"" << out << "\"");
}

void GetUserInput(string &out)
{
    int charBuf = 0;
    size_t outPos = 0;
    int startingXPos, _xPos, _yPos;

    getyx(root, _yPos, startingXPos); //

    while (charBuf != '\n')
    {
        getyx(root, _yPos, _xPos);
        refresh();
        charBuf = getch();

        if (isprint(charBuf))
        {
            insch(charBuf);       // insert character left of current cursor pos
            move(_yPos, _xPos + 1); // move cursor forward one
            outPos++;
        }
        else
        {
            switch (charBuf)
            {
            case KEY_BACKSPACE:
            {
                if (out.length() >= 1 && _xPos != startingXPos) // if not at beginning
                {
                    mvdelch(_yPos, _xPos - 1);             // move to and erase previous character
                    out.erase(out.begin() + (--outPos)); // erase character from string buffer
                    continue;
                }
                continue;
            }

            case KEY_LEFT:
            {
                if (outPos <= 0) // if at beginning of string buffer
                {
                    continue;
                }
                outPos--;             // move back one character in string buffer
                move(_yPos, _xPos - 1); // move cursor back one
                continue;
            }

            case KEY_RIGHT:
            {
                if (outPos >= out.length()) // if at end of string buffer
                {
                    continue;
                }
                outPos++;             // move forward one character in string buffer
                move(_yPos, _xPos + 1); // move cursor forward one
                continue;
            }
            }
        }

        quickLog(VERBOSE, "startingxpos=" << startingXPos << " outpos=" << outPos << " _xPos=" << _xPos << " char='" << (isprint((char) charBuf) ? (char) charBuf : charBuf) << "'");

        // print character
        if (charBuf != '\n')
        {
            out.insert(out.begin() + (outPos - 1), charBuf);
        }
    }

    // update cursor
    move(_yPos, startingXPos + out.length());
    addch('\n');
    quickLog(VERBOSE, "verbatim: \"" << out << "\"");
}
