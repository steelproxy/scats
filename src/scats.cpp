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
#include "setting.h"
#include "contact.h"
#include "log.h"
#include "network.h"
#include "interactive.h"
#include <curses.h>

#define IfSet(value, key) if ((value = settingDatabase.SearchKey(key).GetValue()) == string())

#define DEFAULT_CONTACTS_FILE "contacts.txt"
#define DEFAULT_SETTINGS_FILE "settings.txt"
#define DEFAULT_LOG_FILE "log.txt"

using namespace std;

Log logger;
WINDOW* root;
int curX;
int curY;

template <class charT, class Traits=char_traits<charT> >
class MyOstream : public basic_ostream<charT,Traits> {
 public:
   MyOstream(basic_streambuf<charT,Traits>* sb) 
   : basic_ostream<charT,Traits>(sb) {}
   // . . .
};

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

int main(int argc, char **argv)
{
    ContactDB contactDatabase;
    SettingDB settingDatabase;
    string contactDatabasePath;
    string userInput;
    string userHandle;
    string enableShell;
    string lastCommand;

    root = initscr();
    cbreak(); // don't require newline
    keypad(root, true); // enable advanced key sequences
    noecho(); // don't echo commands, we'll control that
    scrollok(root, true); // enable scrolling


    // load logger
    if (logger.Open(DEFAULT_LOG_FILE)) // initialize logger
    {
        ncoutln("error: Unable to open log file!"); // print error if fail
    }
    logger.SetPrint(false); // do not print log to cout
    logger.Truncate();      // truncate log (new log every run)
    quickPrintLog(INFO, "Starting scats...");

    // load settings
    quickPrintLog(INFO, "Loading settings database...");
    try
    {
        settingDatabase.Open(DEFAULT_SETTINGS_FILE);
        settingDatabase.Load();
    }
    catch (const std::exception &e)
    {
        quickPrintLog(ERROR, "Unable to load settings database: " << e.what());
    }


    // load contacts
    quickPrintLog(INFO, "Loading contacts database...");
    IfSet(contactDatabasePath, "contactDatabasePath")
        contactDatabasePath = DEFAULT_CONTACTS_FILE;
    try
    {
        contactDatabase.Open(contactDatabasePath);
        contactDatabase.Load();
    }
    catch (const std::string &e)
    {
        quickPrintLog(ERROR, "Unable to load contacts database: " << e);
    }

    // interactive loop
    for (;;)
    {
        userInput = string();
        // setup user handle
        IfSet(userHandle, "userHandle")
            InteractiveSetUserHandle(userHandle);
        IfSet(enableShell, "enableShell")
            enableShell = "false";

        addstr("[");
        addstr(userHandle.c_str());
        addstr("] > "); // print prompt
        userInput = getch();
        refresh();
        while(userInput.at(userInput.length() - 1) != '\n')
        {
            if(isprint(userInput.at(userInput.length() - 1)))
                addch(userInput.at(userInput.length() - 1));
            refresh();
            userInput += getch();
        }   

        userInput.erase(userInput.end() - 1);
        addch('\n');

        if (userInput == "help")
        {
            DisplayHelp();
        }
        else if (userInput == "add-contact")
        {
            quickPrintLog(INFO, "Adding new contact...");
            InteractiveAddContact(contactDatabase);
        }
        else if (userInput == "delete-contact")
        {
            quickPrintLog(INFO, "Deleting contact...");
            InteractiveDeleteContact(contactDatabase);
        }
        else if (userInput == "list-contacts")
        {
            cout << "Contacts: " << endl;
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
        else if (userInput == "server")
        {
            ncoutln("What port would you like to start the server on? ");
            ngetstr(userInput);
            Server testServ(stoi(userInput));
            quickPrintLog(INFO, "Starting server on port: " << userInput << "...");
            testServ.Listen();
            ncoutln("Got conenction!");
        }
        else if (userInput == "exit")
        {
            quickPrintLog(INFO, "Exiting scats...");
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
                continue;
            if (system(userInput.c_str()) < 0)
            {
                cout << "Error: " << strerror(errno) << endl;
            }
        }
    }
}
