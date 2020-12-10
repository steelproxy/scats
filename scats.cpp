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

#define quickLog(level, message)                             \
    {                                                        \
        logger.stringBuilder.str(string());                  \
        logger.stringBuilder << message;                     \
        logger.WriteLine(level, logger.stringBuilder.str()); \
    }

#define quickPrintLog(level, message) \
    {                                 \
        cout << message << endl;      \
        quickLog(level, message)      \
    }

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

void loadContacts(fstream &contactsFile, vector<Contact> &contactsList)
{
    string contactLine;

    quickLog(INFO, "Loading contacts...");
    while (getline(contactsFile, contactLine))
    {
        if (contactLine != string())
            contactsList.push_back(Contact(contactLine));
    }
    contactsFile.close();
}

void loadSettings(fstream &settingsFile, vector<Setting> &settingsList)
{
    string settingLine;

    quickLog(INFO, "Loading settings...");
    while (getline(settingsFile, settingLine))
    {
        if (settingLine != string())
            settingsList.push_back(Setting(settingLine));
    }
    settingsFile.close();
}

void SetSetting(vector<Setting> &settingsList, string targetKey, string newValue)
{
    for (size_t index = 0; index < settingsList.size(); index++)
    {
        if (settingsList.at(index).GetKey() == targetKey)
        {
            settingsList.at(index).SetValue(newValue);
        }
    }
}

string GetSetting(vector<Setting> &settingsList, string targetKey)
{
    for (size_t index = 0; index < settingsList.size(); index++)
    {
        if (settingsList.at(index).GetKey() == targetKey)
        {
            return settingsList.at(index).GetValue();
        }
    }
    return string();
}

