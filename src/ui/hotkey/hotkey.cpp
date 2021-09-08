#include "hotkey.h"
#include "../../log/log.h"

HotkeyManager::HotkeyManager() {}

int HotkeyManager::AddHotkey(int key, voidFunctionType func)
{
    for (auto iterator = this->_hotkeys.begin();
         iterator != this->_hotkeys.end(); iterator++)
    {
        if (iterator->first == key)
        {
            quickLog(ERROR, "Hotkey " << key << " already bound.");
            return 1;
        }
    }

    this->_hotkeys.insert({key, func});
    quickLog(INFO, "Hotkey " << key << " has been bound.");
    return 0;
}

int HotkeyManager::DeleteHotkey(int key)
{
    for (auto iterator = this->_hotkeys.begin();
         iterator != this->_hotkeys.end(); iterator++)
    {
        if (iterator->first == key)
        {
            this->_hotkeys.erase(iterator);
            quickLog(INFO, "Hotkey " << key << " erased.");
            return 0;
        }
    }

    quickLog(ERROR,
             "Hotkey " << key << " was not bound and hasn't been deleted.");
    return 1;
}

void HotkeyManager::ProcessKey(int key)
{
    for (auto iterator = this->_hotkeys.begin();
         iterator != this->_hotkeys.end(); iterator++)
    {
        if (iterator->first == key)
        {
            quickLog(INFO, "Hotkey " << key << " executing...");
            iterator->second();
            return;
        }
    }

    quickLog(ERROR,
             "Hotkey " << key << " was not bound and hasn't been executed.");
}