#include "../commandline.h"
#include "../chatlog.h"
#include "../log.h"
#include "../setting.h"

using namespace std;

void InteractiveDeleteSetting()
{
    quickPrintLog(INFO, "Deleting setting...");

    string targetSection;
    do
    {
        ncOutCmd("Section: ");
        targetSection = GetConsoleInput(false);
    } while (targetSection.empty());
    ncOutUsr("Section: " << targetSection);

    string targetKey;
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