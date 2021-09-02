#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog.h"
#include "../ui/commandline.h"
#include "../ui/cursesmode.h"

void InteractiveSetUserHandle()
{
    quickPrintLog(INFO, "Setting new user handle...");

    std::string userHandle;
    ncOutUsr("Please enter a new user handle.");
    ncOutCmd("Handle (max 16 characters, no special characters): ");
    userHandle = GetConsoleInput(false);
    // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 ||
           !isPrintStr(userHandle)) // must be less than 16 characters and be
                                    // only printable characters
    {
        ncOutUsr("Invalid handle, please enter a new one.");
        ncOutCmd("Handle: ");
        userHandle =
            GetConsoleInput(false); // read a new user handle into userHandle
    }

    _iniStructure["General"]["userHandle"] = userHandle;

    quickPrintLog(INFO, "Set new user handle.");
}