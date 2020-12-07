#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
#include "contact.h"
#include "log.h"

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
#define DEFAULT_LOG_FILE "log.txt"

using namespace std;

Log logger;

void DisplayUsage(string path)
{
    cout << "Usage: " << path << " [port]" << endl;
    cout << "Options: " << endl;
    cout << "-c     Specify contacts file." << endl;
    cout << "-u     Set username/handle." << endl;
    cout << "-v     Enable verbose mode." << endl;
}

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
    time_t timeInfo = time(0);
    string contactLine;

    while (getline(contactsFile, contactLine))
    {
        if (contactLine != string())
            contactsList.push_back(Contact(contactLine));
    }
    contactsFile.close();
    quickLog(INFO, "Finished loading contacts in " << timeInfo - time(0) << " seconds.");
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
    fstream contactDBFile;
    vector<Contact> contactDB;
    string userInput;
    string userHandle;
    string aliasBuf;
    string hostnameBuf;
    string portBuf;

    if (logger.Open(DEFAULT_LOG_FILE))
    {
        cout << "error: Unable to open log file!" << endl;
    }

    if (logger.Open(DEFAULT_LOG_FILE) != 0)
    {
        cout << "error: Unable to open log file!" << endl;
    }

    logger.SetPrint(false);
    logger.Truncate();

    quickPrintLog(INFO, "Starting scats...");

    contactDBFile.open(DEFAULT_CONTACTS_FILE, ios::in);
    if (contactDBFile.fail())
    {
        quickPrintLog(SEVERE, "Couldn't open contacts file: " << DEFAULT_CONTACTS_FILE);
        return 1;
    }

    quickPrintLog(INFO, "Loading contacts list...");
    loadContacts(contactDBFile, contactDB);

    cout << "Handle (max 16 characters, no special characters): ";
    getline(cin, userHandle);
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle))
    {
        cout << "Handle is invalid. Please enter a new one: ";
        getline(cin, userHandle);
    }

    for (;;)
    {
        cout << "[" << userHandle << "] > ";
        getline(cin, userInput);
        if (userInput == "help")
        {
            DisplayHelp();
        }
        else if (userInput == "list")
        {
            cout << "Contacts: " << endl;
            for (size_t index = 0; index < contactDB.size(); index++)
            {
                cout << "[" << contactDB.at(index).getAlias() << "]" << endl;
                cout << "   " << contactDB.at(index).getHostname() << endl;
                cout << "   " << contactDB.at(index).getPort() << endl;
            }
        }
        else if (userInput == "add")
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
        else if (userInput == "delete")
        {
            quickPrintLog(INFO, "Deleting contact.");
            cout << "Alias (must be exact): ";
            getline(cin, userInput);
            
            for(size_t index = 0; index < contactDB.size(); index++)
            {
                if(contactDB.at(index).getAlias() == userInput)
                {
                    contactDB.erase(contactDB.begin() + index);
                    quickPrintLog(INFO, "Contact deleted.");
                    break;
                }
            }
        }
        else if(userInput == "save")
        {
            quickPrintLog(INFO, "Saving contact file.");
            contactDBFile.close();
            contactDBFile.open(DEFAULT_CONTACTS_FILE, ios::trunc | ios::out);
            for(size_t index = 0; index < contactDB.size(); index++)
            {
                contactDBFile << contactDB.at(index).toString() << endl;
            }
            contactDBFile.close();
            contactDBFile.open(DEFAULT_CONTACTS_FILE, ios::in);
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
