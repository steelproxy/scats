#ifndef HOTKEY_H
#define HOTKEY_H

#include <map>

typedef void (*t_voidFunction)(void);

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#define logAddHotkey(keycode, func)                                            \
    {                                                                          \
        quickLog(VERBOSE, "Adding hotkey... keycode=" << keycode << " func="   \
                                                      << STRINGIFY(func));     \
        hotkeyMan->AddHotkey(keycode, func);                                   \
    }

class HotkeyManager
{
  public:
    HotkeyManager();

    int  AddHotkey(int keycode, t_voidFunction);
    int  DeleteHotkey(int keycode);

    bool ProcessKey(int keycode);

  private:
    std::map<int, t_voidFunction> _hotkeys;
};

extern HotkeyManager *hotkeyMan;

#endif