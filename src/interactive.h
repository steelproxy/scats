/** 
 *  @file   interactive.cpp
 *  @brief  Interactive function definitions.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <vector>
#include <curses.h>
#include "contact.h"
#include "setting.h"

void InteractiveSetUserHandle(std::string &userHandle);

void InteractiveAddContact(ContactDB &contactDatabase);
void InteractiveDeleteContact(ContactDB &contactDatabase);

void InteractiveAddSetting(SettingDB &settingDatabase);
void InteractiveDeleteSetting(SettingDB &settingDatabase);
void InteractiveChangeSetting(SettingDB &settingDatabase);

extern WINDOW* root;

#endif