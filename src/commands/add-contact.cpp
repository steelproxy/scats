#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"
#include "../ui/isprint.h"

void InteractiveAddContact()
{
    quickPrintLog(INFO, "Adding new contact...");

    std::string newAlias;
    do
    {
        ncOutCmd("Alias: ");
        newAlias = GetConsoleInput(false);
        if (!isPrintStr(newAlias) || _iniStructure.has("contact/" + newAlias))
        {
            ncOutUsr("Invalid alias, please enter a new one.");
            continue;
        }
    } while (newAlias.empty());
    ncOutUsr("Alias: " << newAlias);
    SanitizeINIString(newAlias);

    std::string newEndpoint;
    do
    {
        ncOutCmd("Endpoint: ");
        newEndpoint = GetConsoleInput(false);
        if (!isPrintStr(newEndpoint))
        {
            ncOutUsr("Invalid endpoint, please enter a new one.");
            continue;
        }
    } while (newEndpoint.empty());
    ncOutUsr("Endpoint: " << newEndpoint);
    SanitizeINIString(newEndpoint);

    std::string newPort;
    ncOutCmd("Port: ");
    newPort = GetConsoleInput(false);
    try
    {
        (void)stoi(newPort);
    }
    catch (std::exception &e)
    {
        ncOutUsr("Invalid port! Contact not added.");
        exceptionLog(ERROR, e.what());
        return;
    }
    ncOutUsr("Port: " << newPort);
    SanitizeINIString(newPort);

    _iniStructure["contact/" + newAlias]["endpoint"] = newEndpoint;
    _iniStructure["contact/" + newAlias]["port"] = newPort;

    quickPrintLog(INFO, "Added contact.");
}