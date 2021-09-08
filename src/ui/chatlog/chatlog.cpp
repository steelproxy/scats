#include "chatlog.h"
#include "../../log/log.h"
#include "../../setting/setting.h"
#include "../isprint.h"
#include "../statusline/statusline.h"

ChatLog::ChatLog()
{
    // get terminal dimensions
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX);

    // make window
    this->_wChatLog = newwin(maxY - 4, maxX, 2, 0);

    // set up window parameters, enable scrolling
    keypad(this->_wChatLog, true);
    mouseinterval(0);
    mousemask(BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);

    // set chat history index to start
    this->_chatHistoryIndex = 0;
}

void ChatLog::Print(std::string out)
{
    // get window dimensions
    int maxX;
    maxX = getmaxx(this->_wChatLog);

    // get chat history length setting
    int chatHistoryLength;
    chatHistoryLength = getInt("General", "ChatHistoryLength", 500);
    if (chatHistoryLength <= 0)
    {
        chatHistoryLength = std::stoi(DEFAULT_CHAT_HISTORY_LEN);
    }

    // if chat history has grown too large, erase first element
    if (this->_chatHistory.size() >=
        static_cast<long unsigned>(chatHistoryLength))
    {
        this->_chatHistory.erase(this->_chatHistory.begin());
    }

    // TODO: stop inserting output in lines, move functionality to
    // ChatLog::Redraw()
    if (out.length() > static_cast<long unsigned>(maxX))
    {
        for (size_t currentX = 0; currentX < out.length(); currentX += maxX)
        {
            std::string chatLineStr;
            chatLineStr = out.substr(currentX, maxX);
            if (this->_chatHistoryIndex == this->_chatHistory.size())
                this->_chatHistoryIndex++;
            this->_chatHistory.push_back(chatLineStr);
            if (chatLineStr.length() < static_cast<long unsigned>(maxX))
                break;
        }
    }
    else
    {
        if (this->_chatHistoryIndex == this->_chatHistory.size())
            this->_chatHistoryIndex++; // push forward if at bottom
        this->_chatHistory.push_back(out);
    }

    // handle scrolllock scrolling
    std::string scrollLock = _iniStructure.get("General").get("scrollLock");
    quickLog(VERBOSE, "scrollLock enabled: " << scrollLock);
    if (scrollLock != "true")
    {
        this->_chatHistoryIndex = this->_chatHistory.size();
        statusLine->Unread(false);
    }
    else
    {
        if (this->_chatHistoryIndex < this->_chatHistory.size())
            statusLine->Unread(true);
    }

    Redraw();
}

void ChatLog::Clear()
{
    // clear chat log window
    wclear(this->_wChatLog);
    wmove(this->_wChatLog, 0, 0);

    // clear chat history
    this->_chatHistory.clear();
    this->_chatHistoryIndex = 0;

    // set unread to false
    statusLine->Unread(false);

    // refresh window
    wrefresh(this->_wChatLog);
}

void ChatLog::ScrollUp()
{
    if (this->_chatHistoryIndex >
        static_cast<long unsigned>(getmaxy(this->_wChatLog)))
    {
        this->_chatHistoryIndex--;
        quickLog(VERBOSE, "index down = " << this->_chatHistoryIndex);
        Redraw();
    }
}

void ChatLog::ScrollDown()
{
    if (this->_chatHistoryIndex < this->_chatHistory.size())
    {
        this->_chatHistoryIndex++;
        quickLog(VERBOSE, "index up = " << this->_chatHistoryIndex);
        Redraw();
    }
}

void ChatLog::Redraw()
{
    wclear(this->_wChatLog); // clear and redraw

    // get max terminal dimensions
    int maxTermY;
    int maxTermX;
    (void) maxTermY;
    getmaxyx(this->_wChatLog, maxTermY, maxTermX);

    // get max window dimensions
    int maxWinX;
    int maxWinY;
    getmaxyx(this->_wChatLog, maxWinX, maxWinY);

    // resize
    int maxY;
    int maxX;
    getmaxyx(stdscr, maxY, maxX); // get max terminal dimensions

    wresize(this->_wChatLog, maxY - 4,
            maxX); // idk why tf i gotta go up 4 but ok, oh nvm i get it : if
                   // you ever get fried and forget this its because the
                   // statusline also occupies 2 columns
    mvwin(this->_wChatLog, 2, 0); // move under status line

    if (this->_chatHistoryIndex ==
        this->_chatHistory.size()) // set last message read
        statusLine->Unread(false);

    for (int index = 0; index <= maxWinX; index++)
    {
        if (this->_chatHistoryIndex - index >= 0 &&
            this->_chatHistoryIndex - index < this->_chatHistory.size())
        {
            mvwprintw(
                this->_wChatLog, maxWinX - index, 0, "%s",
                this->_chatHistory.at(this->_chatHistoryIndex - index).c_str());
        }
    }

    wrefresh(this->_wChatLog);
}

void ChatLog::Resize()
{
    this->_chatHistoryIndex = this->_chatHistory.size(); // scroll to bottom
    Redraw();
}