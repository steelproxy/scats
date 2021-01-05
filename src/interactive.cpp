/** 
 *  @file   interactive.cpp
 *  @brief  Interactive functions for command line.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <iostream>
#include <string>
#include <vector>
#include <curses.h>
#include "contact.h"
#include "log.h"
#include "setting.h"
#include "cursesmode.h"

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

void InteractiveSetUserHandle(SettingDB &database, string &userHandle)
{
    ncout("Handle (max 16 characters, no special characters): ");
    GetUserInput(userHandle);                                                                   // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        ncout("Handle is invalid. Please enter a new one: ");
        GetUserInput(userHandle); // read a new user handle into userHandle
    }
    database.AddSetting(Setting("userHandle", userHandle, "Name used to identify yourself to others."));
}

void InteractiveAddContact(ContactDB &contactDatabase)
{
    string newAlias;
    string newEndpoint;
    string newPort;

    do
    {
        ncout("Alias: ");
        GetUserInput(newAlias);
    } while (newAlias.empty());

    do
    {
        ncout("Endpoint: ");
        GetUserInput(newEndpoint);
    } while (newEndpoint.empty());

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

    ncout("Port: ");
    while (true)
    {
        GetUserInput(newPort);
        try
        {
            contactDatabase.AddContact(Contact(newAlias, newEndpoint, stoi(newPort)));
            break;
        }
        catch (const std::exception &e)
        {
            quickPrintLog(ERROR, "Unable to add contact: " << e.what());
            return;
        }
    }
    quickPrintLog(INFO, "Added contact.");
}

void InteractiveDeleteContact(ContactDB &contactDatabase)
{
    string targetAlias;
    size_t startingLen = contactDatabase.GetLength();

    do
    {
        ncout("Alias (must be exact): ");
        GetUserInput(targetAlias);
    } while (targetAlias.empty());

    try
    {
        contactDatabase.DeleteContact(contactDatabase.SearchAlias(targetAlias));
    }
    catch (const char *e)
    {
        quickPrintLog(ERROR, e);
        return;
    }

    if (contactDatabase.GetLength() < startingLen)
        quickPrintLog(INFO, "Contact deleted.");
}

void InteractiveAddSetting(SettingDB &settingDatabase)
{
    string newKey;
    string newValue;
    string newDescription;

    ncout("Key: ");
    GetUserInput(newKey);
    for (size_t index = 0; index < newKey.length(); index++)
    {
        if (newKey.at(index) == '=' || newKey.at(index) == ':' || !isprint(newKey.at(index)))
            newKey.at(index) = ' ';
    }

    ncout("Value: ");
    GetUserInput(newValue);
    for (size_t index = 0; index < newValue.length(); index++)
    {
        if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
            newValue.at(index) = ' ';
    }

    ncout("Description: ");
    GetUserInput(newDescription);
    for (size_t index = 0; index < newDescription.length(); index++)
    {
        if (newDescription.at(index) == '=' || newDescription.at(index) == ':' || !isprint(newDescription.at(index)))
            newDescription.at(index) = ' ';
    }

    settingDatabase.AddSetting(Setting(newKey, newValue, newDescription));
    quickPrintLog(INFO, "New setting added " << newKey << "=" << newValue << ":" << newDescription);
}

void InteractiveDeleteSetting(SettingDB &settingDatabase)
{
    string targetKey;
    size_t startingLen = settingDatabase.GetLength();

    ncout("Key: ");
    GetUserInput(targetKey);

    settingDatabase.DeleteSetting(settingDatabase.SearchKey(targetKey));
    if (settingDatabase.GetLength() < startingLen)
        quickPrintLog(INFO, "Setting deleted.");
}

void InteractiveChangeSetting(SettingDB &settingDatabase)
{
    Setting targetSetting;
    string targetKey;
    string newValue;
    string oldDescription;

    ncout("Key: ");
    GetUserInput(targetKey);
    targetSetting = settingDatabase.SearchKey(targetKey);

    if (!targetSetting.Empty())
    {
        ncoutln(targetSetting.GetKey() << "=" << targetSetting.GetValue());
        ncout(targetSetting.GetKey() << "=");

        GetUserInput(newValue);
        for (size_t index = 0; index < newValue.length(); index++)
        {
            if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
                newValue.at(index) = ' ';
        }
        oldDescription = targetSetting.GetDescription();
        settingDatabase.DeleteSetting(targetSetting);
        settingDatabase.AddSetting(Setting(targetKey, newValue, oldDescription));
        quickPrintLog(INFO, "Changed setting: " << targetKey << "=" << newValue);
    }
}