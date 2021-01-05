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

#define DEFAULT_CONTACTS_FILE "contacts.txt"
#define DEFAULT_SETTINGS_FILE "settings.txt"
#define DEFAULT_ENABLE_SHELL "false"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_FILE "log.txt"

using namespace std;

extern void dngetstr(string &var);

Log logger;
WINDOW *root;
int curX;
int curY;
sigjmp_buf begin_loop;

void SignalHandler(int sig)
{
    if(sig == SIGINT)
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
    ncoutln("delete-contact Deletes a contact.");
    ncoutln("delete-setting Deletes a setting.");
    ncoutln("exit           Exit scats.");
    ncoutln("help           Displays help page.");
    ncoutln("list-contacts  Lists all contacts.");
    ncoutln("list-settings  Lists all settings.");
    ncoutln("server         Starts server.");
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
}

int main(int argc, char **argv)
{
    atexit(Cleanup);

    root = initscr();
    cbreak();             // don't require newline
    keypad(root, true);   // enable advanced key sequences
    noecho();             // don't echo commands, we'll control that
    scrollok(root, true); // enable scrolling

    // load logger
    if(!FileExists(DEFAULT_LOG_FILE))
    {
        ofstream logFile;
        quickPrintLog(WARNING, "Log file does not exist!");
        quickPrintLog(INFO, "Creating log file...");
        logFile.open(DEFAULT_LOG_FILE, ios::out);
        if(logFile.fail())
            quickPrintLog(ERROR, "Unable to create log file!");
        logFile.close();
    }
    if (logger.Open(DEFAULT_LOG_FILE)) // initialize logger
    {
        ncoutln("error: Unable to open log file!"); // print error if fail
    }
    logger.Truncate(); // truncate log (new log every run)
    quickPrintLog(INFO, "Starting scats...");

    // load settings
    SettingDB settingDatabase;
    string contactDatabasePath;
    string enableShell;
    string logLevel;
    string userHandle;
    quickPrintLog(INFO, "Loading setting database...");
    if(!FileExists(DEFAULT_SETTINGS_FILE))
    {
        ofstream settingFile;
        quickPrintLog(WARNING, "Setting database does not exist!");
        quickPrintLog(INFO, "Creating setting database...");
        settingFile.open(DEFAULT_SETTINGS_FILE, ios::out);
        if(settingFile.fail())
            quickPrintLog(ERROR, "Unable to create setting database!");
        settingFile.close();
    }
    if (settingDatabase.Open(DEFAULT_SETTINGS_FILE) ||
        settingDatabase.Load())
        quickPrintLog(ERROR, "Unable to load setting database!");

    // apply settings
    IfSet(contactDatabasePath, "contactDatabasePath")
        contactDatabasePath = DEFAULT_CONTACTS_FILE;
    IfSet(enableShell, "enableShell")
        enableShell = DEFAULT_ENABLE_SHELL;
    IfSet(logLevel, "logLevel")
        logLevel = DEFAULT_LOG_LEVEL;
    IfSet(userHandle, "userHandle")
        InteractiveSetUserHandle(settingDatabase, userHandle);

    // load contacts
    ContactDB contactDatabase;
    if(!FileExists(contactDatabasePath))
    {
        ofstream contactFile;
        quickPrintLog(WARNING, "Contact database does not exist!");
        quickPrintLog(INFO, "Creating contact database.");
        contactFile.open(contactDatabasePath, ios::out);
        if(contactFile.fail())
            quickPrintLog(ERROR, "Unable to create contact database!");
        contactFile.close();
    }
    quickPrintLog(INFO, "Loading contact database...");
    if (contactDatabase.Open(contactDatabasePath) ||
        contactDatabase.Load())
        quickPrintLog(ERROR, "Unable to load contact database!");

    signal(SIGINT, SignalHandler);

    // interactive loop
    for (;;)
    {
        string userInput;

        while ( sigsetjmp(begin_loop, 1 ) != 0 ); // set jump to beginning of loop

        userInput = string();

        logger.SetLevel(LevelToI(logLevel));

        addstr("[");
        addstr(userHandle.c_str());
        addstr("] > "); // print prompt
        ngetstr(userInput);

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
            for (size_t index = 0; index < contactDatabase.GetLength(); index++)
            {
                ncoutln("[" << contactDatabase.GetIndex(index).GetAlias() << "]");
                ncoutln("   " << contactDatabase.GetIndex(index).GetEndpoint());
                ncoutln("   " << contactDatabase.GetIndex(index).GetPort());
            }
        }
        else if (userInput == "add-setting")
        {
            quickPrintLog(INFO, "Adding new setting...");
            InteractiveAddSetting(settingDatabase);
        }
        else if (userInput == "delete-setting")
        {
            quickPrintLog(INFO, "Deleting setting...");
            InteractiveDeleteSetting(settingDatabase);
        }
        else if (userInput == "list-settings")
        {
            ncoutln("Settings: ");
            for (size_t index = 0; index < settingDatabase.GetLength(); index++)
            {
                ncoutln(settingDatabase.GetIndex(index).GetKey() << "=" << settingDatabase.GetIndex(index).GetValue());
                ncoutln("    " << settingDatabase.GetIndex(index).GetDescription());
            }
        }
        else if (userInput == "nuke")
        {
            string nukePrompt;
            ncout("Would you like to delete your user files? (y/n): ");
            ngetstr(nukePrompt);
            if(nukePrompt == "y")
            {
                ncoutln("Deleting log file...");
                if(remove(DEFAULT_LOG_FILE) != 0)
                    ncoutln("Unable to delete log file!");

                ncoutln("Deleting setting database file...");
                if(remove(DEFAULT_SETTINGS_FILE) != 0)
                    ncoutln("Unable to delete setting database!");

                ncoutln("Deleting contact database...");
                if(remove(contactDatabasePath.c_str()) != 0)
                    ncoutln("Unable to delete contact database!");
            }
        }
        else if (userInput == "save-settings")
        {
            quickPrintLog(INFO, "Saving settings file...");
            settingDatabase.Save();
        }
        else if (userInput == "change-setting")
        {
            quickPrintLog(INFO, "Changing settings...");
            InteractiveChangeSetting(settingDatabase);
        }
        else if (userInput == "save-contacts")
        {
            quickPrintLog(INFO, "Saving contact file...");
            contactDatabase.Save();
        }
        else if (userInput == "chat")
        {
            // boost::asio::streambuf readBuf;
            ncoutln("What port would you like to start the server on? ");
            ngetstr(userInput);
            //  Server testServ(stoi(userInput));
            quickPrintLog(INFO, "Starting server on port: " << userInput << "...");
           // testServ.Listen();
            ncoutln("Got conenction!");
            // testServ.Read(readBuf);
        }
        else if (userInput == "exit")
        {
            quickPrintLog(INFO, "Exiting scats...");
            endwin();
            logger.Close();
            return 0;
        }
        else if (userInput == "build")
        {
            quickPrintLog(INFO, "Exiting, rebuilding, and relaunching scats...");
            system("cd ..; make; cd bin; ./scats");
            logger.Close();
            return 1;
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
        lastCommand.push_back(userInput);
    }
}
