/**
 *  @file   interactive.cpp
 *  @brief  Interactive function definitions.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "../setting/setting.h"
#include <curses.h>
#include <vector>

///
/// @brief Interactive function for setting user handle.
/// @param database Reference to SettingDB that user handle setting will be
/// added to.
/// @param userHandle Reference to user handle.
void InteractiveSetUserHandle();

///
/// @brief Interactive function for adding contact to ContactDB.
/// @param contactDatabase Reference to ContactDB that contact will be added to.
void InteractiveAddContact();

///
/// @brief Interactive function for deleting contact from ContactDB.
/// @param contactDatabase Reference to ContactDB that contact will be deleted
/// from.
void InteractiveDeleteContact();

///
/// @brief Interactive function for adding setting to SettingDB.
/// @param settingDatabase Reference to SettingDB that setting will be added to.
void InteractiveAddSetting();

///
/// @brief Interactive function for deleting setting from SettingDB.
/// @param settingDatabase Reference to SettingDB that setting will be deleted
/// from.
void InteractiveDeleteSetting();

///
/// @brief Interactive function for changing a setting in SettingDB.
/// @param settingDatabase Reference to SettingDB that will be searched for
/// matching Setting and edited.
void InteractiveChangeSetting();

void DisplayHelp();

void InteractiveListContacts();

void InteractiveListSettings();

void InteractiveNuke();

void InteractiveEditSettings();

typedef void (*voidFunctionType)(void);

class Command
{
  public:
    Command(std::string newName, std::string newDescription,
            voidFunctionType newMainFunction)
        : name(newName), description(newDescription),
          mainFunction(newMainFunction){};
    std::string name;
    std::string description;

    void Execute() { this->mainFunction(); }

  private:
    voidFunctionType mainFunction;
};

#endif