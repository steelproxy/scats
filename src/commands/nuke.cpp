#include <string>
#include "../chatlog.h"
#include "../commandline.h"
#include "../setting.h"
#include "../log.h"

using namespace std;

void InteractiveNuke()
{
    string nukePrompt;
    ncOutCmd("Would you like to delete your user files? (y/n): ");
    nukePrompt = GetConsoleInput(false);
    if (nukePrompt == "y")
    {
        ncOutUsr("Deleting log file...");
        if (remove(DEFAULT_LOG_FILE) != 0) // delete log
        {
            ncOutUsr("Unable to delete log file!");
        }

        ncOutUsr("Deleting setting database file...");
        if (remove(DEFAULT_SETTINGS_FILE) != 0) // delete settings
        {
            ncOutUsr("Unable to delete setting database!");
        }

        ncOutUsr("Deleting contact database...");
        if (remove(_iniStructure["General"]["contactDatabasePath"].c_str()) != 0) // delete contact database
        {
            ncOutUsr("Unable to delete contact database!");
        }
    }
}