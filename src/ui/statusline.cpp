#include <unistd.h>
#include "../log/log.h"
#include "../setting/setting.h"
#include "commandline.h"
#include "cursesmode.h"
#include "statusline.h"

StatusLine::StatusLine()
{
    // make status line window
    this->_wStatusLine = newwin(2, getmaxx(stdscr), 0, 0);

    // make status line border
    mvwhline(this->_wStatusLine, 1, 0, 0, getmaxx(stdscr));

    // make status line thread
    pthread_create(&(this->_statusLineThread), NULL, &StatusLine::RedrawHelper, this);
}

void StatusLine::Clear()
{
    wclear(this->_wStatusLine);
    mvwhline(this->_wStatusLine, 1, 0, 0, getmaxx(stdscr));
}

void StatusLine::Resize()
{
    wresize(this->_wStatusLine, 2, getmaxx(stdscr));
}

void* StatusLine::Redraw()
{
    for (;;)
    {
        // clear status line
        this -> Clear();
        
        // get user values
        std::string timestamp = makeTimestamp();
        std::string userHandle = _iniStructure.get("General").get("userHandle");
        std::string connectedStr = (_connected) ? "true" : "false";

        // print statuses
        mvwprintw(this->_wStatusLine, 0, 0, "%s [%s] Connected: %s", timestamp.c_str(), userHandle.c_str(), connectedStr.c_str());

        // print this->_unread status
        if(this->_unread)
        {
            wprintw(this->_wStatusLine, "  *unread messages*");
        }

        wrefresh(this->_wStatusLine);
        RestoreCursorPosition();
        sleep(1);
    }
}

void StatusLine::SetConnect(bool newConnect)
{
    _connected = newConnect;
}

bool StatusLine::GetConnect()
{
    return _connected;
}

void *StatusLine::RedrawHelper(void *context)
{
    return ((StatusLine*)context)->Redraw();
}

bool StatusLine::Unread(bool newUnread)
{
    return this->_unread = newUnread;
}

bool StatusLine::Unread()
{
    return this->_unread;
}

