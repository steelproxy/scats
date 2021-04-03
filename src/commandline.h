#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <curses.h>
#include <string>
#include <vector>

#define KEY_ESCAPE 27
#define KEY_DELETE 330
#define ctrl(x) ((x)&0x1f)

#define ncOutCmd(message)                        \
    {                                            \
        std::ostringstream stringBuilder;        \
        stringBuilder.str(string());             \
        stringBuilder << message;                \
        commandLine->Print(stringBuilder.str()); \
    }

#define GetConsoleInput(lineedit) commandLine->GetInput(lineedit)

class CommandLine
{
public:
    CommandLine();

    void Print(std::string out);
    void PrintPrompt();
    std::string GetInput(bool lineEdit);
    void Clear();
    void AddCommands(std::vector<std::string> newCommands);

private:
    void Redraw(std::string &out, size_t pos, size_t starting);

    WINDOW *wCommandLine;
    size_t commandHistoryIndex;
    std::vector<std::string> commandHistory;
    std::vector<std::string> commands;
};

extern CommandLine *commandLine;

#endif