#include "../commandline.h"
#include "../chatlog.h"
#include "../log.h"
#include "../setting.h"

using namespace std;

void InteractiveAddSetting()
{
    quickPrintLog(INFO, "Adding setting...");

    string newSection;
    do
    {
        ncOutCmd("Section: ");
        newSection = GetConsoleInput(false);
    } while (newSection.empty());
    ncOutUsr("Section: " << newSection);
    SanitizeINIString(newSection);

    string newKey;
    do
    {
        ncOutCmd("Key: ");
        newKey = GetConsoleInput(false);
    } while (newKey.empty());
    ncOutUsr("Key: " << newKey);
    SanitizeINIString(newKey);

    string newValue;
    do
    {
        ncOutCmd("Value: ");
        newValue = GetConsoleInput(false);
    } while (newValue.empty());
    ncOutUsr("Value: " << newValue);
    SanitizeINIString(newValue);

    _iniStructure[newSection][newKey] = newValue;

    quickPrintLog(INFO, "New setting added " << newSection << ": " << newKey << "=" << newValue);
}