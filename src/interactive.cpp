/** 
 *  @file   interactive.cpp
 *  @brief  Interactive functions for command line.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

// TODO Implement exception handling.

#include <iostream>
#include <string>
#include <vector>
#include <curses.h>
#include "contact.h"
#include "log.h"
#include "setting.h"
#include "cursesmode.h"

using namespace std;

bool isPrintStr(string str)
{
    for (size_t index = 0; index < str.length(); index++)
    {
        if (!checkPrintable(str.at(index)))
        {
            return false;
        }
    }
    return true;
}

void InteractiveSetUserHandle(SettingDB &database)
{
    string userHandle;
    ncout("Handle (max 16 characters, no special characters): ");
    GetConsoleInput(root, false, userHandle);                                                              // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        ncout("Handle is invalid. Please enter a new one: ");
        GetConsoleInput(root, false, userHandle); // read a new user handle into userHandle
    }
    try
    {
        database.addSetting(Setting("userHandle", userHandle, "Name used to identify yourself to others."));
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to set user handle!");
        exceptionLog(ERROR, msg);
    }
}

void InteractiveAddContact(ContactDB &contactDatabase)
{
    string newAlias;
    do
    {
        ncout("Alias: ");
        GetConsoleInput(root, false, newAlias);
    } while (newAlias.empty());

    string newEndpoint;
    do
    {
        ncout("Endpoint: ");
        GetConsoleInput(root, false, newEndpoint);
    } while (newEndpoint.empty());

    for (size_t index = 0; index < newAlias.length(); index++)
    {
        if (newAlias.at(index) == ',' || !isprint(newAlias.at(index)))
        {
            newAlias.at(index) = ' '; // sanitize ','
        }
    }

    for (size_t index = 0; index < newEndpoint.length(); index++)
    {
        if (newEndpoint.at(index) == ',' || !isprint(newEndpoint.at(index)))
        {
            newEndpoint.at(index) = ' '; // sanitize ','
        }
    }

    string newPort;
    ncout("Port: ");
    while (true)
    {
        GetConsoleInput(root, false, newPort);
        try
        {
            contactDatabase.addContact(Contact(newAlias, newEndpoint, stoi(newPort)));
            break;
        }
        catch (const char *msg)
        {
            quickPrintLog(ERROR, "Unable to add contact!");
            exceptionLog(ERROR, msg);
            return;
        }
    }
    quickPrintLog(INFO, "Added contact.");
}

void InteractiveDeleteContact(ContactDB &contactDatabase)
{
    string targetAlias;
    size_t startingLen = contactDatabase.getLength();

    do
    {
        ncout("Alias (must be exact): ");
        GetConsoleInput(root, false, targetAlias);
    } while (targetAlias.empty());

    try
    {
        contactDatabase.deleteContact(contactDatabase.searchAlias(targetAlias));
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to delete contact!");
        exceptionLog(ERROR, msg);
        return;
    }

    if (contactDatabase.getLength() < startingLen) // can probably remove this
    {
        quickPrintLog(INFO, "Contact deleted.");
    }
}

void InteractiveAddSetting(SettingDB &settingDatabase)
{
    string newKey;
    ncout("Key: ");
    GetConsoleInput(root, false, newKey);
    for (size_t index = 0; index < newKey.length(); index++)
    {
        if (newKey.at(index) == '=' || newKey.at(index) == ':' || !isprint(newKey.at(index)))
        {
            newKey.at(index) = ' ';
        }
    }

    string newValue;
    ncout("Value: ");
    GetConsoleInput(root, false, newValue);
    for (size_t index = 0; index < newValue.length(); index++)
    {
        if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
        {
            newValue.at(index) = ' ';
        }
    }

    string newDescription;
    ncout("Description: ");
    GetConsoleInput(root, false, newDescription);
    for (size_t index = 0; index < newDescription.length(); index++)
    {
        if (newDescription.at(index) == '=' || newDescription.at(index) == ':' || !isprint(newDescription.at(index)))
        {
            newDescription.at(index) = ' ';
        }
    }

    try
    {
        settingDatabase.addSetting(Setting(newKey, newValue, newDescription));
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to add setting!");
        exceptionLog(ERROR, msg);
        return;
    }

    quickPrintLog(INFO, "New setting added " << newKey << "=" << newValue << ":" << newDescription);
}

void InteractiveDeleteSetting(SettingDB &settingDatabase)
{
    string targetKey;
    size_t startingLen = settingDatabase.getLength();

    ncout("Key: ");
    GetConsoleInput(root, false, targetKey);

    try
    {
        settingDatabase.deleteSetting(settingDatabase.searchKey(targetKey));
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to delete setting!");
        exceptionLog(ERROR, msg);
        return;
    }

    if (settingDatabase.getLength() < startingLen)
    {
        quickPrintLog(INFO, "Setting deleted.");
    }
}

void InteractiveChangeSetting(SettingDB &settingDatabase)
{
    Setting targetSetting;
    string targetKey;
    string newValue;
    string oldDescription;

    ncout("Key: ");
    GetConsoleInput(root, false, targetKey);
    targetSetting = settingDatabase.searchKey(targetKey);

    if (!targetSetting.empty())
    {
        ncoutln(targetSetting.getKey() << "=" << targetSetting.getValue());
        ncout(targetSetting.getKey() << "=");

        GetConsoleInput(root, false, newValue);
        for (size_t index = 0; index < newValue.length(); index++)
        {
            if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
                newValue.at(index) = ' ';
        }
        oldDescription = targetSetting.getDescription();

        try
        {
            settingDatabase.deleteSetting(targetSetting);
            settingDatabase.addSetting(Setting(targetKey, newValue, oldDescription));
        }
        catch (const char *msg)
        {
            quickPrintLog(ERROR, "Unable to change setting!");
            exceptionLog(ERROR, msg);
            return;
        }

        quickPrintLog(INFO, "Changed setting: " << targetKey << "=" << newValue);
    }
    else
    {
        ncoutln("No setting found!");
    }
}