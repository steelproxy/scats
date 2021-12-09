#include "commandline.h"
#include "../../chat/chat_message.hpp"
#include "../../commands/commands.h"
#include "../../log/log.h"
#include "../../setting/setting.h"
#include "../chatlog/chatlog.h"
#include "../hotkey/hotkey.h"
#include "../isprint.h"
#include "../statusline/statusline.h"
#include <curses.h>
#include <functional>
#include <iomanip>
#include <map>

void CommandLine::Redraw(std::string &out, size_t pos, size_t starting)
{

    // resize window as neccessary
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX);

    wresize(this->_wCommandLine, 2, maxX);
    mvwin(this->_wCommandLine, maxY - 2, 0);

    int yPos;
    int xPos;
    getyx(this->_wCommandLine, yPos, xPos);
    (void)yPos;
    mvwhline(this->_wCommandLine, 0, 0, 0, maxX);

    wmove(this->_wCommandLine, 1, starting);
    wclrtoeol(this->_wCommandLine);

    unsigned int startIndex = 0;
    if (maxX <= (starting + pos)) {
        startIndex = ((starting + pos) - maxX);
    }

    // print buffer by character and highlight selected character to simulate

    // print buffer by character and hig
    // character
    for (unsigned int index = startIndex;
         (index < out.length()) && ((index - startIndex) < maxX); index++) {
        if (index == static_cast<unsigned int>(pos)) {
            // set reverse attr
            quickPrintLog(INFO, "pos=" << index << " char=" << out.at(index));
            quickPrintLog(INFO, std::setw(starting + pos + 1) << "v");
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
    if (pos >= out.length()) {
        // quickPrintLog(VERBOSE, "simulated cursor");
        wattron(this->_wCommandLine, A_REVERSE);
        waddch(this->_wCommandLine, ' ');
        wattrset(this->_wCommandLine, A_NORMAL);
    }

    wrefresh(this->_wCommandLine);
}

CommandLine::CommandLine()
{
    // create window
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX);
    this->_wCommandLine = newwin(2, maxX, maxY - 2, 0);

    // setup window
    keypad(this->_wCommandLine, true); // enable advanced keyboard functions
    mvwhline(this->_wCommandLine, 0, 0, 0,
             maxX); // draw horizontal bottom border
    wrefresh(this->_wCommandLine);

    // set history to beginning
    this->_commandHistoryIndex = 0;

    // todo: move this to scats.cpp
    logAddHotkey(ctrl('h'), DisplayHelp);
    logAddHotkey(KEY_ESCAPE, InteractiveEditSettings);
    logAddHotkey(KEY_MOUSE, []() { commandLine->HandleScroll(); });
}

CommandLine::t_commandMap CommandLine::_newCommands = {

    {"add-contact", std::make_pair("Adds a contact.", InteractiveAddContact)},
    {"add-setting",
     std::make_pair("Adds a new setting.", InteractiveAddSetting)},
    //{"change-contact", std::make_pair("Changes a contact.",
    // InteractiveChangeContact)},
    {"change-setting",
     std::make_pair("Changes settings.", InteractiveChangeSetting)},
    {"delete-contact",
     std::make_pair("Deletes a contact.", InteractiveDeleteContact)},
    {"delete-setting",
     std::make_pair("Deletes a setting.", InteractiveDeleteSetting)},
    {"edit-settings",
     std::make_pair("Edits settings.", InteractiveEditSettings)},
    {"help", std::make_pair("Displays help menu.", DisplayHelp)},
    {"list-contacts",
     std::make_pair("Lists contacts.", InteractiveListContacts)},
    {"list-settings",
     std::make_pair("Lists settings.", InteractiveListSettings)},
    {"nuke", std::make_pair("Deletes user files.", InteractiveNuke)},
    {"set-user-handle",
     std::make_pair("Sets user handle.", InteractiveSetUserHandle)},
    {"save-settings", std::make_pair("Saves all settings.", SaveSettings)},
    {"clear", std::make_pair("Clears chat log.", []() { chatLog->Clear(); })},
    {"exit", std::make_pair("Exits.", []() { exit(0); })},
    {"server", std::make_pair("Starts chat server.",
                              []() {
                                  commandLine->Clear();
                                  commandLine->Print("Port: ");
                                  std::string port = commandLine->LineInput();
                                  StartChatServer(port);
                              })},
    {"client", std::make_pair("Starts chat client.",
                              []() {
                                  commandLine->Clear();
                                  commandLine->Print("Host: ");
                                  std::string host = commandLine->LineInput();
                                  commandLine->Clear();
                                  commandLine->Print("Port: ");
                                  std::string port = commandLine->LineInput();
                                  StartChatClient(host, port);
                              })}

};

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

