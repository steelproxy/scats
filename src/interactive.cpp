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
#include "chatlog.h"
#include "commandline.h"

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
    quickPrintLog(INFO, "Setting new user handle...");

    string userHandle;
    ncOutUsr("Please enter a new user handle.");
    ncOutCmd("Handle (max 16 characters, no special characters): ");
    userHandle = GetConsoleInput(false);
    // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        ncOutUsr("Invalid handle, please enter a new one.");
        ncOutCmd("Handle: ");
        userHandle = GetConsoleInput(false); // read a new user handle into userHandle
    }
    try
    {
        database.addSetting(Setting("userHandle", userHandle, "unknown", "Name used to identify yourself to others."));
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to set user handle!");
        exceptionLog(ERROR, msg);
    }

    quickPrintLog(INFO, "Set new user handle.");
}

void InteractiveAddContact(ContactDB &contactDatabase)
{
    quickPrintLog(INFO, "Adding new contact...");

    string newAlias;
    do
    {
        ncOutCmd("Alias: ");
        newAlias = GetConsoleInput(false);
        if (!isPrintStr(newAlias))
        {
            ncOutUsr("Invalid alias, please enter a new one.");
            continue;
        }
    } while (newAlias.empty());
    ncOutUsr("Alias: " << newAlias);

    string newEndpoint;
    do
    {
        ncOutCmd("Endpoint: ");
        newEndpoint = GetConsoleInput(false);
        if (!isPrintStr(newEndpoint))
        {
            ncOutUsr("Invalid endpoint, please enter a new one.");
            continue;
        }
    } while (newEndpoint.empty());
    ncOutUsr("Endpoint: " << newEndpoint);

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
    int _newPort;
    ncOutCmd("Port: ");
    newPort = GetConsoleInput(false);
    try
    {
        _newPort = stoi(newPort);
    }
    catch (std::exception &e)
    {
        ncOutUsr("Invalid port! Contact not added.");
        exceptionLog(ERROR, e.what());
        return;
    }
    ncOutUsr("Port: " << _newPort);

    try
    {
        contactDatabase.addContact(Contact(newAlias, newEndpoint, _newPort));
    }
    catch (const char *msg)
    {
        quickPrintLog(ERROR, "Unable to add contact!");
        exceptionLog(ERROR, msg);
        return;
    }

    quickPrintLog(INFO, "Added contact.");
}

void InteractiveDeleteContact(ContactDB &contactDatabase)
{
    quickPrintLog(INFO, "Deleting contact...");

    string targetAlias;
    size_t startingLen = contactDatabase.getLength();

    do
    {
        ncOutCmd("Alias (must be exact): ");
        targetAlias = GetConsoleInput(false);
    } while (targetAlias.empty());
    ncOutUsr("Alias: " << targetAlias);

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
    quickPrintLog(INFO, "Adding setting...");

    string newKey;
    do
    {
        ncOutCmd("Key: ");
        newKey = GetConsoleInput(false);
    } while (newKey.empty());
    ncOutUsr("Key: " << newKey);

    for (size_t index = 0; index < newKey.length(); index++)
    {
        if (newKey.at(index) == '=' || newKey.at(index) == ':' || !isprint(newKey.at(index)))
        {
            newKey.at(index) = ' ';
        }
    }

    string newValue;
    do
    {
        ncOutCmd("Value: ");
        newValue = GetConsoleInput(false);
    } while (newValue.empty());
    ncOutUsr("Value: " << newValue);

    for (size_t index = 0; index < newValue.length(); index++)
    {
        if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
        {
            newValue.at(index) = ' ';
        }
    }

    string newDescription;
    do
    {
        ncOutCmd("Description: ");
        newDescription = GetConsoleInput(false);
    } while (newDescription.empty());
    ncOutUsr("Description: " << newDescription);

    for (size_t index = 0; index < newDescription.length(); index++)
    {
        if (newDescription.at(index) == '=' || newDescription.at(index) == ':' || !isprint(newDescription.at(index)))
        {
            newDescription.at(index) = ' ';
        }
    }

    try
    {
        settingDatabase.addSetting(Setting(newKey, newValue, newValue, newDescription));
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
    quickPrintLog(INFO, "Deleting setting...");

    string targetKey;
    size_t startingLen = settingDatabase.getLength();

    do
    {
        ncOutCmd("Key: ");
        targetKey = GetConsoleInput(false);
    } while (targetKey.empty());
    ncOutUsr("Key: " << targetKey);

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
    quickPrintLog(INFO, "Changing setting...");

    Setting targetSetting;
    string newValue;
    string oldDescription;
    string oldDefault;

    string targetKey;
    do
    {
        ncOutCmd("Key: ");
        targetKey = GetConsoleInput(false);
    } while (targetKey.empty());
    targetSetting = settingDatabase.searchKey(targetKey);

    if (!targetSetting.empty())
    {
        ncOutUsr(targetSetting.getKey() << "=" << targetSetting.getValue());
        ncOutCmd(targetSetting.getKey() << "=");

        newValue = GetConsoleInput(false);
        for (size_t index = 0; index < newValue.length(); index++)
        {
            if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
                newValue.at(index) = ' ';
        }
        oldDescription = targetSetting.getDescription();
        oldDefault = targetSetting.getDefault();

        try
        {
            settingDatabase.deleteSetting(targetSetting);
            settingDatabase.addSetting(Setting(targetKey, newValue, oldDefault, oldDescription));
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
        ncOutUsr("No setting found!");
    }
}