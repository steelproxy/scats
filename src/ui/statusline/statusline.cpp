#include "statusline.h"
#include "../../log/log.h"
#include "../../setting/setting.h"
#include "../commandline/commandline.h"
#include "../isprint.h"
#include <unistd.h>

StatusLine::StatusLine()
{
    // make status line window
    this->_wStatusLine = newwin(STATUSLINE_Y_LEN, STATUSLINE_X_LEN, 0, 0);

    // make status line border
    mvwhline(this->_wStatusLine, 1, 0, 0, STATUSLINE_X_LEN);

    // make status line thread
    pthread_create(&(this->_statusLineThread), NULL, &StatusLine::RedrawHelper,
                   this);
}

void StatusLine::Clear()
{
    wclear(this->_wStatusLine);
    mvwhline(this->_wStatusLine, 1, 0, 0, STATUSLINE_X_LEN);
}

void StatusLine::Resize() { wresize(this->_wStatusLine, 2, getmaxx(stdscr)); }

void *StatusLine::Redraw()
{
    for (;;)
    {
        // clear status line
        this->Clear();

        // get max terminal dimensions
        int maxTermY;
        int maxTermX;
        (void)maxTermY;
        getmaxyx(this->_wStatusLine, maxTermY, maxTermX);

        // get max window dimensions
        int maxWinY;
        int maxWinX;
        getmaxyx(this->_wStatusLine, maxWinY, maxWinX);

        // resize window if necessary
        if (maxWinY < 2 || (maxWinX - maxTermX) != 0)
        {
            if (wresize(this->_wStatusLine, 2, maxTermX) == ERR)
            {
                quickLog(ERROR, "Unable to resize status line window!");
                return nullptr;
            }
        }

        // get user values
        std::string timestamp = makeTimestamp();
        std::string userHandle = _iniStructure.get("General").get("userHandle");
        std::string connectedStr = (_connected) ? "true" : "false";

        // print statuses
        mvwprintw(this->_wStatusLine, 0, 0, "%s [%s] Connected: %s",
                  timestamp.c_str(), userHandle.c_str(), connectedStr.c_str());

        // print this->_unread status
        if (this->_unread)
        {
            wprintw(this->_wStatusLine, "  *unread messages*");
        }

        wrefresh(this->_wStatusLine);
        // RestoreCursorPosition();
        sleep(1);
    }
}

void StatusLine::SetConnect(bool newConnect) { _connected = newConnect; }

bool StatusLine::GetConnect() { return _connected; }

void *StatusLine::RedrawHelper(void *context)
{
    return ((StatusLine *)context)->Redraw();
}

bool StatusLine::Unread(bool newUnread) { return this->_unread = newUnread; }

bool StatusLine::Unread() { return this->_unread; }