void CommandLine::HandleScroll()
{
    MEVENT event;
    if (getmouse(&event) == OK) {
        quickLog(VERBOSE, "Got mouse event ok.");
        if (event.bstate & BUTTON4_PRESSED) {
            quickLog(VERBOSE, "Got mouse scroll up.");
            chatLog->ScrollUp();
        } else if (event.bstate & BUTTON5_PRESSED) {
            quickLog(VERBOSE, "Got mouse scroll down.");
            chatLog->ScrollDown();
        } else {
            return;
        }
    } else {
        quickLog(ERROR, "Failed to get mouse event.");
        return;
    }
}

std::string CommandLine::LineInput()
{
    std::string lineBuf;

    // get starting x position
    int         startingXPos = getcurx(this->_wCommandLine);

    int         charBuf;
    int         lineBufPos = 0;
    this->Redraw(lineBuf, lineBufPos, startingXPos);
    while ((charBuf = wgetch(this->_wCommandLine)) != '\n') {
        if (hotkeyMan->ProcessKey(charBuf)) {
            continue;
        }

        switch (charBuf) {
        case KEY_LEFT:
            if (lineBufPos > 0) {
                lineBufPos--;
                quickLog(VERBOSE, "Got left. lineBufPos=" << lineBufPos);
            }
            break;

        case KEY_RIGHT:
            if (static_cast<unsigned int>(lineBufPos) < lineBuf.length()) {
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

        case __KEY_BACKSPACE:
            if (lineBufPos > 0 && lineBuf.length() > 0) {
                lineBuf.erase(lineBuf.begin() + (--lineBufPos));
            }
            break;

        case KEY_RESIZE:
            this->Redraw(lineBuf, lineBufPos, startingXPos);
            chatLog->Redraw();
            continue;
            break;

        case '\t': {
            // TODO fix segfault for tab, think i solved it lineBuf.erase
            // culprit?
            quickLog(VERBOSE, "got tab.");

            if (lineBuf.size() <= 1 || lineBuf.at(0) != '/') {
                continue;
            }

            lineBuf.erase(lineBuf.begin());

            bool found = false;
            for (std::vector<Command>::iterator _iterator =
                     this->_commands.begin();
                 _iterator != this->_commands.end(); _iterator++) {
                std::string command = _iterator->name;
                if (command.find(lineBuf, 0) != std::string::npos) {
                    quickLog(VERBOSE, "found command matching query: "
                                          << lineBuf << "~=" << command);

                    lineBuf.clear();
                    lineBuf += '/';
                    lineBuf += command;
                    lineBufPos = lineBuf.length();
                    found      = true;
                    break;
                }
            }
            if (!found) {
                lineBuf.insert(lineBuf.begin(), '/');
            }
            this->Redraw(lineBuf, lineBufPos, startingXPos);
            continue;
        }

        default:
            if (isPrintKey(charBuf)) {
                lineBuf.insert(lineBuf.begin() + lineBufPos++, charBuf);
            }

            break;
        }
        quickLog(VERBOSE, "charBuf=" << charBuf);
        this->Redraw(lineBuf, lineBufPos, startingXPos);
    }
    quickLog(VERBOSE, "startingXPos=" << startingXPos << " lineBufPos="
                                      << lineBufPos << " lineBuf=" << lineBuf);

    // execute comand
    if (lineBuf.length() > 1 && lineBuf.at(0) == '/') {
        std::string lineBufSub = lineBuf.substr(1);
        for (t_commandMap::iterator _iterator = this->_newCommands.begin();
             _iterator != this->_newCommands.end(); _iterator++) {
            std::string commandAlias = _iterator->first;
            if (lineBufSub == commandAlias) {
                quickPrintLog(VERBOSE, "Executing command: " << lineBuf);
                _iterator->second.second();
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
