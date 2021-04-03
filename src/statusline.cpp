#include "statusline.h"
#include "log.h"
#include "setting.h"
#include "server.h"
#include "commandline.h"
#include "cursesmode.h"

StatusLine::StatusLine()
{
    // make status line window
    wStatusLine = newwin(2, getmaxx(stdscr), 0, 0);

    mvwhline(wStatusLine, 1,0,0, '_');
    // make status line thread
    pthread_create(&statusLineThread, NULL, &StatusLine::RedrawHelper, this);
}

void StatusLine::Clear()
{
    wmove(this->wStatusLine, 0, 0);
    wclrtoeol(this->wStatusLine);
}

void* StatusLine::Redraw()
{
    for (;;)
    {
        this -> Clear();
        
        // get user values
        std::string timestamp = makeTimestamp();
        std::string userHandle = getSet("userHandle");
        std::string connected = (connectedToServer()) ? "true" : "false";

        // print statuses
        wprintw(wStatusLine, "%s [%s] Connected: %s", timestamp.c_str(), userHandle.c_str(), connected.c_str());
        
        if(unread)
        {
            wprintw(wStatusLine, "  *unread messages*");
        }

        wrefresh(wStatusLine);
        RestoreCursorPosition();
        sleep(1);
    }
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