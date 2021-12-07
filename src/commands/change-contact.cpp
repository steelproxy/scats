/*#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"

void InteractiveChangeSetting()
{
    quickPrintLog(INFO, "Changing contact...");

    std::string targetAlias;
    do
    {
        ncOutCmd("Alias: ");
        targetAlias = GetConsoleInput(false);
    } while (targetAlias.empty());

    if (!_iniStructure.has("contact/" + targetAlias))
    {
        quickPrintLog(ERROR, "Unable to find contact!");
        return;
    }

    std::string newAlias;
    std::string newEndpoint;
    std::string newPort;
    do
    {
        ncOutCmd("Alias: ");
        newAlias = GetConsoleInput(false);
        ncOutCmd("Endpoint: ");

    } while (targetKey.empty());


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
}*/