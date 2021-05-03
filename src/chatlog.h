#ifndef CHATLOG_H
#define CHATLOG_H

#include <curses.h>
#include <vector>
#include <string>

#define ncOutUsr(message)                            \
    {                                                \
        std::ostringstream stringBuilder;            \
        stringBuilder.str(std::string());                 \
        stringBuilder << message;                    \
        std::string outString = stringBuilder.str(); \
        chatLog->Print(outString);                   \
    }

class ChatLog
{
public:
    ChatLog();

    void Print(std::string out);
    void Clear();
    void ScrollUp();
    void ScrollDown();

    void Resize();

private:
    void Redraw();

    WINDOW* _wChatLog;
    size_t _chatHistoryIndex;
    std::vector<std::string> _chatHistory;
};

extern ChatLog *chatLog;

#endif