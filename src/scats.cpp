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
#include "network.h"
#include "interactive.h"
#include "cursesmode.h"
#include <curses.h>

#define IfSet(value, key) if ((value = settingDatabase.SearchKey(key).GetValue()) == string())

#define DefSet(value, key, default, description)                         \
    if ((value = settingDatabase.SearchKey(key).GetValue()) == string()) \
    {                                                                    \
        value = default;                                                 \
        settingDatabase.AddSetting(Setting(key, default, description));  \
    }

#define DEFAULT_CONTACTS_FILE "contacts.txt"
#define DEFAULT_SETTINGS_FILE "settings.txt"
#define DEFAULT_ENABLE_SHELL "false"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_FILE "log.txt"
#define DEFAULT_TRUNCATE_LOG "false"
#define DEFAULT_HISTORY_LEN 64

using namespace std;

Log logger;
sigjmp_buf begin_loop;

void SignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        quickPrintLog(INFO, "Sigint recieved.");
        siglongjmp(begin_loop, 1);
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

int LevelToI(string level)
{
    if (level == "verbose")
        return VERBOSE;
    else if (level == "info")
        return INFO;
    else if (level == "warning")
        return WARNING;
    else if (level == "error")
        return ERROR;
    else if (level == "severe")
        return SEVERE;
    else
        return INFO;
}

void Cleanup()
{
    endwin(); // end curses mode
    logger.Close(); // close log file
}

