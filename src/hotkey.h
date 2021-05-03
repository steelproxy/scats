#ifndef HOTKEY_H
#define HOTKEY_H

#include <map>

typedef void (*voidFunctionType)(void); 

class HotkeyManager
{
public:
    HotkeyManager();

    int AddHotkey(int, voidFunctionType);
    int DeleteHotkey(int);
    
    void ProcessKey(int);

private:
    std::map<int, voidFunctionType> hotkeys;
    voidFunctionType hotkeyNotFound;
};

#endif