bool isPrintStr(string str)
{
    for (size_t index = 0; index < str.length(); index++)
    {
        if (!isprint(str.at(index)))
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    Server testServ;
    fstream settingDBFile;
    fstream contactDBFile;
    vector<Setting> settingDB;
    vector<Contact> contactDB;
    string userInput;
    string userHandle;
    string aliasBuf;
    string hostnameBuf;
    string portBuf;
    string keyBuf;
    string valueBuf;
    string descriptionBuf;

    // load logger
    if (logger.Open(DEFAULT_LOG_FILE)) // initialize logger
    {
        cout << "error: Unable to open log file!" << endl; // print error if fail
    }
    logger.SetPrint(false); // do not print log to cout
    logger.Truncate();      // truncate log (new log every run)
    quickPrintLog(INFO, "Starting scats...");

    // load settings
    settingDBFile.open(DEFAULT_SETTINGS_FILE, ios::in); // open settings database file for reading
    if (settingDBFile.fail())                           // if it fails
    {
        quickPrintLog(SEVERE, "Couldn't open settings file: " << DEFAULT_SETTINGS_FILE);
        return 1; // return error
    }
    quickPrintLog(INFO, "Loading settings...");
    loadSettings(settingDBFile, settingDB); // load settings from database file into database

    // load contacts
    contactDBFile.open(DEFAULT_CONTACTS_FILE, ios::in); // open contacts database file for reading
    if (contactDBFile.fail())                           // if it fails
    {
        quickPrintLog(SEVERE, "Couldn't open contacts file: " << DEFAULT_CONTACTS_FILE);
        return 1; // return error
    }
    quickPrintLog(INFO, "Loading contacts list...");
    loadContacts(contactDBFile, contactDB); // load contacts from database file into database

    // setup user handle
    cout << "Handle (max 16 characters, no special characters): ";
    getline(cin, userHandle);                                                              // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        cout << "Handle is invalid. Please enter a new one: ";
        getline(cin, userHandle); // read a new user handle into userHandle
    }

    // interactive loop
    for (;;)
    {
        cout << "[" << userHandle << "] > "; // print prompt
        getline(cin, userInput);             // get user input

        if (userInput == "help")
        {
            DisplayHelp();
        }
        else if (userInput == "add-contact")
        {
            quickPrintLog(INFO, "Adding new contact.");
            cout << "Alias: ";
            getline(cin, aliasBuf);
            cout << "Hostname: ";
            getline(cin, hostnameBuf);
            cout << "Port: ";

            while (true)
            {
                getline(cin, portBuf);
                try
                {
                    contactDB.push_back(Contact(aliasBuf, hostnameBuf, stoi(portBuf)));
                    break;
                }
                catch (const std::exception &e)
                {
                    cout << "Invalid port. Please enter a new one: " << endl;
                }
            }
        }
        else if (userInput == "delete-contact")
        {
            quickPrintLog(INFO, "Deleting contact.");
            cout << "Alias (must be exact): ";
            getline(cin, userInput);

            for (size_t index = 0; index < contactDB.size(); index++)
            {
                if (contactDB.at(index).getAlias() == userInput)
                {
                    contactDB.erase(contactDB.begin() + index);
                    quickPrintLog(INFO, "Contact deleted.");
                    break;
                }
            }
        }
        else if (userInput == "list-contacts")
        {
            cout << "Contacts: " << endl;
            for (size_t index = 0; index < contactDB.size(); index++)
            {
                cout << "[" << contactDB.at(index).getAlias() << "]" << endl;
                cout << "   " << contactDB.at(index).getHostname() << endl;
                cout << "   " << contactDB.at(index).getPort() << endl;
            }
        }
        else if (userInput == "add-setting")
        {
            cout << "Adding new setting." << endl;
            cout << "Key: ";
            settingDB.push_back(Setting());
            getline(cin, keyBuf);
            settingDB.at(settingDB.size() - 1).SetKey(keyBuf);

            cout << "Value: ";
            getline(cin, valueBuf);
            settingDB.at(settingDB.size() - 1).SetValue(valueBuf);

            cout << "Description: ";
            getline(cin, descriptionBuf);
            settingDB.at(settingDB.size() - 1).SetDescription(descriptionBuf);

            cout << "Created setting (" << settingDB.at(settingDB.size() - 1).GetKey() << "=" << settingDB.at(settingDB.size() - 1).GetValue() << ":" << settingDB.at(settingDB.size() - 1).GetDescription() << ")" << endl;
        }
        else if (userInput == "delete-setting")
        {
            cout << "Deleting setting." << endl;
            cout << "Key: ";
            getline(cin, userInput);
            for (size_t index = 0; index < settingDB.size(); index++)
            {
                if (settingDB.at(index).GetKey() == userInput)
                {
                    settingDB.erase(settingDB.begin() + index);
                    cout << "Erased setting." << endl;
                    break;
                }
            }
        }
        else if (userInput == "list-settings")
        {
            cout << "Settings: " << endl;
            for (size_t index = 0; index < settingDB.size(); index++)
            {
                cout << settingDB.at(index).GetKey() << "=" << settingDB.at(index).GetValue() << endl;
                cout << "    " << settingDB.at(index).GetDescription() << endl;
            }
        }
        else if (userInput == "save-settings")
        {
            settingDBFile.close();
            settingDBFile.open(DEFAULT_SETTINGS_FILE, ios::trunc | ios::out);
            if (settingDBFile.fail())
            {
                quickPrintLog(ERROR, "Unable to open settings file!");
                continue;
            }

            for (size_t index = 0; index < settingDB.size(); index++)
            {
                settingDBFile << settingDB.at(index).ToString() << endl;
            }
            settingDBFile.close();
            settingDBFile.open(DEFAULT_SETTINGS_FILE, ios::in);
            if (settingDBFile.fail())
            {
                quickPrintLog(ERROR, "Unable to open settings file!");
                continue;
            }
        }
        else if (userInput == "save-contacts")
        {
            quickPrintLog(INFO, "Saving contact file.");
            contactDBFile.close();
            contactDBFile.open(DEFAULT_CONTACTS_FILE, ios::trunc | ios::out);
            for (size_t index = 0; index < contactDB.size(); index++)
            {
                contactDBFile << contactDB.at(index).toString() << endl;
            }
            contactDBFile.close();
            contactDBFile.open(DEFAULT_CONTACTS_FILE, ios::in);
        }
        else if (userInput == "server")
        {
            cout << "What port would you like to start the server on? " << endl;
            getline(cin, userInput);
            testServ.Free();
            testServ = Server(stoi(userInput));
            cout << "Starting server on port: " << userInput << "..." << endl;
            for(;;)
            {
                testServ.Listen();
                cout << "Got conenction!" << endl;
            }
            quickPrintLog(INFO, "Starting server...");
        }
        else if (userInput == "exit")
        {
            quickPrintLog(INFO, "Exiting scats...");
            contactDBFile.close();
            logger.Close();
            return 0;
        }
    }
}