int main(int argc, char **argv)
{
    // register cleanup function
    atexit(Cleanup);

    // start curses mode
    StartCurses();

    // initialize command list
    commands = {"add-contact", "add-setting", "build", "change-setting",
                "chat", "clear", "delete-contact", "delete-setting", "exit", "help",
                "list-contacts", "list-settings", "nuke", "save-contacts", "save-settings"};

    // create log file if necessary
    if (!FileExists(DEFAULT_LOG_FILE))
    {
        ofstream logFile;
        ncoutln("Log file does not exist!");
        ncoutln("Creating log file...");
        logFile.open(DEFAULT_LOG_FILE, ios::out);
        if (logFile.fail())
            ncoutln("Unable to create log file!");
        logFile.close();
    }

    // initialize logger
    if (logger.Open(DEFAULT_LOG_FILE)) 
    {
        ncoutln("error: Unable to open log file!"); // print error if fail
    }
    logger.Truncate();

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
            newSettingFile = true;
        settingFile.close();
    } 

    // load settings
    SettingDB settingDatabase;
    string contactDatabasePath;
    string enableShell;
    string logLevel;
    string userHandle;
    if (settingDatabase.Open(DEFAULT_SETTINGS_FILE) ||
        settingDatabase.Load())
        quickPrintLog(ERROR, "Unable to load setting database!");

    // apply settings
    DefSet(contactDatabasePath, "contactDatabasePath", DEFAULT_CONTACTS_FILE, "Path to contact database.");
    DefSet(enableShell, "enableShell", DEFAULT_ENABLE_SHELL, "Enable shell access from within scats.");
    DefSet(logLevel, "logLevel", DEFAULT_LOG_LEVEL, "Minimum severity level to print to log.");
    logger.SetLevel(LevelToI(logLevel));
    IfSet(userHandle, "userHandle")
        InteractiveSetUserHandle(settingDatabase, userHandle);
    if (newSettingFile)
    {
        if (settingDatabase.Save())
            quickPrintLog(ERROR, "Unable to save settings database!");
    }

    // create contacts file if necessary
    if (!FileExists(contactDatabasePath))
    {
        ofstream contactFile;
        quickPrintLog(WARNING, "Contact database does not exist!");
        quickPrintLog(INFO, "Creating contact database.");
        contactFile.open(contactDatabasePath, ios::out);
        if (contactFile.fail())
            quickPrintLog(ERROR, "Unable to create contact database!");
        contactFile.close();
    }

    // load contacts
    ContactDB contactDatabase;
    quickPrintLog(INFO, "Loading contact database...");
    if (contactDatabase.Open(contactDatabasePath) ||
        contactDatabase.Load())
        quickPrintLog(ERROR, "Unable to load contact database!");

    signal(SIGINT, SignalHandler);

    // interactive loop
    for (;;)
    {
        string userInput;

        while (sigsetjmp(begin_loop, 1) != 0)
            ; // set jump to beginning of loop

        userInput = string(); // clear string

        // print and prompt
        addstr("[");
        addstr(userHandle.c_str());
        addstr("] > ");
        GetConsoleInput(userInput);

        if (userInput == "help") // display help
        {
            DisplayHelp();
        }
        else if (userInput == "add-contact") // add a contact
        {
            quickPrintLog(INFO, "Adding new contact...");
            InteractiveAddContact(contactDatabase);
        }
        else if (userInput == "delete-contact") // delete a contact
        {
            quickPrintLog(INFO, "Deleting contact...");
            InteractiveDeleteContact(contactDatabase);
        }
        else if (userInput == "list-contacts") // list all contacts
        {
            ncoutln("Contacts: ");
            for (size_t index = 0; index < contactDatabase.GetLength(); index++) // read contacts from database
            {
                ncoutln("[" << contactDatabase.GetIndex(index).GetAlias() << "]");
                ncoutln("   " << contactDatabase.GetIndex(index).GetEndpoint());
                ncoutln("   " << contactDatabase.GetIndex(index).GetPort());
            }
        }
        else if (userInput == "add-setting") // add a new setting
        {
            quickPrintLog(INFO, "Adding new setting...");
            InteractiveAddSetting(settingDatabase);
        }
        else if (userInput == "delete-setting") // delete a setting
        {
            quickPrintLog(INFO, "Deleting setting...");
            InteractiveDeleteSetting(settingDatabase);
        }
        else if (userInput == "list-settings") // list all settings
        {
            ncoutln("Settings: ");
            for (size_t index = 0; index < settingDatabase.GetLength(); index++) // read settings from database
            {
                ncoutln(settingDatabase.GetIndex(index).GetKey() << "=" << settingDatabase.GetIndex(index).GetValue());
                ncoutln("    " << settingDatabase.GetIndex(index).GetDescription());
            }
        }
        else if (userInput == "nuke") // nuke files
        {
            string nukePrompt;
            ncout("Would you like to delete your user files? (y/n): ");
            GetUserInput(nukePrompt);
            if (nukePrompt == "y")
            {
                ncoutln("Deleting log file...");
                if (remove(DEFAULT_LOG_FILE) != 0) // delete log
                    ncoutln("Unable to delete log file!");

                ncoutln("Deleting setting database file...");
                if (remove(DEFAULT_SETTINGS_FILE) != 0) // delete settings
                    ncoutln("Unable to delete setting database!");

                ncoutln("Deleting contact database...");
                if (remove(contactDatabasePath.c_str()) != 0) // delete contact database
                    ncoutln("Unable to delete contact database!");
            }
            return 0; // exit
        }
        else if (userInput == "save-settings") // save settings
        {
            quickPrintLog(INFO, "Saving settings file...");
            settingDatabase.Save();
        }
        else if (userInput == "change-setting") // change a setting
        {
            quickPrintLog(INFO, "Changing settings...");
            InteractiveChangeSetting(settingDatabase);
        }
        else if (userInput == "clear") // clear screen
        {
            clear();
        }
        else if (userInput == "save-contacts") // save contacts
        {
            quickPrintLog(INFO, "Saving contact file...");
            contactDatabase.Save();
        }
        else if (userInput == "chat") // start chat server
        {
            // boost::asio::streambuf readBuf;
            ncoutln("What port would you like to start the server on? ");
            GetUserInput(userInput);
            //  Server testServ(stoi(userInput));
            quickPrintLog(INFO, "Starting server on port: " << userInput << "...");
            // testServ.Listen();
            ncoutln("Got conenction!");
            // testServ.Read(readBuf);
        }
        else if (userInput == "exit") // exit scats
        {
            quickPrintLog(INFO, "Exiting scats...");
            return 0;
        }
        else if (userInput == "build") // rebuild scats
        {
            quickPrintLog(INFO, "Exiting, rebuilding, and relaunching scats...");
            system("cd ..; make; cd bin; ./scats");
            return 0;
        }
        else
        {
            if (enableShell != "true")
            {
                ncoutln("Unknown command!");
            }
            else if (system(userInput.c_str()) < 0)
            {
                ncoutln("Error: " << strerror(errno));
            }
        }
        if(commandHistory.size() < DEFAULT_HISTORY_LEN)
            commandHistory.push_back(userInput);
        else
        {
            commandHistory.erase(commandHistory.begin());
            commandHistory.push_back(userInput);
        }
    }
}
