#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include "../ui/commandline/commandline.h"

void InteractiveDeleteContact()
{
    quickPrintLog(INFO, "Deleting contact...");

    std::string targetAlias;
    do {
        ncOutCmd("Alias (must be exact): ");
        targetAlias = GetConsoleInput(false);
    } while (targetAlias.empty());
    ncOutUsr("Alias: " << targetAlias);

    if (!_iniStructure.has("contact/" + targetAlias)) {
        quickPrintLog(ERROR, "Contact not found!");
        return;
    }

    _iniStructure.remove("contact/" + targetAlias);
    quickPrintLog(INFO, "Contact deleted.");
}