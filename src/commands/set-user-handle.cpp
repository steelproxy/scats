#include "../commandline.h"
#include "../chatlog.h"
#include "../log.h"
#include "../setting.h"
#include "../cursesmode.h"

using namespace std;

void InteractiveSetUserHandle()
{
    quickPrintLog(INFO, "Setting new user handle...");

    string userHandle;
    ncOutUsr("Please enter a new user handle.");
    ncOutCmd("Handle (max 16 characters, no special characters): ");
    userHandle = GetConsoleInput(false);
    // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        ncOutUsr("Invalid handle, please enter a new one.");
        ncOutCmd("Handle: ");
        userHandle = GetConsoleInput(false); // read a new user handle into userHandle
    }

    _iniStructure["General"]["userHandle"] = userHandle;

    quickPrintLog(INFO, "Set new user handle.");
}