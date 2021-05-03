#include "../setting.h"
#include "../commandline.h"
#include "../chatlog.h"
#include "../log.h"

using namespace std;

void InteractiveDeleteContact()
{
    quickPrintLog(INFO, "Deleting contact...");

    string targetAlias;
    do
    {
        ncOutCmd("Alias (must be exact): ");
        targetAlias = GetConsoleInput(false);
    } while (targetAlias.empty());
    ncOutUsr("Alias: " << targetAlias);

    if (!_iniStructure.has("contact/" + targetAlias))
    {
        quickPrintLog(ERROR, "Contact not found!");
        return;
    }

    _iniStructure.remove("contact/" + targetAlias);
    quickPrintLog(INFO, "Contact deleted.");
}