#include "statusline.h"
#include "log.h"
#include "setting.h"
#include "commandline.h"
#include "cursesmode.h"
#include <unistd.h>

StatusLine::StatusLine()
{
    // make status line window
    wStatusLine = newwin(2, getmaxx(stdscr), 0, 0);

    mvwhline(wStatusLine, 1,0,0, getmaxx(stdscr));
    // make status line thread
    pthread_create(&statusLineThread, NULL, &StatusLine::RedrawHelper, this);
}

void StatusLine::Clear()
{
    mvwhline(wStatusLine, 1,0,0, getmaxx(stdscr));
    wmove(this->wStatusLine, 0, 0);
    wclrtoeol(this->wStatusLine);
}

void StatusLine::Resize()
{
    int maxX;
    maxX = getmaxx(wStatusLine);

    wresize(wStatusLine, 2, maxX);
    wrefresh(wStatusLine);
}

void* StatusLine::Redraw()
{
    for (;;)
    {
        this -> Clear();
        
        // get user values
        std::string timestamp = makeTimestamp();
        std::string userHandle = _iniStructure["General"]["userHandle"];
        std::string connectedStr = (connected) ? "true" : "false";

        // print statuses
        wprintw(wStatusLine, "%s [%s] Connected: %s", timestamp.c_str(), userHandle.c_str(), connectedStr.c_str());
        
        if(unread)
        {
            wprintw(wStatusLine, "  *unread messages*");
        }

        wrefresh(wStatusLine);
        RestoreCursorPosition();
        sleep(1);
    }
}

void StatusLine::SetConnect(bool newConnect)
{
    connected = newConnect;
}

bool StatusLine::GetConnect()
{
    return connected;
}

void *StatusLine::RedrawHelper(void *context)
{
    return ((StatusLine*)context)->Redraw();
}

bool StatusLine::Unread(bool newUnread)
{
    return unread = newUnread;
}

bool StatusLine::Unread()
{
    return unread;
}

