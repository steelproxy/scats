#include <functional>
#include <map>
#include "commandline.h"
#include "../hotkey/hotkey.h"
#include "../chatlog/chatlog.h"
#include "../isprint.h"
#include "../statusline/statusline.h"
#include "../../chat/chat_message.hpp"
#include "../../commands/commands.h"
#include "../../log/log.h"
#include "../../setting/setting.h"

void test() { quickPrintLog(INFO, "you are gay."); }

void CommandLine::Redraw(std::string &out, size_t pos, size_t starting)
{
    int maxX = getmaxx(this->_wCommandLine);
    int yPos;
    int xPos;
    getyx(this->_wCommandLine, yPos, xPos);
    (void)yPos;
    mvwhline(this->_wCommandLine, 0, 0, 0, maxX);

    wmove(this->_wCommandLine, 1, starting);
    wclrtoeol(this->_wCommandLine);

    // print buffer by character and highlight selected character to simulate
    // character
    for (int index = 0; index < out.length(); index++)
    {
        if (index == pos)
        {
            // set reverse attr
            wattron(this->_wCommandLine, A_REVERSE);
        }
        // print character
        waddch(this->_wCommandLine, out.at(index));

        // reset terminal attrs
        wattrset(this->_wCommandLine, A_NORMAL);
    }

    // move to selected character
    wmove(this->_wCommandLine, 1, starting + pos);

    // print highlighted space to simulate cursor
    if (pos >= out.length())
    {
        wattron(this->_wCommandLine, A_REVERSE);
        waddch(this->_wCommandLine, ' ');
        wattrset(this->_wCommandLine, A_NORMAL);
    }

    wrefresh(this->_wCommandLine);
}

CommandLine::CommandLine()
{
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX); // get terminal dimensions
    this->_wCommandLine = newwin(2, maxX, maxY - 2, 0);

    keypad(this->_wCommandLine, true); // enable advanced key sequences
    mvwhline(this->_wCommandLine, 0, 0, 0, maxX);
    wrefresh(this->_wCommandLine);

    this->_commandHistoryIndex = 0;

    this->_hotkeyMan.AddHotkey(ctrl('h'), DisplayHelp);
    this->_hotkeyMan.AddHotkey(KEY_ESCAPE, InteractiveEditSettings);
}

void CommandLine::Print(std::string out)
{
    Clear();
    wprintw(this->_wCommandLine, "%s", out.c_str());
    wrefresh(this->_wCommandLine);
}

void CommandLine::PrintPrompt()
{
    wmove(this->_wCommandLine, 1, 0);
    wclrtoeol(this->_wCommandLine);
    CommandLine::Print("[" + _iniStructure["General"]["userHandle"] + "]: ");
}

std::string CommandLine::LineInput()
{
    std::string lineBuf;

    // get starting x position
    int startingXPos = getcurx(this->_wCommandLine);

    int charBuf;
    int lineBufPos = 0;
    while ((charBuf = wgetch(this->_wCommandLine)) != '\n')
    {
        switch (charBuf)
        {
        case KEY_LEFT:
            if (lineBufPos > 0)
            {
                lineBufPos--;
                quickLog(VERBOSE, "Got left. lineBufPos=" << lineBufPos);
            }
            break;

        case KEY_RIGHT:
            if (lineBufPos < lineBuf.length())
            {
                lineBufPos++;
                quickLog(VERBOSE, "Got right. lineBufPos=" << lineBufPos);
            }
            break;

        case KEY_UP:
            // scroll history
            break;

        case KEY_DOWN:
            // scroll history
            break;

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
                }
                else if (event.bstate & BUTTON5_PRESSED)
                {
                    quickLog(VERBOSE, "Got mouse scroll down.");
                    chatLog->ScrollDown();
                }
                else
                {
                    continue;
                }
            }
            else
            {
                quickLog(ERROR, "Failed to get mouse event.");
                continue;
            }
            continue;
        }

        case __KEY_BACKSPACE:
            if(lineBufPos > 0 && lineBuf.length() > 0)
            {
                lineBuf.erase(lineBuf.begin() + (--lineBufPos));
            }
            break;

        default:
            if(isPrintKey(charBuf)) {
            lineBuf.insert(lineBuf.begin() + lineBufPos++, charBuf);}

            break;
        }
        quickLog(VERBOSE, "charBuf=" << charBuf);
        this->Redraw(lineBuf, lineBufPos, startingXPos);
    }
    quickLog(VERBOSE, "startingXPos=" << startingXPos << " lineBufPos="
                                      << lineBufPos << " lineBuf=" << lineBuf);

    // execute comand
    if (lineBuf.length() > 1 && lineBuf.at(0) == '/')
    {
        std::string lineBufSub = lineBuf.substr(1);
        for (std::vector<Command>::iterator _iterator = this->_commands.begin();
             _iterator != this->_commands.end(); _iterator++)
        {
            std::string command = _iterator->name;
            if (lineBufSub == command)
            {
                quickPrintLog(VERBOSE, "Executing command: " << lineBuf);
                _iterator->Execute();
            }
        }
    }

    this->Clear();
    this->PrintPrompt();
    return lineBuf;
}

