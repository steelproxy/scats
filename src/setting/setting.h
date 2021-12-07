#ifndef SETTING_H
#define SETTING_H

#include "ini.h"
#include <map>

// General
//----------------------------------------------------------------
#define DEFAULT_SETTINGS_FILE "settings.ini"
#define DEFAULT_LOG_LEVEL "info"
#define DEFAULT_LOG_FILE "log.txt"
#define DEFAULT_TRUNCATE_LOG "false"
#define DEFAULT_HISTORY_LEN "64"
#define DEFAULT_CHAT_HISTORY_LEN "500"
#define DEFAULT_SCROLLLOCK "false"
#define DEFAULT_SETTING_FILENAME "settings.ini"
#define DEFAULT_MAX_FILE_SIZE "10mb"
#define DEFAULT_SERVER_LISTEN_PORT "25565"
#define DEFAULT_MAX_CHAT_PARTICIPANTS "10"
#define DEFAULT_SHOW_LINE_NUMBERS "true"
//----------------------------------------------------------------

const std::map<std::pair<const std::string, const std::string>,
               const std::string>
    _defaultMap = {
        {std::make_pair("General", "chatHistoryLength"),
         DEFAULT_CHAT_HISTORY_LEN},
        {std::make_pair("General", "commandHistoryLength"),
         DEFAULT_HISTORY_LEN},
        {std::make_pair("General", "logLevel"), DEFAULT_LOG_LEVEL},
        {std::make_pair("General", "scrollLock"), DEFAULT_SCROLLLOCK},
        {std::make_pair("General", "showLineNumbers"),
         DEFAULT_SHOW_LINE_NUMBERS},
        {std::make_pair("Server", "maximumFileSize"), DEFAULT_MAX_FILE_SIZE},
        {std::make_pair("Server", "port"), DEFAULT_SERVER_LISTEN_PORT},
        {std::make_pair("Server", "maxChatParticipants"),
         DEFAULT_MAX_CHAT_PARTICIPANTS}};

extern mINI::INIStructure _iniStructure;

void LoadSettings();
void SaveSettings();

bool FileExists(std::string);
const int getInt(std::string,
                 std::string); // todo: implement default value map search
const bool getBool(std::string, std::string);

void SanitizeINIString(std::string &dirtyString);
void ListINI(mINI::INIStructure &_targetStructure);

#endif