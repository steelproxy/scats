#include <iostream>
#include <string>
#include <vector>
#include "contact.h"
#include "log.h"
#include "setting.h"

using namespace std;

extern Log logger;

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

void InteractiveSetUserHandle(string &userHandle)
{
    cout << "Handle (max 16 characters, no special characters): ";
    getline(cin, userHandle);                                                              // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        cout << "Handle is invalid. Please enter a new one: ";
        getline(cin, userHandle); // read a new user handle into userHandle
    }
}

void InteractiveAddContact(ContactDB &contactDatabase)
{
    string newAlias;
    string newEndpoint;
    string newPort;

    quickPrintLog(INFO, "Adding new contact.");
    cout << "Alias: ";
    getline(cin, newAlias);
    cout << "Hostname: ";
    getline(cin, newEndpoint);

    for (size_t index = 0; index < newAlias.length(); index++)
    {
        if (newAlias.at(index) == ',' || !isprint(newAlias.at(index)))
            newAlias.at(index) = ' '; // sanitize ','
    }

    for (size_t index = 0; index < newEndpoint.length(); index++)
    {
        if (newEndpoint.at(index) == ',' || !isprint(newEndpoint.at(index)))
            newEndpoint.at(index) = ' '; // sanitize ','
    }

    cout << "Port: ";
    while (true)
    {
        getline(cin, newPort);
        try
        {
            contactDatabase.AddContact(Contact(newAlias, newEndpoint, stoi(newPort)));
            break;
        }
        catch (const std::exception &e)
        {
            cout << "Unable to add contact: " << e.what() << endl;
        }
    }
}

void InteractiveDeleteContact(ContactDB &contactDatabase)
{
    string tarGetAlias;
    size_t startingLen = contactDatabase.GetLength();

    quickPrintLog(INFO, "Deleting contact.");
    cout << "Alias (must be exact): ";
    getline(cin, tarGetAlias);

    contactDatabase.DeleteContact(contactDatabase.SearchAlias(tarGetAlias));
    if (contactDatabase.GetLength() < startingLen)
        cout << "Contact deleted." << endl;
}

void InteractiveAddSetting(SettingDB &settingDatabase)
{
    string newKey;
    string newValue;
    string newDescription;

    cout << "Adding new setting..." << endl;
    cout << "Key: ";
    getline(cin, newKey);
    for (size_t index = 0; index < newKey.length(); index++)
    {
        if (newKey.at(index) == '=' || newKey.at(index) == ':' || !isprint(newKey.at(index)))
            newKey.at(index) = ' ';
    }

    cout << "Value: ";
    getline(cin, newValue);
    for (size_t index = 0; index < newValue.length(); index++)
    {
        if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
            newValue.at(index) = ' ';
    }

    cout << "Description: ";
    getline(cin, newDescription);
    for (size_t index = 0; index < newDescription.length(); index++)
    {
        if (newDescription.at(index) == '=' || newDescription.at(index) == ':' || !isprint(newDescription.at(index)))
            newDescription.at(index) = ' ';
    }

    settingDatabase.AddSetting(Setting(newKey, newValue, newDescription));
}

void InteractiveDeleteSetting(SettingDB &settingDatabase)
{
    string targetKey;
    size_t startingLen = settingDatabase.GetLength();

    quickPrintLog(INFO, "Deleting setting...");
    cout << "Key: ";
    getline(cin, targetKey);

    settingDatabase.DeleteSetting(settingDatabase.SearchKey(targetKey));
    if (settingDatabase.GetLength() < startingLen)
        cout << "Setting deleted." << endl;
}

void InteractiveChangeSetting(SettingDB &settingDatabase)
{
    Setting targetSetting;
    string targetKey;
    string newValue;
    string oldDescription;

    quickPrintLog(INFO, "Changing setting...");
    cout << "Key: ";
    getline(cin, targetKey);

    targetSetting = settingDatabase.SearchKey(targetKey);

    if (!targetSetting.Empty())
    {
        cout << targetSetting.GetKey() << "=" << targetSetting.GetValue() << endl;
        cout << targetSetting.GetKey() << "=";

        getline(cin, newValue);
        for (size_t index = 0; index < newValue.length(); index++)
        {
            if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
                newValue.at(index) = ' ';
        }
        oldDescription = targetSetting.GetDescription();
        settingDatabase.DeleteSetting(targetSetting);
        settingDatabase.AddSetting(Setting(targetKey, newValue, oldDescription));
    }
}