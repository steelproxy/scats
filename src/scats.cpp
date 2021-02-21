/** 
 *  @file   scats.cpp
 *  @brief  Main program file.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
#include <pthread.h>
#include <setjmp.h>
#include <cstdlib>
#include <signal.h>
#include <cstring>
#include "setting.h"
#include "contact.h"
#include "log.h"
#include "server.h"
#include "interactive.h"
#include "cursesmode.h"
#include <curses.h>

#define DEFAULT_CONTACTS_FILE "contacts.txt"
#define DEFAULT_SETTINGS_FILE "settings.txt"
#define DEFAULT_ENABLE_SHELL "false"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_FILE "log.txt"
#define DEFAULT_TRUNCATE_LOG "false"
#define DEFAULT_HISTORY_LEN "64"

using namespace std;

// initialize command list
const std::string commands[] = {"add-contact", "add-setting", "build", "change-setting",
                                "chat", "clear", "delete-contact", "delete-setting", "exit", "help",
                                "list-contacts", "list-settings", "nuke", "save-contacts", "save-settings"};
const size_t commandsLen = 15;
vector<string> commandHistory;

WINDOW *wRoot;
WINDOW *wStatusLine;
WINDOW *wUserLog;
WINDOW *wCommandLine;

Log logger;
sigjmp_buf sigintJumpPoint;

void SignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        quickPrintLog(INFO, "Sigint caught.");
        siglongjmp(sigintJumpPoint, 1);
    }
}

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
    ncOutUsr("nuke           Erases all user files and exits.");
}

void Cleanup()
{
    endwin();       // end curses mode
    logger.close(); // close log file
}

int main(int argc, char **argv)
{
    // register cleanup function
    atexit(Cleanup);

    // start curses mode
    StartCurses();

    // create log file if necessary
    if (!FileExists(DEFAULT_LOG_FILE))
    {
        ofstream logFile;
        ncOutUsr("Log file not found at: " << DEFAULT_LOG_FILE);
        ncOutUsr("Log file does not exist!");
        ncOutUsr("Creating log file...");
        logFile.open(DEFAULT_LOG_FILE, ios::out);
        if (logFile.fail())
        {
            ncOutUsr("Unable to create log file!");
        }
        logFile.close();
    }

    // initialize logger
    try
    {
        logger.open(DEFAULT_LOG_FILE);
        logger.truncate();
    }
    catch (const char *msg)
    {
        ncOutUsr("Unable to open log file!"); // print error if fail
    }

    // start scats
    quickPrintLog(INFO, "Starting scats...");

    // create settings file if necessary
    bool newSettingFile = false;
    quickPrintLog(INFO, "Loading setting database...");
    if (!FileExists(DEFAULT_SETTINGS_FILE))
    {
        ofstream settingFile;
        newSettingFile = true;
        quickPrintLog(VERBOSE, "Setting database file not found at: " << DEFAULT_SETTINGS_FILE);
        quickPrintLog(WARNING, "Setting database does not exist!");
        quickPrintLog(INFO, "Creating setting database...");
        settingFile.open(DEFAULT_SETTINGS_FILE, ios::out);
        if (settingFile.fail())
        {
            quickPrintLog(ERROR, "Unable to create setting database!");
        }
        else
        {
            newSettingFile = true;
        }
        settingFile.close();
    }

    // load settings
    SettingDB settingDatabase;
    try
    {
        settingDatabase.open(DEFAULT_SETTINGS_FILE);
        settingDatabase.load();
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to load setting database!");
        exceptionLog(ERROR, msg);
    }

    // apply settings
    DefSet("contactDatabasePath", DEFAULT_CONTACTS_FILE, "Path to contact database.");
    DefSet("logLevel", DEFAULT_LOG_LEVEL, "Minimum severity level to print to log.");
    DefSet("commandHistoryLength", DEFAULT_HISTORY_LEN, "Maximum number of commands to be kept in history.");
    logger.setLevel(LevelToI(getSet(settingDatabase, "logLevel")));
    IfSet("userHandle")
    {
        InteractiveSetUserHandle(settingDatabase);
    }
    if (newSettingFile)
    {
        try
        {
            settingDatabase.save();
        }
        catch (const char *msg)
        {
            quickPrintLog(ERROR, "Unable to save settings database!");
            exceptionLog(ERROR, msg);
        }
    }
    // TODO fix resize
    // get command history vector length
    size_t commandHistoryLen = 0;
    try
    {
        commandHistoryLen = stoi(getSet(settingDatabase, "commandHistoryLength"));
    }
    catch (const std::exception &e)
    {
        quickLog(ERROR, "Invalid value for commandHistoryLength!");
        exceptionLog(ERROR, e.what());
        settingDatabase.searchKey("commandHistoryLength").setValue(DEFAULT_HISTORY_LEN);
        commandHistoryLen = stoi(DEFAULT_HISTORY_LEN);
    }

    // create contacts file if necessary
    if (!FileExists(getSet(settingDatabase, "contactDatabasePath")))
    {
        ofstream contactFile;
        quickPrintLog(VERBOSE, "Contact database file not found at: " << getSet(settingDatabase, "contactDatabasePath"));
        quickPrintLog(WARNING, "Contact database does not exist!");
        quickPrintLog(INFO, "Creating contact database.");
        contactFile.open(getSet(settingDatabase, "contactDatabasePath"), ios::out);
        if (contactFile.fail())
        {
            quickPrintLog(ERROR, "Unable to create contact database!");
        }
        contactFile.close();
    }

    // load contacts
    ContactDB contactDatabase;
    quickPrintLog(INFO, "Loading contact database...");
    try
    {
        contactDatabase.open(getSet(settingDatabase, "contactDatabasePath"));
        contactDatabase.load();
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to load contact database!");
        exceptionLog(ERROR, msg);
    }

    // capture SIGINT's
    signal(SIGINT, SignalHandler);

    // interactive loop
    for (;;)
    {
        while (sigsetjmp(sigintJumpPoint, 1) != 0)
            ; // set jump to beginning of loop

        // status line
        wmove(wStatusLine, 0, 0);
        wclrtoeol(wStatusLine);
        wprintw(wStatusLine, "%s [%s] Connected: %s", makeTimestamp().c_str(), getSet(settingDatabase, "userHandle").c_str(), (connectedToServer()) ? "true" : "false");
        wrefresh(wStatusLine);

        wrefresh(wCommandLine);
        // user prompt
        string userInput;
        userInput = string(); // clear string
        ncOutCmd("[" << getSet(settingDatabase, "userHandle").c_str() << "]: ");
        GetConsoleInput(true, userInput);

        // extract command substring
        string commandSubStr;
        if (userInput.empty() || userInput.at(0) != '/') // check if input is a command
        {
            if(connectedToServer() && !userInput.empty())
            {
                SendChat(userInput);
                ncOutUsr("[" << getSet(settingDatabase, "userHandle").c_str() << "]: " << userInput);
                continue;
            }
            quickPrintLog(ERROR, "Not connected to chat!");
            continue;
        }
        commandSubStr = userInput.substr(1); // extract command after '/'

        // push command history
        if (commandHistory.size() < commandHistoryLen) // if there is room left in the history vector
        {
            commandHistory.push_back(commandSubStr); // push back last command
        }
        else
        {
            commandHistory.erase(commandHistory.begin()); // erase first command in history vector
            commandHistory.push_back(commandSubStr);      // push back last command
        }

        if (commandSubStr == "help") // display help
        {
            DisplayHelp();
        }
        else if (commandSubStr == "add-contact") // add a contact
        {
            InteractiveAddContact(contactDatabase);
        }
        else if (commandSubStr == "delete-contact") // delete a contact
        {
            InteractiveDeleteContact(contactDatabase);
        }
        else if (commandSubStr == "list-contacts") // list all contacts
        {
            ncOutUsr("Contacts: ");
            for (size_t index = 0; index < contactDatabase.getLength(); index++) // read contacts from database
            {
                ncOutUsr("[" << contactDatabase.getIndex(index).getAlias() << "]");
                ncOutUsr("   " << contactDatabase.getIndex(index).getEndpoint());
                ncOutUsr("   " << contactDatabase.getIndex(index).getPort());
            }
        }
        else if (commandSubStr == "add-setting") // add a new setting
        {
            InteractiveAddSetting(settingDatabase);
        }
        else if (commandSubStr == "delete-setting") // delete a setting
        {
            InteractiveDeleteSetting(settingDatabase);
        }
        else if (commandSubStr == "list-settings") // list all settings
        {
            ncOutUsr("Settings: ");
            for (size_t index = 0; index < settingDatabase.getLength(); index++) // read settings from database
            {
                ncOutUsr(settingDatabase.getIndex(index).getKey() << "=" << settingDatabase.getIndex(index).getValue());
                ncOutUsr("    " << settingDatabase.getIndex(index).getDescription());
            }
        }
        else if (commandSubStr == "nuke") // nuke files
        {
            string nukePrompt;
            ncOutCmd("Would you like to delete your user files? (y/n): ");
            GetConsoleInput(false, nukePrompt);
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
                if (remove(getSet(settingDatabase, "contactDatabasePath").c_str()) != 0) // delete contact database
                {
                    ncOutUsr("Unable to delete contact database!");
                }
                return 0; // exit
            }
        }
        else if (commandSubStr == "save-settings") // save settings
        {
            quickPrintLog(INFO, "Saving settings file...");
            settingDatabase.save();
        }
        else if (commandSubStr == "change-setting") // change a setting
        {
            InteractiveChangeSetting(settingDatabase);
        }
        else if (commandSubStr == "clear") // clear screen
        {
            wclear(wUserLog);
            wmove(wUserLog, 0, 0);
            wrefresh(wUserLog);
        }
        else if (commandSubStr == "save-contacts") // save contacts
        {
            quickPrintLog(INFO, "Saving contact file...");
            contactDatabase.save();
        }
        else if (commandSubStr == "server") // start chat server
        {
            quickPrintLog(INFO, "Starting server on port: " << "...");
            StartServer(25565);
        }
        else if (commandSubStr == "client")
        {
            quickPrintLog(INFO, "Connecting to server at port 25565");
            ConnectServer(25565);
        }
        else if (commandSubStr == "exit") // exit scats
        {
            quickPrintLog(INFO, "Exiting scats...");
            return 0;
        }
        else if (commandSubStr == "build") // rebuild scats
        {
            quickPrintLog(INFO, "Exiting, rebuilding, and relaunching scats...");
            system("cd ..; make; cd bin; ./scats");
            return 0;
        }
        else
        {
            ncOutUsr("Unknown command!");
        }
    }
}
