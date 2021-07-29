#include <curses.h>
#include <signal.h>
#include "../ui/commandline.h"
#include "../ui/chatlog.h"
#include "../log/log.h"
#include "../setting/setting.h"
#include "../scats.h"

void InteractiveEditSettingsCleanup(int sig)
{
    if (sig == SIGINT)
    {
        // restore signal handler
        signal(SIGINT, SIG_DFL);

        quickPrintLog(INFO, "Caught sigint, exiting settings editor...");
        SignalHandler(sig);
    }
}

void InteractiveEditSettings()
{
    // trap sigint
    signal(SIGINT, InteractiveEditSettingsCleanup);

    // create settings editor window
    int maxTermX;
    int maxTermY;
    WINDOW *settingsEditorWin;
    getmaxyx(stdscr, maxTermY, maxTermX);
    settingsEditorWin = newwin(maxTermY - 4, maxTermX - 2, 2, 1);
    if (settingsEditorWin == NULL)
    {
        quickPrintLog(ERROR, "Failed to create settings editor window!");
        return;
    }

    // draw border
    wborder(settingsEditorWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

    // print elements
    int maxWinY;
    int maxWinX;
    int curY;
    int curX;
    int keyCode = 0;
    wmove(settingsEditorWin, 1, 3);
    do
    {
        if(keyCode == KEY_RESIZE)
        {
            wclear(settingsEditorWin);
            wmove(settingsEditorWin, 1, 3);
        }

        for (auto const &it : _iniStructure)
        {
            getmaxyx(settingsEditorWin, maxWinY, maxWinX);
            getyx(settingsEditorWin, curY, curX);
            if (curY < maxWinY)
            {

                auto const &section = it.first;
                auto const &collection = it.second;
                wprintw(settingsEditorWin, "[%s]:", section.c_str());
                wmove(settingsEditorWin, ++curY, 3);
                getyx(settingsEditorWin, curY, curX);
                for (auto const &it2 : collection)
                {
                    if(curY < maxWinY - 2)
                    {
                        auto const &key = it2.first;
                        auto const &value = it2.second;
                        wmove(settingsEditorWin, curY, 3);
                        wprintw(settingsEditorWin, "%s=%s", key.c_str(), value.c_str());
                        wmove(settingsEditorWin, ++curY, 3);
                        wrefresh(settingsEditorWin);
                    }
                }
            }
        }

        do
        {
            keyCode = wgetch(settingsEditorWin);
        } while(keyCode != KEY_ESCAPE || keyCode != KEY_RESIZE);

    } while(keyCode != KEY_ESCAPE);
    wgetch(settingsEditorWin);
    wclear(settingsEditorWin);
    wrefresh(settingsEditorWin);
    if(delwin(settingsEditorWin) == ERR)
    {
        quickLog(ERROR, "Unable to delte settings editor window");
    }
    chatLog->Redraw();
}
