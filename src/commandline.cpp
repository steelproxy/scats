#include "commandline.h"
#include "chatlog.h"
#include "cursesmode.h"
#include "log.h"
#include "setting.h"

using namespace std;

CommandLine::CommandLine()
{
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX); // get terminal dimensions
    wCommandLine = newwin(1, maxX, maxY - 1, 0);

    keypad(wCommandLine, true); // enable advanced key sequences

    commandHistoryIndex = 0;
}

void CommandLine::Print(string out)
{
    Clear();
    wprintw(wCommandLine, "%s", out.c_str());
    wrefresh(wCommandLine);
}

void CommandLine::PrintPrompt()
{
    wmove(wCommandLine, 0, 0);
    wclrtoeol(wCommandLine);
    CommandLine::Print("[" + getSet("userHandle") + "]: ");
}

string CommandLine::GetInput(bool lineEdit) // TODO: fix overflow
{
    string out;
    int charBuf = 0;
    int startingXPos;  // starting x position
    int _xPos, _yPos;  // current x and y positions
    size_t outPos = 0; // position in output buffer
    size_t commandHistoryIndex =
        (commandHistory.size() == 0) ? 0 : commandHistory.size() - 1;

    bool historyScrollUp = false;
    bool historyScrollDown = false;

    getyx(wCommandLine, _yPos, _xPos); // get current cursor position
    startingXPos = _xPos;              // store starting cursor x position

    while (charBuf != '\n')
    {
        wrefresh(wCommandLine);

        getyx(wCommandLine, _yPos, _xPos);
        SetCursorPosition(wCommandLine, _yPos, _xPos);
        charBuf = wgetch(wCommandLine);

        quickLog(VERBOSE, " _xPos=" << _xPos << " _yPos=" << _yPos
                                    << " startingXPos=" << startingXPos
                                    << " outpos=" << outPos
                                    << " charBuf=" << charBuf);

        if (checkPrintable(charBuf))
        {
            winsch(wCommandLine, charBuf);       // insert character left of current cursor pos
            wmove(wCommandLine, _yPos, ++_xPos); // move cursor forward one
            outPos++;
            wrefresh(wCommandLine);
        }
        else
        {
            switch (charBuf)
            {
            case KEY_MOUSE:
            {
                MEVENT event;
                if (getmouse(&event) == OK)
                {
                    quickLog(VERBOSE, "Got mouse event ok.");
                    if (event.bstate & BUTTON4_PRESSED)
                    {
                        quickLog(VERBOSE, "Got mouse scroll up.");
                        chatLog->ScrollUp();
                        PrintPrompt();
                        wprintw(wCommandLine, "%s", out.c_str());
                        wmove(wCommandLine, _yPos, _xPos);
                    }
                    else if (event.bstate & BUTTON5_PRESSED)
                    {
                        quickLog(VERBOSE, "Got mouse scroll down.");
                        chatLog->ScrollDown();
                        PrintPrompt();
                        wprintw(wCommandLine, "%s", out.c_str());
                        wmove(wCommandLine, _yPos, _xPos);
                    }
                    else
                    {
                        continue;
                    }
                }
                continue;
            }

            case KEY_BACKSPACE:
            {
                quickLog(VERBOSE, "got backspace.");
                if (outPos > 0) // if not at beginning
                {
                    mvwdelch(wCommandLine, _yPos,
                             _xPos - 1); // move to and erase previous character
                    out.erase(out.begin() +
                              (--outPos)); // erase character from string buffer
                    continue;
                }
                continue;
            }

            case KEY_LEFT:
            {
                quickLog(VERBOSE, "got left.");
                if (outPos > 0) // if not beginning of string buffer
                {
                    outPos--;                              // move back one character in string buffer
                    wmove(wCommandLine, _yPos, _xPos - 1); // move cursor back one
                }
                continue;
            }

            case KEY_RIGHT:
            {
                quickLog(VERBOSE, "got right.");
                if (outPos < out.length()) // if at end of string buffer
                {
                    outPos++;                              // move forward one character in string buffer
                    wmove(wCommandLine, _yPos, _xPos + 1); // move cursor forward one
                }
                continue;
            }

            case KEY_UP:
            {
                quickLog(VERBOSE, "got up.");
                if (!lineEdit)
                {
                    quickLog(VERBOSE, "Line editing disabled, ignoring...");
                    continue;
                }

                historyScrollDown = false;  // cancel first-time scroll down feature
                if (commandHistory.empty()) // if command history is empty
                {
                    continue;
                }

                out.clear();                              // clear string buffer
                wmove(wCommandLine, _yPos, startingXPos); // move to beginning of line
                wclrtoeol(wCommandLine);                  // clear to end of line

                if (historyScrollUp == false) // if first time scrolling up
                {
                    out = commandHistory.at(
                        commandHistoryIndex); // set string buffer to current command in
                                              // commandHistory
                    historyScrollUp = true;   // disable first-time scroll up feature
                }
                else if (commandHistoryIndex != 0) // if not at beginning of history
                {
                    out = commandHistory.at(
                        --commandHistoryIndex); // decrement index and set string buffer
                                                // to previous command
                }
                else
                {
                    out = commandHistory.at(
                        commandHistoryIndex); // set string buffer to current command in
                                              // commandHistory
                }

                out.insert(out.begin(), '/');       // insert '/' denoting command
                waddstr(wCommandLine, out.c_str()); // print selected string
                wmove(wCommandLine, _yPos,
                      startingXPos + out.length()); // move cursor to end of string
                outPos = out.length();              // update position in string buffer to end
                historyScrollDown = true;
                continue;
            }

            case KEY_DOWN:
            {
                quickLog(VERBOSE, "got down.");
                if (!lineEdit)
                {
                    quickLog(VERBOSE, "line editing disabled, ignoring...");
                    continue;
                }

                historyScrollUp = false;    // cancel first-time scroll up feature
                if (commandHistory.empty()) // if command history is empty
                {
                    continue;
                }

                out.clear();                              // clear string buffer
                wmove(wCommandLine, _yPos, startingXPos); // move to beginning of line
                wclrtoeol(wCommandLine);                  // clear to end of line

                if (historyScrollDown == false) // if at end of history and first-time
                {
                    out = commandHistory.at(
                        commandHistoryIndex); // set string buffer to current command in
                                              // commandHistory
                    historyScrollDown = true; // disable first-time scroll down feature
                }
                else if (commandHistoryIndex ==
                         commandHistory.size() - 1) // if at end
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
                waddstr(wCommandLine, out.c_str());
                wmove(wCommandLine, _yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            // delete key
            case KEY_DELETE:
            {
                quickLog(VERBOSE, "got delete.");
                if (outPos < out.length() && outPos >= 0)
                {
                    wmove(wCommandLine, _yPos, startingXPos); // move to beginning of line
                    wclrtoeol(wCommandLine);                  // clear line
                    out.erase(out.begin() + outPos);          // erase character from string buffer
                    waddstr(wCommandLine, out.c_str());       // print new string
                    wmove(wCommandLine, _yPos, _xPos);
                    continue;
                }
                continue;
            }

            case '\t':
            {
                // TODO fix segfault for tab, think i solved it out.erase culprit?
                quickLog(VERBOSE, "got tab.");
                if (!lineEdit)
                {
                    quickLog(VERBOSE, "Line editing disabled, ignoring...");
                    continue;
                }

                if (out.size() <= 1 || out.at(0) != '/')
                {
                    continue;
                }

                out.erase(out.begin());

                bool found = false;
                for (size_t commandIndex = 0; commandIndex < commands.size(); commandIndex++)
                {
                    string command = commands[commandIndex];
                    if (command.find(out, 0) != string::npos)
                    {
                        quickLog(VERBOSE, "found command matching query: " << out << "~="
                                                                           << command);
                        out.clear();
                        wmove(wCommandLine, _yPos, startingXPos);
                        wclrtoeol(wCommandLine);
                        out += '/';
                        out += command;
                        waddstr(wCommandLine, out.c_str());
                        wmove(wCommandLine, _yPos, startingXPos + out.length());
                        outPos = out.length();
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    out.insert(out.begin(), '/');
                }
                continue;
            }

            case ctrl('a'):
            {
                quickLog(VERBOSE, "got ctrl+a.");
                wmove(wCommandLine, _yPos, startingXPos);
                outPos = 0;
                continue;
            }

            case ctrl('e'):
            {
                quickLog(VERBOSE, "got ctrl+e.");
                wmove(wCommandLine, _yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            case ctrl('k'):
            {
                quickLog(VERBOSE, "got ctrl+k.");
                wclrtoeol(wCommandLine);
                out.erase(out.begin() + outPos, out.end());
                // outPos = 0
                continue;
            }

            case ctrl('h'):
            {
                return "" + ctrl('h');
            }

            case ctrl('q'):
            {
                quickLog(VERBOSE, "got ctrl+q.");
                exit(0);
            }

            case KEY_ESCAPE:
            {
                quickLog(VERBOSE, "got escape.");
                continue;
            }

            default:
            {
                if (charBuf == '\n')
                {
                    quickLog(VERBOSE, "got newline. ending input...");
                }
                else
                {
                    quickLog(VERBOSE, "got non-printable character: " << charBuf);
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

    if (out.length() > 1 && out.at(0) == '/')
    {
        string commandSubStr = out;
        commandSubStr.erase(commandSubStr.begin());
        // push command history
        if (commandHistory.size() < 1000) // if there is room left in the history vector
        {
            commandHistory.push_back(commandSubStr); // push back last command
        }
        else
        {
            commandHistory.erase(commandHistory.begin()); // erase first command in history vector
            commandHistory.push_back(commandSubStr);      // push back last command
        }
    }

    wmove(wCommandLine, _yPos, startingXPos);
    wclrtoeol(wCommandLine);
    wrefresh(wCommandLine);
    quickLog(VERBOSE, "verbatim: \"" << out << "\"");

    return out;
}

void CommandLine::Clear()
{
    wmove(wCommandLine, 0, 0);
    wclrtoeol(wCommandLine);
}

void CommandLine::AddCommands(vector<string> newCommands)
{
    for(string command : newCommands)
    {
        commands.push_back(command);
    }
}