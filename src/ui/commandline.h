#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <curses.h>
#include <string>
#include <vector>
#include <map>
#include "../ui/hotkey.h"

#define KEY_ESCAPE 27
#define KEY_DELETE 330
#define ctrl(x) ((x)&0x1f)

#define ncOutCmd(message)                        \
    {                                            \
        std::ostringstream stringBuilder;        \
        stringBuilder.str(std::string());             \
        stringBuilder << message;                \
        commandLine->Print(stringBuilder.str()); \
    }

#define GetConsoleInput(lineedit) commandLine->GetInput(lineedit)

typedef void (*voidFunctionType)(void); 

class CommandLine
{
public:
    CommandLine();

    void Print(std::string out);
    void PrintPrompt();
    std::string GetInput(bool lineEdit);
    void Clear();
    void AddCommands();
    void Resize();

private:
    void Redraw(std::string &out, size_t pos, size_t starting);

    WINDOW *_wCommandLine;
    size_t _commandHistoryIndex;
    std::vector<std::string> _commandHistory;
    std::map<std::string, voidFunctionType> _commands;
    HotkeyManager _hotkeyMan;
};

extern CommandLine *commandLine;

#endif