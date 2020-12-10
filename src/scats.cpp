#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
#include "setting.h"
#include "contact.h"
#include "log.h"
#include "network.h"
#include "interactive.h"

#define IfSet(value, key) if ((value = settingDatabase.SearchKey(key).GetValue()) == string())

#define DEFAULT_CONTACTS_FILE "contacts.txt"
#define DEFAULT_SETTINGS_FILE "settings.txt"
#define DEFAULT_LOG_FILE "log.txt"

using namespace std;

Log logger;

void DisplayHelp()
{
    cout << "Commands: " << endl;
    cout << "help       Displays help page." << endl;
    cout << "add        Adds a new contact." << endl;
    cout << "list       Lists all contacts in contacts list." << endl;
    cout << "connect    Connects to a contact." << endl;
}

int main(int argc, char **argv)
{
    ContactDB contactDatabase;
    SettingDB settingDatabase;
    string userInput;
    string userHandle;
    string enableShell;

    // load logger
    if (logger.Open(DEFAULT_LOG_FILE)) // initialize logger
    {
        cout << "error: Unable to open log file!" << endl; // print error if fail
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
    try
    {
        contactDatabase.Open(DEFAULT_CONTACTS_FILE);
        contactDatabase.Load();
    }
    catch (const std::exception &e)
    {
        quickPrintLog(ERROR, "Unable to load contacts database: " << e.what());
    }

    // interactive loop
    for (;;)
    {
        // setup user handle
        IfSet(userHandle, "userHandle")
            InteractiveSetUserHandle(userHandle);
        IfSet(enableShell, "enableShell")
            enableShell = "false";

        cout << "[" << userHandle << "] > "; // print prompt
        getline(cin, userInput);             // get user input

        if (userInput == "help")
        {
            DisplayHelp();
        }
        else if (userInput == "add-contact")
        {
            InteractiveAddContact(contactDatabase);
        }
        else if (userInput == "delete-contact")
        {
            InteractiveDeleteContact(contactDatabase);
        }
        else if (userInput == "list-contacts")
        {
            cout << "Contacts: " << endl;
            for (size_t index = 0; index < contactDatabase.GetLength(); index++)
            {
                cout << "[" << contactDatabase.GetIndex(index).GetAlias() << "]" << endl;
                cout << "   " << contactDatabase.GetIndex(index).GetEndpoint() << endl;
                cout << "   " << contactDatabase.GetIndex(index).GetPort() << endl;
            }
        }
        else if (userInput == "add-setting")
        {
            InteractiveAddSetting(settingDatabase);
        }
        else if (userInput == "delete-setting")
        {
            InteractiveDeleteSetting(settingDatabase);
        }
        else if (userInput == "list-settings")
        {
            cout << "Settings: " << endl;
            for (size_t index = 0; index < settingDatabase.GetLength(); index++)
            {
                cout << settingDatabase.GetIndex(index).GetKey() << "=" << settingDatabase.GetIndex(index).GetValue() << endl;
                cout << "    " << settingDatabase.GetIndex(index).GetDescription() << endl;
            }
        }
        else if (userInput == "save-settings")
        {
            quickPrintLog(INFO, "Saving settings file.");
            settingDatabase.Save();
        }
        else if (userInput == "change-setting")
        {
            InteractiveChangeSetting(settingDatabase);
        }
        else if (userInput == "save-contacts")
        {
            quickPrintLog(INFO, "Saving contact file.");
            contactDatabase.Save();
        }
        else if (userInput == "server")
        {
            cout << "What port would you like to start the server on? " << endl;
            getline(cin, userInput);
            Server testServ(stoi(userInput));
            quickPrintLog(INFO, "Starting server on port: " << userInput << "...");
            testServ.Listen();
            cout << "Got conenction!" << endl;
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
