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
    ncout("Handle (max 16 characters, no special characters): ");
    ngetstr(userHandle);                                                          // read user handle into userHandle
    while (userHandle.length() > 16 || userHandle.length() < 1 || !isPrintStr(userHandle)) // must be less than 16 characters and be only printable characters
    {
        ncout("Handle is invalid. Please enter a new one: ");
        ngetstr(userHandle); // read a new user handle into userHandle
    }
}

void InteractiveAddContact(ContactDB &contactDatabase)
{
    string newAlias;
    string newEndpoint;
    string newPort;

    ncout("Alias: ");
    ngetstr(newAlias);
    ncout("Endpoint: ");
    ngetstr(newEndpoint);

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
        getline(cin, newPort);
        try
        {
            contactDatabase.AddContact(Contact(newAlias, newEndpoint, stoi(newPort)));
            break;
        }
        catch (const std::exception &e)
        {
           ncoutln("Unable to add contact: " << e.what());
            return;
        }
    }
    quickPrintLog(INFO, "Added contact.");
}

void InteractiveDeleteContact(ContactDB &contactDatabase)
{
    string targetAlias;
    size_t startingLen = contactDatabase.GetLength();

    ncout("Alias (must be exact): ");
    ngetstr(targetAlias);

    contactDatabase.DeleteContact(contactDatabase.SearchAlias(targetAlias));
    if (contactDatabase.GetLength() < startingLen)
        quickPrintLog(INFO, "Contact deleted.");
}

void InteractiveAddSetting(SettingDB &settingDatabase)
{
    string newKey;
    string newValue;
    string newDescription;

    ncout("Key: ");
    ngetstr(newKey);
    for (size_t index = 0; index < newKey.length(); index++)
    {
        if (newKey.at(index) == '=' || newKey.at(index) == ':' || !isprint(newKey.at(index)))
            newKey.at(index) = ' ';
    }

    ncout("Value: ");
    ngetstr(newValue);
    for (size_t index = 0; index < newValue.length(); index++)
    {
        if (newValue.at(index) == '=' || newValue.at(index) == ':' || !isprint(newValue.at(index)))
            newValue.at(index) = ' ';
    }

    ncout("Description: ");
    ngetstr(newDescription);
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
    ngetstr(targetKey);

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
    ngetstr(targetKey);
    targetSetting = settingDatabase.SearchKey(targetKey);

    if (!targetSetting.Empty())
    {
        ncoutln(targetSetting.GetKey() << "=" << targetSetting.GetValue());
        ncout(targetSetting.GetKey() << "=");

        ngetstr(newValue);
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