#include "../log/log.h"
#include "../scats.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"
#include "../ui/isprint.h"
#include <curses.h>
#include <signal.h>

class SettingsEditor
{
  public:
    SettingsEditor();
    ~SettingsEditor();
    void Draw();
    void Focus();

  private:
    std::string selected;
    bool edit;
    WINDOW *wSettingsEditor;
};

void Cleanup(int);

SettingsEditor::SettingsEditor()
{
    // trap sigint
    signal(SIGINT, Cleanup);

    // get terminal dimensions
    int maxTermX;
    int maxTermY;
    getmaxyx(stdscr, maxTermY, maxTermX);

    // create settings editor window
    this->wSettingsEditor = newwin(maxTermY - 4, maxTermX - 2, 2, 1);
    if (this->wSettingsEditor == NULL)
    {
        quickPrintLog(ERROR, "Failed to create settings editor window!");
        return;
    }

    // enable keypad
    keypad(this->wSettingsEditor, true);

    // setup selection
    this->selected = _iniStructure.begin()->second.begin()->first;
    this->edit = false;
}

void SettingsEditor::Draw()
{

    // clear editor window
    wclear(this->wSettingsEditor);

    // get terminal dimensions
    int maxTermX;
    int maxTermY;
    getmaxyx(stdscr, maxTermY, maxTermX);

    // get window dimensions
    int maxWinX;
    int maxWinY;
    getmaxyx(this->wSettingsEditor, maxWinY, maxWinX);

    // check if resize needed
    if ((maxTermY - maxWinY) != 4 || (maxTermX - maxWinX) != 2)
    {
        quickLog(VERBOSE, "Resizing settings editor window...");
        if (wresize(this->wSettingsEditor, maxTermY - 4, maxTermX - 2) == ERR)
        {
            quickLog(ERROR, "Unable to resize settings editor window!");
            return;
        }
    }

    // draw border
    wborder(this->wSettingsEditor, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wmove(this->wSettingsEditor, 1, 3);

    int curY;
    int curX;
    int valueX;
    int valueY;
    for (auto const &it : _iniStructure)
    {
        getmaxyx(this->wSettingsEditor, maxWinY, maxWinX);
        getyx(this->wSettingsEditor, curY, curX);
        if (curY < maxWinY)
        {

            auto const &section = it.first;
            auto const &collection = it.second;
            wprintw(this->wSettingsEditor, "[%s]:\n", section.c_str());
            wmove(this->wSettingsEditor, ++curY, 3);
            getyx(this->wSettingsEditor, curY, curX);
            for (auto &it2 : collection)
            {
                if (curY < maxWinY - 2)
                {
                    auto const &key = it2.first;
                    auto const &value = it2.second;
                    wmove(this->wSettingsEditor, curY, 3);
                    if (key == this->selected)
                    {
                        wattron(this->wSettingsEditor, A_REVERSE);
                    }
                    wprintw(this->wSettingsEditor, "%s=", key.c_str());
                    getyx(this->wSettingsEditor, curY, curX);
                    if (key == this->selected)
                    {
                        valueX = curX;
                        valueY = curY;
                    }

                    if (!this->edit || key != this->selected)
                        wprintw(this->wSettingsEditor, "%s", value.c_str());
                    wattrset(this->wSettingsEditor, A_NORMAL);
                    wmove(this->wSettingsEditor, ++curY, 3);
                    wrefresh(this->wSettingsEditor);
                }
            }
        }
    }
    if (this->edit)
        wmove(this->wSettingsEditor, valueY, valueX);
}

void SettingsEditor::Focus()
{
    static auto it = _iniStructure.begin();
    static auto it_s = it->second.begin();
    int keyCode;
    while ((keyCode = wgetch(this->wSettingsEditor)) != KEY_ESCAPE)
    {
        if (keyCode == KEY_DOWN)
        {
            if (it_s < it->second.end() - 1)
                it_s++;
            else if (it < _iniStructure.end() - 1)
            {
                it++;
                it_s = it->second.begin();
            }
        }
        if (keyCode == KEY_UP)
        {
            if (it_s > it->second.begin())
                it_s--;
            else if (it > _iniStructure.begin())
            {
                it--;
                it_s = it->second.end();
            }
        }
        if (keyCode == '\n')
        {
            this->edit = true;
            this->Draw();

            // get cursor position
            int startingXPos;
            int startingYPos;
            getyx(this->wSettingsEditor, startingYPos, startingXPos);

            std::string newValue;
            int newKeyCode;
            int cursorPos = 0;
            while ((newKeyCode = wgetch(this->wSettingsEditor)) != '\n')
            {
                if (newKeyCode == KEY_LEFT)
                {
                    if (cursorPos > 0)
                        cursorPos--;
                    else
                        continue;
                }
                else if (newKeyCode == KEY_RIGHT)
                {
                    if (static_cast<unsigned int>(cursorPos) <
                        newValue.length() - 1)
                        cursorPos++;
                    else
                        continue;
                }
                else if (newKeyCode == KEY_BACKSPACE)
                {
                    if (cursorPos > 0 && newValue.length() > 1)
                    {
                        newValue.erase(--cursorPos);
                    }
                }
                else if (isPrintKey(newKeyCode))
                {
                    newValue.insert(cursorPos + newValue.begin(),
                                    (char)newKeyCode);
                    cursorPos++;
                }
                wmove(this->wSettingsEditor, startingYPos, startingXPos);
                for (unsigned int index = 0; index < newValue.length(); index++)
                {
                    if (index == static_cast<unsigned int>(cursorPos))
                    {
                        wattron(this->wSettingsEditor, A_REVERSE);
                    }
                    waddch(this->wSettingsEditor, newValue.at(index));
                    wattroff(this->wSettingsEditor, A_REVERSE);
                }
                wmove(this->wSettingsEditor, startingYPos,
                      startingXPos + cursorPos);
                wrefresh(this->wSettingsEditor);
            }

            _iniStructure[it->first][it_s->first] = newValue;
            this->edit = false;
        }
        this->selected = it_s->first;
        this->Draw();
    }
}

SettingsEditor::~SettingsEditor()
{
    if (delwin(this->wSettingsEditor) == ERR)
    {
        quickLog(ERROR, "Unable to delte settings editor window");
    }
    Cleanup(0);
}

void Cleanup(int sig)
{

    signal(SIGINT, SIG_DFL);

    quickPrintLog(INFO, "Caught sigint, exiting settings editor...");

    chatLog->Redraw();
    SignalHandler(sig);
}

void InteractiveEditSettings()
{

    // print elements
    SettingsEditor seMain;
    seMain.Draw();
    seMain.Focus();
}
