#ifndef STATUSLINE_H
#define STATUSLINE_H

#include <curses.h>
#include <pthread.h>

class StatusLine
{
public:
    StatusLine();

    void *Redraw();
    static void *RedrawHelper(void* context);

    bool Unread();
    bool Unread(bool newUnread); 

    void SetConnect(bool);
    bool GetConnect();

    void Resize();

private:
    void Clear();
    
    pthread_t statusLineThread;
    WINDOW* wStatusLine;
    WINDOW* wRoot;
    bool unread;
    bool connected;
};

extern StatusLine *statusLine;

#endif