#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "../../commands/commands.h"
#include "../hotkey/hotkey.h"
#include <curses.h>
#include <map>
#include <string>
#include <vector>

#define KEY_ESCAPE 27
#define KEY_DELETE 330
#define __KEY_BACKSPACE 127
#define ctrl(x) ((x)&0x1f)

#define ncOutCmd(message)                                                      \
    {                                                                          \
        std::ostringstream stringBuilder;                                      \
        stringBuilder.str(std::string());                                      \
        stringBuilder << message;                                              \
        commandLine->Print(stringBuilder.str());                               \
    }

#define GetConsoleInput(lineedit) commandLine->LineInput()

typedef void (*t_voidFunction)(void);

class CommandLine
{
  public:
    CommandLine();

    void        Print(std::string out);
    void        PrintPrompt();
    std::string LineInput();
    void        Clear();
    void        Resize();
    typedef std::map<const std::string, std::pair<std::string, t_voidFunction>>
        t_commandMap;

  private:
    void    Redraw(std::string &out, size_t pos, size_t starting);
    void    HandleScroll();

    WINDOW *_wCommandLine;
    size_t  _commandHistoryIndex;
    std::vector<std::string> _commandHistory;
    std::vector<Command>     _commands;
    static t_commandMap      _newCommands;
};

extern CommandLine *commandLine;

#endif