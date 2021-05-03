#include "chatlog.h"
#include "cursesmode.h"
#include "setting.h"
#include "statusline.h"
#include "log.h"

using namespace std;

ChatLog::ChatLog()
{
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX);             // get terminal dimensions
    _wChatLog = newwin(maxY - 4, maxX, 2, 0); // make windows

    keypad(_wChatLog, true);
    mouseinterval(0);
    mousemask(BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
    clearok(_wChatLog, true);
    //scrollok(_wChatLog, true); // enable scrolling
    _chatHistoryIndex = 0;
}

void ChatLog::Print(string out)
{
    int maxX;
    maxX = getmaxx(_wChatLog); // get terminal dimensions

    int chatHistoryLength = getInt("General", "ChatHistoryLength", 500);
    if(chatHistoryLength <= 0)
    {
        chatHistoryLength = 5;
    }
    // push to chat history
    if (_chatHistory.size() >= static_cast<long unsigned>(chatHistoryLength))
    {
        _chatHistory.erase(_chatHistory.begin());
    }
    if (out.length() > static_cast<long unsigned>(maxX))
    {
        for (size_t currentX = 0; currentX < out.length(); currentX += maxX)
        {
            string chatLineStr;
            chatLineStr = out.substr(currentX, maxX);
            if (_chatHistoryIndex == _chatHistory.size())
                _chatHistoryIndex++;
            _chatHistory.push_back(chatLineStr);
            if (chatLineStr.length() < static_cast<long unsigned>(maxX))
                break;
        }
    }
    else
    {
        if (_chatHistoryIndex == _chatHistory.size())
            _chatHistoryIndex++; // push forward if at bottom
        _chatHistory.push_back(out);
    }

    string scrollLock = _iniStructure["General"]["scrollLock"];
    quickLog(VERBOSE, "scrollLock enabled: " << scrollLock);
    if (scrollLock != "true")
    {
        _chatHistoryIndex = _chatHistory.size();
        statusLine->Unread(false);
    }
    else
    {
        if (_chatHistoryIndex < _chatHistory.size())
            statusLine->Unread(true);
    }

    Redraw();
}

void ChatLog::Clear()
{
    statusLine->Unread(false); // clear unread
    wclear(_wChatLog);         // clear window
    wmove(_wChatLog, 0, 0);

    _chatHistory.clear(); // clear chat history
    _chatHistoryIndex = 0;

    wrefresh(_wChatLog);
}

void ChatLog::ScrollUp()
{
    if (_chatHistoryIndex > static_cast<long unsigned>(getmaxy(_wChatLog)))
    {
        _chatHistoryIndex--;
        quickLog(VERBOSE, "index down = " << _chatHistoryIndex);
        Redraw();
    }
}

void ChatLog::ScrollDown()
{
    if (_chatHistoryIndex < _chatHistory.size())
    {
        _chatHistoryIndex++;
        quickLog(VERBOSE, "index up = " << _chatHistoryIndex);
        Redraw();
    }
}

void ChatLog::Redraw()
{
    wclear(_wChatLog); // clear and redraw

    int col;
    int row;
    getmaxyx(_wChatLog, col, row); // get max terminal dimensions

    if (_chatHistoryIndex == _chatHistory.size()) // set last message read
        statusLine->Unread(false);

    for (int index = 0; index <= col; index++)
    {
        if (_chatHistoryIndex - index >= 0 && _chatHistoryIndex - index < _chatHistory.size())
        {
            mvwprintw(_wChatLog, col - index, 0, "%s", _chatHistory.at(_chatHistoryIndex - index).c_str());
        }
    }

    wrefresh(_wChatLog);
}

void ChatLog::Resize()
{
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX); // get max terminal dimensions

    wresize(_wChatLog, maxY - 4, maxX); // idk why tf i gotta go up 4 but ok, oh nvm i get it : if you ever get fried and forget this its because the statusline also occupies 2 columns
    mvwin(_wChatLog, 2, 0);             // move under status line

    _chatHistoryIndex = _chatHistory.size(); // scroll to bottom
    Redraw();
}