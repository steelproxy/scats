#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"

void InteractiveDeleteSetting()
{
    quickPrintLog(INFO, "Deleting setting...");

    std::string targetSection;
    do
    {
        ncOutCmd("Section: ");
        targetSection = GetConsoleInput(false);
    } while (targetSection.empty());
    ncOutUsr("Section: " << targetSection);

    std::string targetKey;
    do
    {
        ncOutCmd("Key: ");
        targetKey = GetConsoleInput(false);
    } while (targetKey.empty());
    ncOutUsr("Key: " << targetKey);

    if (_iniStructure[targetSection].has(targetKey))
    {
        _iniStructure[targetSection].remove(targetKey);
        quickPrintLog(INFO, "Setting deleted.");
    }
    else
    {
        quickPrintLog(WARNING, "Unable to find setting!");
    }
}