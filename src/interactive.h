#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <vector>
#include "contact.h"
#include "setting.h"

void InteractiveSetUserHandle(std::string &userHandle);

void InteractiveAddContact(ContactDB &contactDatabase);
void InteractiveDeleteContact(ContactDB &contactDatabase);

void InteractiveAddSetting(SettingDB &settingDatabase);
void InteractiveDeleteSetting(SettingDB &settingDatabase);

#endif