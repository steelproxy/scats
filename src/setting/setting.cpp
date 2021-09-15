#include "../setting/setting.h"
#include "../log/log.h"
#include "../ui/chatlog/chatlog.h"
#include "ini.h"
#include <iostream>
#include <uuid/uuid.h>

mINI::INIFile *_iniFile;
mINI::INIStructure _iniStructure;

#define GetSet(section, key, def)                                              \
    if (_iniStructure.get(section).get(key) == "")                             \
    {                                                                          \
        _iniStructure[section][key] = def;                                     \
    }

void ApplyDefaults()
{
    GetSet("General", "LogLevel", DEFAULT_LOG_LEVEL);
    GetSet("General", "CommandHistoryLength", DEFAULT_HISTORY_LEN);
    GetSet("General", "ChatHistoryLength", DEFAULT_CHAT_HISTORY_LEN);
    GetSet("General", "scrollLock", DEFAULT_SCROLLLOCK);
    GetSet("Server", "maximumFileSize", "10MB");
    GetSet("Server", "Port", "25565");
    GetSet("Server", "MaxChatParticipants", "1");

    // generate uuid
    uuid_t out;
    uuid_generate(out);

    // print uuid
    std::string uuidStr{ out, out + 16 };
    quickPrintLog(VERBOSE, "uuid generated: " << uuidStr);
    _iniStructure["General"]["TempUUID"] = uuidStr;

}

void LoadSettings()
{
    _iniFile = new mINI::INIFile(DEFAULT_SETTING_FILENAME);

    quickPrintLog(INFO, "Reading settings...");
    if (!_iniFile->read(_iniStructure))
    {
        quickPrintLog(ERROR, "Unable to read settings!");
        throw "Unable to read settings!";
    }
    ApplyDefaults();
    logger.setLevel(LevelToI(_iniStructure.get("General").get("logLevel")));
}

void SaveSettings()
{
    quickPrintLog(INFO, "Saving settings...");
    if (!_iniFile->generate(_iniStructure))
    {
        quickPrintLog(ERROR, "Unable to save settings!");
    }
}

void ListINI(mINI::INIStructure &_targetStructure)
{
    for (auto const &it : _targetStructure)
    {
        auto const &section = it.first;
        auto const &collection = it.second;
        ncOutUsr("[" << section << "]");
        for (auto const &it2 : collection)
        {
            auto const &key = it2.first;
            auto const &value = it2.second;
            ncOutUsr(key << "=" << value);
        }
    }
}

const int getInt(std::string section, std::string key, int def)
{
    std::string value = _iniStructure[section][key];
    quickLog(VERBOSE,
             "section=" << section << " key=" << key << " value=" << value);

    int _intValue = 0;
    std::stringstream ssValue(value);
    ssValue >> _intValue;

    if (_intValue > 0)
        return _intValue;
    return def;
}

bool FileExists(std::string path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
    {
        return true;
    }

    return false;
}

void SanitizeINIString(std::string &dirtyString)
{
    for (size_t index = 0; index < dirtyString.length(); index++)
    {
        switch (dirtyString.at(index))
        {
        case '[':
        case ']':
        case '=':
        case '#':
        case ';':
            dirtyString.erase(dirtyString.begin() + index);
            break;

        default:
            break;
        }
    }
}