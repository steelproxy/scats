#include "hotkey.h"
#include "log.h"

HotkeyManager::HotkeyManager()
{

}

int HotkeyManager::AddHotkey(int key, voidFunctionType func)
{
    for(auto iterator = hotkeys.begin(); iterator != hotkeys.end(); iterator++)
    {
        if(iterator -> first == key)
        {
            quickLog(ERROR, "Hotkey " << key << " already bound.");
            return 1;
        }
    }

    hotkeys.insert({key, func});
    quickLog(INFO, "Hotkey " << key << " has been bound.");
}

int HotkeyManager::DeleteHotkey(int key)
{
    for(auto iterator = hotkeys.begin(); iterator != hotkeys.end(); iterator++)
    {
        if(iterator -> first == key)
        {
            hotkeys.erase(iterator);
            quickLog(INFO, "Hotkey " << key << " erased.");
            return 0;
        }
    }

    quickLog(ERROR, "Hotkey " << key << " was not bound and hasn't been deleted.");
    return -1;
}

void HotkeyManager::ProcessKey(int key)
{
    for(auto iterator = hotkeys.begin(); iterator != hotkeys.end(); iterator++)
    {
        if(iterator -> first == key)
        {
            quickLog(INFO, "Hotkey " << key << " executing...");
            iterator -> second();
            return;
        }
    }

    quickLog(ERROR, "Hotkey " << key << " was not bound and hasn't been executed.");
}