#include "../setting/setting.h"
#include "../log/log.h"
#include "../ui/chatlog/chatlog.h"
#include "ini.h"
#include <cctype>
#include <iostream>
#include <map>
#include <uuid/uuid.h>

mINI::INIFile     *_iniFile;
mINI::INIStructure _iniStructure;

// ! deprecated in favor of _defaultMap
#define GetSet(section, key, def)                                              \
    if (_iniStructure.get(section).get(key) == "") {                           \
        _iniStructure[section][key] = def;                                     \
    }

bool isNumber(const std::string &s)
{
    return std::all_of(s.begin(), s.end(),
                       [](char c) { return isdigit(c) != 0; });
}

void ApplyDefaults()
{
    // got rid of GetSet calls
    for (auto const &it : _defaultMap) {
        auto const &section = it.first.first;
        auto const &key     = it.first.second;
        auto const &value   = it.second;

        if (_iniStructure.get(section).get(key) == "") {
            _iniStructure[section][key] =
                _defaultMap.at(std::make_pair(section, key));
        }
    }

    // generate uuid
    uuid_t out;
    uuid_generate(out);

    // print uuid
    std::string uuidStr{out, out + 16};
    SanitizeINIString(uuidStr);

    // use preUserPrint since chatlog hasnt been initialized
    preUserPrint(VERBOSE, "uuid generated: " << uuidStr);
    _iniStructure["General"]["TempUUID"] = uuidStr;
}

void LoadSettings()
{
    _iniFile = new mINI::INIFile(DEFAULT_SETTING_FILENAME);

    // preuserprint since chatlog hasn't been initialized yet
    preUserPrint(INFO, "Reading settings...");
    if (!_iniFile->read(_iniStructure)) {
        preUserPrint(ERROR, "Unable to read settings!");
        throw "Unable to read settings!";
    }
    ApplyDefaults();
    logger.setLevel(LevelToI(_iniStructure.get("General").get("logLevel")));
}

void SaveSettings()
{
    if (preUser.empty()) {
        quickPrintLog(INFO, "Saving settings...");
    } else {
        preUserPrint(INFO, "Saving settings...");
    }
    if (!_iniFile->generate(_iniStructure)) {
        if (preUser.empty()) {
            quickPrintLog(ERROR, "Unable to save settings!");
        } else {
            preUserPrint(ERROR, "Unable to save settings!");
        }
    }
}

void ListINI(mINI::INIStructure &_targetStructure)
{
    for (auto const &it : _targetStructure) {
        auto const &section    = it.first;
        auto const &collection = it.second;
        ncOutUsr("[" << section << "]");
        for (auto const &it2 : collection) {
            auto const &key   = it2.first;
            auto const &value = it2.second;
            ncOutUsr(key << "=" << value);
        }
    }
}

const int getInt(std::string section, std::string key)
{
    std::string targetValue = _iniStructure[section][key];

    // log the value
    quickLog(VERBOSE, "section=" << section << " key=" << key
                                 << " targetValue=" << targetValue);

    const int defaultValue =
        std::stoi(_defaultMap.at(std::make_pair(section, key)));
    if (!isNumber(targetValue) || targetValue == "") {
        return defaultValue;
    } else {
        try {
            return std::stoi(targetValue);
        } catch (std::exception e) {
            exceptionLog(ERROR, "Unable to convert value! section="
                                    << section << " key=" << key
                                    << " value=" << targetValue);
            return defaultValue;
        }
    }
}

const bool getBool(std::string section, std::string key)
{
    std::string targetValue = _iniStructure[section][key];

    // log the value
    quickLog(VERBOSE, "section=" << section << " key=" << key
                                 << " value=" << targetValue);

    // return default value
    const std::string &defaultValue =
        _defaultMap.at(std::make_pair(section, key));
    if (targetValue != "true" && targetValue != "false") {
        return (defaultValue == "true") ? true : false;
    }

    // return calculated value
    return (targetValue == "true") ? true : false;
}

bool FileExists(std::string path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return true;
    }

    return false;
}

void SanitizeINIString(std::string &dirtyString)
{
    std::string cleanString;
    for (size_t index = 0; index < dirtyString.length(); index++) {
        if (!isprint(dirtyString[index]) || dirtyString[index] == '[' ||
            dirtyString[index] == ']' || dirtyString[index] == '=' ||
            dirtyString[index] == '#' || dirtyString[index] == ';') {
            int         keyValue    = dirtyString[index];
            std::string keyValueStr = std::to_string(keyValue);

            cleanString += '\\' + keyValueStr;
        } else {
            cleanString += dirtyString[index];
        }
    }
    dirtyString = cleanString;
}