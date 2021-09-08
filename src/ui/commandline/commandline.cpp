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
    // resize window as neccessary
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX);

    mvwin(this->_wCommandLine, maxY - 2, 0);
    wresize(this->_wCommandLine, 2, maxX);

    int yPos;
    int xPos;
    getyx(this->_wCommandLine, yPos, xPos);
    (void)yPos;
    mvwhline(this->_wCommandLine, 0, 0, 0, maxX);

    wmove(this->_wCommandLine, 1, starting);
    wclrtoeol(this->_wCommandLine);


    // print buffer by character and highlight selected character to simulate
    // character
    for (unsigned int index = 0; index < out.length(); index++)
    {
        if (index == static_cast<unsigned int>(pos))
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
    this->Redraw(lineBuf, lineBufPos, startingXPos);
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
            if (static_cast<unsigned int>(lineBufPos) < lineBuf.length())
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

        case KEY_RESIZE:
            this->Redraw(lineBuf, lineBufPos, startingXPos);
            chatLog->Redraw();
            continue;
            break;

case '\t':
            {
                // TODO fix segfault for tab, think i solved it lineBuf.erase
                // culprit?
                quickLog(VERBOSE, "got tab.");

                if (lineBuf.size() <= 1 || lineBuf.at(0) != '/')
                {
                    continue;
                }

                lineBuf.erase(lineBuf.begin());

                bool found = false;
                for (std::vector<Command>::iterator _iterator =
                         this->_commands.begin();
                     _iterator != this->_commands.end(); _iterator++)
                {
                    std::string command = _iterator->name;
                    if (command.find(lineBuf, 0) != std::string::npos)
                    {
                        quickLog(VERBOSE, "found command matching query: "
                                              << lineBuf << "~=" << command);

                        lineBuf.clear();
                        lineBuf += '/';
                        lineBuf += command;
                        lineBufPos = lineBuf.length();
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    lineBuf.insert(lineBuf.begin(), '/');
                }
                this->Redraw(lineBuf, lineBufPos, startingXPos);
                continue;
            }

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