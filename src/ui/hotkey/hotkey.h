#ifndef HOTKEY_H
#define HOTKEY_H

#include <map>

typedef void (*voidFunctionType)(void);

class HotkeyManager
{
  public:
    HotkeyManager();

    int AddHotkey(int keycode, voidFunctionType);
    int DeleteHotkey(int keycode);

    void ProcessKey(int keycode);

  private:
    std::map<int, voidFunctionType> _hotkeys;
};

#endif