std::string CommandLine::GetInput(bool lineEdit) // TODO: fix overflow
{
    std::string out;
    int charBuf = 0;
    int startingXPos;  // starting x position
    int _xPos, _yPos;  // current x and y positions
    size_t outPos = 0; // position in output buffer
    size_t commandHistoryIndex = (this->_commandHistory.size() == 0)
                                     ? 0
                                     : this->_commandHistory.size() - 1;

    bool historyScrollUp = false;

    getyx(this->_wCommandLine, _yPos, _xPos); // get current cursor position
    _yPos = 1;
    startingXPos = _xPos; // store starting cursor x position

    while (charBuf != '\n')
    {
        wrefresh(this->_wCommandLine);
        Redraw(out, outPos, startingXPos);

        getyx(this->_wCommandLine, _yPos, _xPos);

        charBuf = wgetch(this->_wCommandLine);

        if (isPrintKey(charBuf))
        {
            /*if(outPos >= getmaxx(this->_wCommandLine) - startingXPos)
            {
                    quickPrintLog(VERBOSE, "Line overflow.");
                    wmove(this->_wCommandLine, _yPos, startingXPos);
                    wclrtoeol(this->_wCommandLine);

                    std::string line;
                    line = out.substr(outPos - ((getmaxx(this->_wCommandLine) -
            startingXPos))); wprintw(this->_wCommandLine, "%s", line.c_str());
                    getyx(this->_wCommandLine, _yPos, _xPos);
            }*/
            // winsch(this->_wCommandLine, charBuf);             // insert
            // character left of current cursor pos wmove(this->_wCommandLine,
            // _yPos, ++_xPos);
            // // move cursor forward one
            outPos++;
            wrefresh(this->_wCommandLine);
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
                        wprintw(this->_wCommandLine, "%s", out.c_str());
                        wmove(this->_wCommandLine, _yPos, _xPos);
                    }
                    else if (event.bstate & BUTTON5_PRESSED)
                    {
                        quickLog(VERBOSE, "Got mouse scroll down.");
                        chatLog->ScrollDown();
                        PrintPrompt();
                        wprintw(this->_wCommandLine, "%s", out.c_str());
                        wmove(this->_wCommandLine, _yPos, _xPos);
                    }
                    else
                    {
                        continue;
                    }
                }
                continue;
            }

            case __KEY_BACKSPACE:
            {
                quickLog(VERBOSE, "got backspace. ");
                if (outPos > 0) // if not; at beginning
                {
                    mvwdelch(this->_wCommandLine, _yPos,
                             _xPos - 1); // move to and erase previous character
                    out.erase(
                        out.begin() +
                        (--outPos)); // erase character from std::string buffer
                    continue;
                }
                continue;
            }

            case KEY_LEFT:
            {
                quickLog(VERBOSE, "got left.");
                if (outPos > 0) // if not beginning of std::string buffer
                {
                    outPos--; // move back one character in std::string buffer
                    wmove(this->_wCommandLine, _yPos,
                          _xPos - 1); // move cursor back one
                }
                continue;
            }

            case KEY_RIGHT:
            {
                quickLog(VERBOSE, "got right.");
                if (outPos < out.length()) // if at end of std::string buffer
                {
                    outPos++; // move forward one character in std::string
                              // buffer
                    wmove(this->_wCommandLine, _yPos,
                          _xPos + 1); // move cursor forward one
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

                if (this->_commandHistory
                        .empty()) // if command history is empty
                {
                    continue;
                }

                out.clear(); // clear std::string buffer
                wmove(this->_wCommandLine, _yPos,
                      startingXPos);            // move to beginning of line
                wclrtoeol(this->_wCommandLine); // clear to end of line

                if (historyScrollUp == false) // if first time scrolling up
                {
                    out = this->_commandHistory.at(
                        this->_commandHistory.size() -
                        1); // set std::string buffer to current
                            // command in this->_commandHistory
                    historyScrollUp =
                        true; // disable first-time scroll up feature
                }
                else if (commandHistoryIndex !=
                         0) // if not at beginning of history
                {
                    out = this->_commandHistory.at(
                        --commandHistoryIndex); // decrement index and set
                                                // std::string buffer to
                                                // previous command
                }
                else
                {
                    out = this->_commandHistory.at(
                        commandHistoryIndex); // set std::string buffer to
                                              // current command in
                                              // this->_commandHistory
                }

                out.insert(out.begin(), '/'); // insert '/' denoting command
                waddstr(this->_wCommandLine,
                        out.c_str()); // print selected std::string
                wmove(this->_wCommandLine, _yPos,
                      startingXPos +
                          out.length()); // move cursor to end of std::string
                outPos = out.length(); // update position in std::string buffer
                                       // to end
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

                if (this->_commandHistory.empty() ||
                    !historyScrollUp) // if command history is empty
                {
                    continue;
                }

                out.clear(); // clear std::string buffer
                wmove(this->_wCommandLine, _yPos,
                      startingXPos);            // move to beginning of line
                wclrtoeol(this->_wCommandLine); // clear to end of line

                if (commandHistoryIndex ==
                    this->_commandHistory.size() - 1) // if at end
                {
                    out = "";
                }
                else
                {
                    out = this->_commandHistory.at(++commandHistoryIndex);
                }

                if (out != "")
                {
                    out.insert(out.begin(), '/');
                }
                waddstr(this->_wCommandLine, out.c_str());
                wmove(this->_wCommandLine, _yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            // delete key
            case KEY_DELETE:
            {
                quickLog(VERBOSE, "got delete.");
                if (outPos < out.length() && outPos >= 0)
                {
                    wmove(this->_wCommandLine, _yPos,
                          startingXPos);            // move to beginning of line
                    wclrtoeol(this->_wCommandLine); // clear line
                    out.erase(
                        out.begin() +
                        outPos); // erase character from std::string buffer
                    waddstr(this->_wCommandLine,
                            out.c_str()); // print new std::string
                    wmove(this->_wCommandLine, _yPos, _xPos);
                    continue;
                }
                continue;
            }

            case '\t':
            {
                // TODO fix segfault for tab, think i solved it out.erase
                // culprit?
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
                for (std::vector<Command>::iterator _iterator =
                         this->_commands.begin();
                     _iterator != this->_commands.end(); _iterator++)
                {
                    std::string command = _iterator->name;
                    if (command.find(out, 0) != std::string::npos)
                    {
                        quickLog(VERBOSE, "found command matching query: "
                                              << out << "~=" << command);

                        out.clear();
                        wmove(this->_wCommandLine, _yPos, startingXPos);
                        wclrtoeol(this->_wCommandLine);
                        out += '/';
                        out += command;
                        waddstr(this->_wCommandLine, out.c_str());
                        wmove(this->_wCommandLine, _yPos,
                              startingXPos + out.length());
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
                wmove(this->_wCommandLine, _yPos, startingXPos);
                outPos = 0;
                continue;
            }

            case ctrl('e'):
            {
                quickLog(VERBOSE, "got ctrl+e.");
                wmove(this->_wCommandLine, _yPos, startingXPos + out.length());
                outPos = out.length();
                continue;
            }

            case ctrl('k'):
            {
                quickLog(VERBOSE, "got ctrl+k.");
                wclrtoeol(this->_wCommandLine);
                out.erase(out.begin() + outPos, out.end());
                // outPos = 0
                continue;
            }

            case KEY_RESIZE:
            {
                statusLine->Resize();
                chatLog->Resize();
                this->Resize();
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
                    this->_hotkeyMan.ProcessKey(charBuf);
                    quickLog(VERBOSE,
                             "got non-printable character: " << charBuf);
                }
                continue;
            }
            }
        }

        if (charBuf != '\n')
        {
            out.insert(out.begin() + (outPos - 1), charBuf);
            historyScrollUp = false;
        }
    }

    if (out.length() > 1 && out.at(0) == '/')
    {
        std::string commandSubStr = out;
        commandSubStr.erase(commandSubStr.begin());
        // push command history
        if (this->_commandHistory.size() <
            1000) // if there is room left in the history vector
        {
            this->_commandHistory.push_back(
                commandSubStr); // push back last command
        }
        else
        {
            this->_commandHistory.erase(
                this->_commandHistory
                    .begin()); // erase first command in history vector
            this->_commandHistory.push_back(
                commandSubStr); // push back last command
        }
    }

    wmove(this->_wCommandLine, _yPos, startingXPos);
    wclrtoeol(this->_wCommandLine);
    wrefresh(this->_wCommandLine);

    quickLog(VERBOSE, " _xPos=" << _xPos << " _yPos=" << _yPos
                                << " startingXPos=" << startingXPos
                                << " outpos=" << outPos << " verbatim=" << out);

    if (out.length() > 1 && out.at(0) == '/' && lineEdit)
    {
        std::string outSub = out.substr(1);
        for (std::vector<Command>::iterator _iterator = this->_commands.begin();
             _iterator != this->_commands.end(); _iterator++)
        {
            std::string command = _iterator->name;
            if (outSub == command)
            {
                quickPrintLog(VERBOSE, "Executing command: " << out);
                _iterator->Execute();
            }
        }
    }
    return out;
}

