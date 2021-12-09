#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"

void InteractiveAddSetting()
{
    quickPrintLog(INFO, "Adding setting...");

    std::string newSection;
    do {
        ncOutCmd("Section: ");
        newSection = GetConsoleInput(false);
    } while (newSection.empty());
    ncOutUsr("Section: " << newSection);
    SanitizeINIString(newSection);

    std::string newKey;
    do {
        ncOutCmd("Key: ");
        newKey = GetConsoleInput(false);
    } while (newKey.empty());
    ncOutUsr("Key: " << newKey);
    SanitizeINIString(newKey);

    std::string newValue;
    do {
        ncOutCmd("Value: ");
        newValue = GetConsoleInput(false);
    } while (newValue.empty());
    ncOutUsr("Value: " << newValue);
    SanitizeINIString(newValue);

    _iniStructure[newSection][newKey] = newValue;

    quickPrintLog(INFO, "New setting added " << newSection << ": " << newKey
                                             << "=" << newValue);
}