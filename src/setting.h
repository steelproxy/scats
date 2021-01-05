/** 
 *  @file   setting.h
 *  @brief  Setting and SettingDB class definitions.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <vector>

///
/// @brief   Class for managing user settings, contains key, value, and description of object.
///
class Setting /* @brief   Class for managing user settings, contains key, value, and description of object. */
{
public:
    ///
    /// @details Default constructor, initializes all values to string() by default.
    ///
    Setting();

    /// @details String constructor, initializes all values to those parsed from string argument.
    /// @param[in] newSetting String containing all information to initialize Setting object, stored in format "<key>=<value>:<description>".
    ///
    Setting(std::string newSetting);

    /// @details Full constructor, initalizes all values to those provided by arguments.
    /// @param[in] newKey New key.
    /// @param[in] newValue New value.
    /// @param[in] newDescription New description.
    Setting(std::string newKey, std::string newValue, std::string newDescription);

    /// @brief Gets setting key.
    /// @return Returns key string.
    ///
    std::string GetKey();

    /// @brief Gets setting value;
    /// @return Returns value string.
    ///
    std::string GetValue();

    /// @brief Gets setting description.
    /// @return Returns description string.
    ///
    std::string GetDescription();

    /// @brief Sets a new key.
    /// @param[in] New key.
    ///
    void SetKey(std::string newKey);

    /// @brief Sets a new value.
    /// @param[in] New value.
    ///
    void SetValue(std::string newValue);

    /// @brief Sets a new description.
    /// @param[in] New description
    ///
    void SetDescription(std::string newDescription);

    /// @brief Creates a string containing all object info.
    /// @details String in format "<key>=<value>:<description>."
    /// @return Returns string contatining all object info.
    std::string ToString();

    /// @brief Checks if object is empty
    /// @return true if empty, false if not.
    ///
    bool Empty();

    /// @brief == operator overload for comparing two Setting objects.
    /// @param[in] s1 First operand.
    /// @param[in] s2 Second operand.
    friend bool operator==(Setting s1, Setting s2);

private:
    ///
    /// @brief String containing key.
    ///
    std::string key;

    ///
    /// @brief String containing value.
    ///
    std::string value;

    ///
    /// @brief String containing
    ///
    std::string description;
};

///
/// @brief Class for managing a database of Settings.
///
class SettingDB
{
public:
    ///
    /// @brief Default constructor, all values empty by default.
    ///
    SettingDB();

    ///
    /// @brief Checks if testSetting is in databse.
    /// @return true if duplicate is found, false otherwise.
    bool IsDuplicate(Setting testSetting);

    ///
    /// @brief Adds a new Setting (newSetting), to the database.
    /// @details Will not add if newSetting contains empty data.
    int AddSetting(Setting newSetting);

    ///
    /// @brief Creates and adds a new setting by parsing the information provided in newSettingLine
    /// @details Will not add Setting if newSettingLine contains invalid data.
    int AddSetting(std::string newSettingLine);

    ///
    /// @brief Deletes a setting from the database.
    /// @details Will not delete if not found in database, use GetLength() to check for success.
    int DeleteSetting(Setting targetSetting);

    ///
    /// @brief Deletes a setting from the database provided the information in targetSettingLine.
    int DeleteSetting(std::string targetSettingLine);

    Setting SearchKey(std::string targetKey);
    std::vector<Setting> SearchValue(std::string targetValue);
    std::vector<Setting> SearchDescription(std::string targetDescription);

    Setting GetIndex(size_t index);
    size_t GetLength();

    int Open(std::string newPath);
    int Load();
    void Clear();
    int Save();

private:
    std::vector<Setting> database;
    std::string path;
    std::fstream database_file;
};

bool FileExists(std::string path);

#endif