void CommandLine::Clear()
{
    wmove(this->_wCommandLine, 1, 0);
    wclrtoeol(this->_wCommandLine);
}

void CommandLine::AddCommands()
{
    // set commands
    _commands.emplace_back("add-contact", "Adds a contact.",
                           InteractiveAddContact);
    _commands.emplace_back("add-setting", "Adds a new setting.",
                           InteractiveAddSetting);
    _commands.emplace_back("change-setting", "Changes settings.",
                           InteractiveChangeSetting);
    _commands.emplace_back("delete-contact", "Deletes a contact.",
                           InteractiveDeleteContact);
    _commands.emplace_back("delete-setting", "Deletes a setting.",
                           InteractiveDeleteSetting);
    _commands.emplace_back("edit-settings", "Edits settings.",
                           InteractiveEditSettings);
    _commands.emplace_back("help", "Displays help menu.", DisplayHelp);
    _commands.emplace_back("list-contacts", "Lists contacts.",
                           InteractiveListContacts);
    _commands.emplace_back("list-settings", "Lists settings.",
                           InteractiveListSettings);
    _commands.emplace_back("nuke", "Deletes user files.", InteractiveNuke);
    _commands.emplace_back("set-user-handle", "Sets user handle.",
                           InteractiveSetUserHandle);
    _commands.emplace_back("save-settings", "Saves all settings.",
                           SaveSettings);
    _commands.emplace_back("clear", "Clears chat log.",
                           []() { chatLog->Clear(); });
    _commands.emplace_back("exit", "Exits.", []() { exit(0); });

    /*    this->_commands.insert({"help", DisplayHelp});
        this->_commands.insert({"add-contact", InteractiveAddContact});
        this->_commands.insert({"delete-contact", InteractiveDeleteContact});
        this->_commands.insert({"list-contacts", InteractiveListContacts});
        this->_commands.insert({"list-settings", InteractiveListSettings});
        this->_commands.insert({"add-setting", InteractiveAddSetting});
        this->_commands.insert({"delete-setting", InteractiveDeleteSetting});
        this->_commands.insert({"nuke", InteractiveNuke});
        this->_commands.insert({"save-settings", SaveSettings});
        this->_commands.insert({"change-setting", InteractiveChangeSetting});
        this->_commands.insert({"clear", []() { chatLog->Clear(); }});
        this->_commands.insert({"server", []() {
        StartChatServer(_iniStructure["Server"]["Port"]); }});
        this->_commands.insert({"client", []() {
        StartChatClient(_iniStructure["Client"]["Host"],
        _iniStructure["Client"]["Port"]); }}); this->_commands.insert({"exit",
       []() { quickPrintLog(INFO, "Exiting scats..."); exit(0); }});
        this->_commands.insert({"build", nullptr});
        this->_commands.insert({"list-ini", []() { ListINI(_iniStructure);
       }});*/
}

void CommandLine::Resize()
{
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX);

    mvwin(this->_wCommandLine, maxY - 2, 0);
    wresize(this->_wCommandLine, 2, maxX);
}