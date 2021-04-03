/** 
 *  @file   Setting.h
 *  @brief  Setting and SettingDB class definitions.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <vector>

///
/// @brief Macro for quick config changes.
///
#define IfSet(key) if (settingDatabase.searchKey(key).getValue() == string())

///
/// @brief Macro for quick default setting.
///
#define DefSet(key, default, description)                     \
{ \
    Setting query = settingDatabase.searchKey(key); \
    if (query == string()) \
    {                                                                    \
        settingDatabase.addSetting(Setting(key, default, default, description));  \
    } \
    else \
    { \
        settingDatabase.deleteSetting(query); \
        query.setDefault(default); \
        query.setDescription(description); \
        settingDatabase.addSetting(query); \   
    } \
}

///
///
/// @brief   Class for managing user Settings, contains key, value, and description of object.
class Setting /* @brief   Class for managing user Settings, contains key, value, and description of object. */
{
public:
    ///
    ///
    /// @details Default constructor, initializes all values to string() by default.
    Setting();

    ///
    /// @details String constructor, initializes all values to those parsed from string argument.
    /// @param newSetting String containing all information to initialize Setting object, stored in format "<key>=<value>:<description>".
    Setting(std::string newSetting);

    /// @details Full constructor, initalizes all values to those provided by arguments.
    /// @param newKey New key.
    /// @param newValue New value.
    /// @param newDescription New description.
    Setting(std::string newKey, std::string newValue, std::string newDefaultValue, std::string newDescription);

    ///
    /// @brief Gets Setting key.
    /// @return Returns key string.
    std::string getKey();

    ///
    /// @brief Gets Setting value;
    /// @return Returns value string.
    std::string getValue();

    ///
    /// @brief Gets Setting description.
    /// @return Returns description string.
    std::string getDescription();

    std::string getDefault();

    ///
    /// @brief Sets a new key.
    /// @param New key.
    void setKey(std::string newKey);

    ///
    /// @brief Sets a new value.
    /// @param[in] New value.
    void setValue(std::string newValue);

    ///
    /// @brief Sets a new description.
    /// @param New description.
    void setDescription(std::string newDescription);

    void setDefault(std::string newDefault);


    /// @brief Creates a string containing all object info.
    /// @details String in format "<key>=<value>:<description>."
    /// @return Returns string contatining all object info.
    std::string toString();

    ///
    /// @brief Checks if object is empty
    /// @return Returns true if empty, false if otherwise.
    bool empty();

    /// @brief == operator overload for comparing two Setting objects.
    /// @param s1 First operand.
    /// @param s2 Second operand.
    friend bool operator==(Setting s1, Setting s2);

private:
    std::string key;
    std::string value;
    std::string description;
    std::string defaultValue;
};

///
/// @brief Class for managing a database of Setting objects.
///
class SettingDB
{
public:
    ///
    /// @brief Default constructor, all values empty by default.
    ///
    SettingDB();

    ///
    /// @brief Checks if Setting is in database.
    /// @return true if duplicate is found, false otherwise.
    /// @param testSetting Setting to search for duplicates with.
    /// @return Returns true if duplicate found, false if otherwise.
    bool checkDuplicate(Setting testSetting);

    ///
    /// @brief Adds a new Setting (newSetting), to the database.
    /// @details Will not add if Setting contains invalid data.
    /// @param newSetting Setting to add to database.
    /// @exception "Setting is empty!" Provided Setting is empty or invalid.
    /// @exception "Duplicate Setting!" Provided Setting is a duplicate.
    void addSetting(Setting newSetting);

    ///
    /// @brief Creates and adds a new Setting by parsing the information provided a string.
    /// @details Will not add if Setting contains invalid data.
    /// @param newSettingLine Line containing key, value, and description of Setting.
    /// @exception "Setting is empty!" Provided Setting is empty or invalid.
    /// @exception "Duplicate Setting!" Provided Setting is a duplicate.
    void addSetting(std::string newSettingLine);

    ///
    /// @brief Deletes a Setting from the database.
    /// @details Will not delete if not found in database, use GetLength() to check for success.
    /// @param targetSetting Setting to be deleted.
    /// @exception "Setting not found!" Unable to find matching Setting.
    void deleteSetting(Setting targetSetting);

    ///
    /// @brief Deletes a Setting from the database provided a string.
    /// @param targetSettingLine Line containing key, value, and description of Setting.
    /// @exception "Setting not found!" Unable to find matching Setting.
    void deleteSetting(std::string targetSettingLine);

    ///
    /// @brief Deletes a Setting from the database provided the key of said Setting(s).
    /// @param targetKey Key of Setting.
    /// @return Returns Setting with matching key, or Setting() if none found.
    Setting searchKey(std::string targetKey);

    ///
    /// @brief Searches for a Setting with value matching argument.
    /// @param targetValue Sequence to search for.
    /// @return Returns vector of Settings with matching values.
    std::vector<Setting> searchValue(std::string targetValue);

    ///
    /// @brief Searches for a Setting with description matching argument.
    /// @param targetDescription Sequence to search for.
    /// @return Returns vector of Settings with matching descriptions.
    std::vector<Setting> searchDescription(std::string targetDescription);

    ///
    /// @brief Gets Setting at index in database.
    /// @param index Index of Setting.
    /// @return Returns Setting at index in database, or Setting() if none found.
    /// @exception "Out of bounds!" Index is out of bounds.
    Setting getIndex(size_t index);

    ///
    /// @brief Gets length of database.
    /// @return Returns length of database.
    size_t getLength();

    ///
    /// @brief Opens Setting database.
    /// @param newPath Path to Setting databse.
    /// @exception "Unable to open file!" Unable to open database file.
    void open(std::string newPath);

    ///
    /// @brief Loads Settings from database.
    /// @exception "Unable to open file!" Unable to open database file.
    void load();

    ///
    /// @brief Clears all Settings from database.
    void clear();

    ///
    /// @brief Save all Settings in database to file.
    /// @exception "Unable to open file!" Unable to open database file.
    void save();

private:
    std::vector<Setting> database;
    std::string path;
    std::fstream database_file;
};

///
/// @brief Checks if a file exists at given path.
/// @param path Path to file.
/// @return Returns true if file exists, false if otherwise.
bool FileExists(std::string path);

std::string getSet(std::string targetKey);
int getIntSet(std::string targetKey);

extern SettingDB settingDatabase;

#endif
