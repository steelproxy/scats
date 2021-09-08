#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"

void InteractiveChangeSetting()
{
    quickPrintLog(INFO, "Changing setting...");

    std::string newValue;
    std::string oldDescription;
    std::string oldDefault;

    std::string targetSection;
    do
    {
        ncOutCmd("Section: ");
        targetSection = GetConsoleInput(false);
    } while (targetSection.empty());
    if (!_iniStructure.has(targetSection))
    {
        quickPrintLog(ERROR, "Unable to find section!");
        return;
    }

    std::string targetKey;
    do
    {
        ncOutCmd("Key: ");
        targetKey = GetConsoleInput(false);
    } while (targetKey.empty());
    if (!_iniStructure.get(targetSection).has(targetKey))
    {
        quickPrintLog(ERROR, "Unable to find key!");
        return;
    }

    ncOutUsr(targetKey << "="
                       << _iniStructure.get(targetSection).get(targetKey));
    do
    {
        ncOutCmd(targetKey << "=");
        newValue = GetConsoleInput(false);
        commandLine->Clear();
        commandLine->PrintPrompt();
    } while (newValue.empty());

    _iniStructure[targetSection][targetKey] = newValue;

    quickPrintLog(INFO, "Changed setting: " << targetKey << "=" << newValue);
}