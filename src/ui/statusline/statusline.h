#ifndef STATUSLINE_H
#define STATUSLINE_H

#include <curses.h>
#include <pthread.h>

// dimensions
#define STATUSLINE_Y_LEN 2
#define STATUSLINE_X_LEN getmaxx(stdscr)

class StatusLine
{
  public:
    StatusLine();

    void *Redraw();

    bool  Unread();
    bool  Unread(bool newUnread);

    void  SetConnect(bool newConnected);
    bool  GetConnect();

    void  Resize();

  private:
    void         Clear();
    static void *RedrawHelper(void *context);

    pthread_t    _statusLineThread;
    WINDOW      *_wStatusLine;
    bool         _unread;
    bool         _connected;
};

extern StatusLine *statusLine;

#endif