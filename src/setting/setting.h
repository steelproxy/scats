#ifndef SETTING_H
#define SETTING_H

#include "ini.h"

#define DEFAULT_SETTINGS_FILE "settings.ini"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_FILE "log.txt"
#define DEFAULT_TRUNCATE_LOG "false"
#define DEFAULT_HISTORY_LEN "64"
#define DEFAULT_CHAT_HISTORY_LEN "500"
#define DEFAULT_SCROLLLOCK "false"
#define DEFAULT_SETTING_FILENAME "settings.ini"

extern mINI::INIStructure _iniStructure;

void LoadSettings();
void SaveSettings();

bool FileExists(std::string);
const int getInt(std::string, std::string, int); // todo: implement default value map search
const bool getBool(std::string, std::string);

void SanitizeINIString(std::string &dirtyString);
void ListINI(mINI::INIStructure &_targetStructure);

#endif