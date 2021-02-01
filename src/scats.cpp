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

WINDOW *root;
Log logger;
sigjmp_buf sigintJumpPoint;

void SignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        ncoutln(endl << "Sigint caught.");
        quickLog(INFO, "Sigint caught.");
        siglongjmp(sigintJumpPoint, 1);
    }
}

void DisplayHelp()
{
    ncoutln("Commands: ");
    ncoutln("add-contact    Adds a new contact.");
    ncoutln("add-setting    Adds a new setting.");
    ncoutln("build          Rebuild and restart.");
    ncoutln("change-setting Changes a setting");
    ncoutln("chat           Starts server.");
    ncoutln("delete-contact Deletes a contact.");
    ncoutln("delete-setting Deletes a setting.");
    ncoutln("exit           Exit scats.");
    ncoutln("help           Displays help page.");
    ncoutln("list-contacts  Lists all contacts.");
    ncoutln("list-settings  Lists all settings.");
    ncoutln("nuke           Erases all user files and exits.");
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
        ncoutln("Log file does not exist!");
        ncoutln("Creating log file...");
        logFile.open(DEFAULT_LOG_FILE, ios::out);
        if (logFile.fail())
        {
            ncoutln("Unable to create log file!");
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
        ncoutln("Unable to open log file!"); // print error if fail
    }

    // start scats
    ncoutln("Starting scats...");

    // create settings file if necessary
    bool newSettingFile = false;
    quickPrintLog(INFO, "Loading setting database...");
    if (!FileExists(DEFAULT_SETTINGS_FILE))
    {
        ofstream settingFile;
        newSettingFile = true;
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

        // user prompt
        string userInput;
        userInput = string(); // clear string
        addstr("[");
        addstr(getSet(settingDatabase, "userHandle").c_str());
        addstr("] > ");
        GetConsoleInput(root, true, userInput);

        // extract command substring
        string commandSubStr;
        if (userInput.empty() || userInput.at(0) != '/') // check if input is a command
        {
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
            quickPrintLog(INFO, "Adding new contact...");
            InteractiveAddContact(contactDatabase);
        }
        else if (commandSubStr == "delete-contact") // delete a contact
        {
            quickPrintLog(INFO, "Deleting contact...");
            InteractiveDeleteContact(contactDatabase);
        }
        else if (commandSubStr == "list-contacts") // list all contacts
        {
            ncoutln("Contacts: ");
            for (size_t index = 0; index < contactDatabase.getLength(); index++) // read contacts from database
            {
                ncoutln("[" << contactDatabase.getIndex(index).getAlias() << "]");
                ncoutln("   " << contactDatabase.getIndex(index).getEndpoint());
                ncoutln("   " << contactDatabase.getIndex(index).getPort());
            }
        }
        else if (commandSubStr == "add-setting") // add a new setting
        {
            quickPrintLog(INFO, "Adding new setting...");
            InteractiveAddSetting(settingDatabase);
        }
        else if (commandSubStr == "delete-setting") // delete a setting
        {
            quickPrintLog(INFO, "Deleting setting...");
            InteractiveDeleteSetting(settingDatabase);
        }
        else if (commandSubStr == "list-settings") // list all settings
        {
            ncoutln("Settings: ");
            for (size_t index = 0; index < settingDatabase.getLength(); index++) // read settings from database
            {
                ncoutln(settingDatabase.getIndex(index).getKey() << "=" << settingDatabase.getIndex(index).getValue());
                ncoutln("    " << settingDatabase.getIndex(index).getDescription());
            }
        }
        else if (commandSubStr == "nuke") // nuke files
        {
            string nukePrompt;
            ncout("Would you like to delete your user files? (y/n): ");
            GetConsoleInput(root, false, nukePrompt);
            if (nukePrompt == "y")
            {
                ncoutln("Deleting log file...");
                if (remove(DEFAULT_LOG_FILE) != 0) // delete log
                {
                    ncoutln("Unable to delete log file!");
                }

                ncoutln("Deleting setting database file...");
                if (remove(DEFAULT_SETTINGS_FILE) != 0) // delete settings
                {
                    ncoutln("Unable to delete setting database!");
                }

                ncoutln("Deleting contact database...");
                if (remove(getSet(settingDatabase, "contactDatabasePath").c_str()) != 0) // delete contact database
                {
                    ncoutln("Unable to delete contact database!");
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
            quickPrintLog(INFO, "Changing settings...");
            InteractiveChangeSetting(settingDatabase);
        }
        else if (commandSubStr == "clear") // clear screen
        {
            clear();
        }
        else if (commandSubStr == "save-contacts") // save contacts
        {
            quickPrintLog(INFO, "Saving contact file...");
            contactDatabase.save();
        }
        else if (commandSubStr == "chat") // start chat server
        {
            boost::asio::io_service ioService;
            // boost::asio::streambuf readBuf;
            ncoutln("What port would you like to start the server on? ");
            GetConsoleInput(root, false, commandSubStr);
            quickPrintLog(INFO, "Starting server on port: " << "...");
            ChatServer testSrv(ioService, 25565);
            for (;;)
                ;
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
            ncoutln("Unknown command!");
        }
    }
}
