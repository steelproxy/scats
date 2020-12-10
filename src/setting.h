#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <vector>

class Setting
{
public:
    Setting();
    Setting(std::string newSetting);
    Setting(std::string newKey, std::string newValue, std::string newDescription);

    std::string GetKey();
    std::string GetValue();
    std::string GetDescription();

    void SetKey(std::string newKey);
    void SetValue(std::string newValue);
    void SetDescription(std::string newDescription);

    std::string ToString();

    bool Empty();

    friend bool operator ==(Setting s1, Setting s2);

private:
    std::string key;
    std::string value;
    std::string description;
};

class SettingDB
{
public:
    SettingDB();

    bool IsDuplicate(Setting testSetting);

    void AddSetting(Setting newSetting);
    void AddSetting(std::string newSettingLine);
    
    void DeleteSetting(Setting targetSetting);
    void DeleteSetting(std::string targetSettingLine);

    Setting SearchKey(std::string targetKey);
    std::vector<Setting> SearchValue(std::string targetValue);
    std::vector<Setting> SearchDescription(std::string targetDescription);

    Setting GetIndex(size_t index);
    size_t GetLength();

    void Open(std::string newPath);
    void Load();
    void Clear();
    void Save();

private:
    std::vector<Setting> database;
    std::string path;
    std::fstream database_file;
};

#endif
