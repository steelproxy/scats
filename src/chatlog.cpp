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
    getmaxyx(stdscr, maxY, maxX);               // get terminal dimensions
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

    // push to chat history

    if (_chatHistory.size() >= static_cast<long unsigned>(getIntSet("chatHistoryLength")))
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

    quickLog(VERBOSE, "scrollLock enabled: " << getSet("scrollLock"));
    string scrollLock = getSet("scrollLock");
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
    statusLine -> Unread(false);
    wclear(_wChatLog);
    wmove(_wChatLog, 0, 0);
    _chatHistory.size();
    _chatHistory.clear();
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
    wclear(_wChatLog);
    int col;
    int row;
    getmaxyx(_wChatLog, col, row);

    if(_chatHistoryIndex == _chatHistory.size())
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