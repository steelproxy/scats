#include "../log/log.h"
#include "../ui/chatlog/chatlog.h"

void DisplayHelp()
{
    ncOutUsr("Commands: ");
    ncOutUsr("add-contact    Adds a new contact.");
    ncOutUsr("add-setting    Adds a new setting.");
    ncOutUsr("build          Rebuild and restart.");
    ncOutUsr("change-setting Changes a setting");
    ncOutUsr("chat           Starts server.");
    ncOutUsr("delete-contact Deletes a contact.");
    ncOutUsr("delete-setting Deletes a setting.");
    ncOutUsr("exit           Exit scats.");
    ncOutUsr("help           Displays help page.");
    ncOutUsr("list-contacts  Lists all contacts.");
    ncOutUsr("list-settings  Lists all settings.");
    ncOutUsr("list-ini       List all settings in INI file.");
    ncOutUsr("nuke           Erases all user files and exits.");
}