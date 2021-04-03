/** 
 *  @file   settingdb.cpp
 *  @brief  Implementation of SettingDB class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "setting.h"

using namespace std;

SettingDB::SettingDB()
{
}

bool SettingDB::checkDuplicate(Setting testSetting)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getKey() == testSetting.getKey())
        {
            return true;
        }
    }

    return false;
}

void SettingDB::addSetting(Setting newSetting)
{
    if (!newSetting.empty())
    {
        if (!checkDuplicate(newSetting))
        {
            this->database.push_back(newSetting);
        }
        else
        {
            throw "Duplicate Setting!";
        }
    }
    else
    {
        throw "Setting is empty!";
    }
}

void SettingDB::addSetting(string newSettingLine)
{
    Setting newSetting;

    newSetting = Setting(newSettingLine);
    if (!newSetting.empty())
    {
        if (!checkDuplicate(newSetting))
        {
            this->database.push_back(newSetting);
        }
        else
        {
            throw "Duplicate Setting!";
        }
    }
    else
    {
        throw "Setting is empty!";
    }
}

void SettingDB::deleteSetting(Setting targetSetting)
{
    size_t startingSize = this->database.size();

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index) == targetSetting)
        {
            this->database.erase(this->database.begin() + index);
            break;
        }
    }

    if (startingSize <= this->database.size())
    {
        throw "Setting not found!";
    }
}

void SettingDB::deleteSetting(string targetSettingLine)
{
    size_t startingSize = this->database.size();

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).toString() == targetSettingLine)
        {
            this->database.erase(this->database.begin() + index);
            break;
        }
    }

    if (startingSize <= this->database.size())
    {
        throw "Setting not found!";
    }
}

Setting SettingDB::searchKey(string targetKey)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getKey() == targetKey)
        {
            return this->database.at(index);
        }
    }

    return Setting();
}

vector<Setting> SettingDB::searchValue(string targetValue)
{
    vector<Setting> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getValue() == targetValue)
        {
            results.push_back(this->database.at(index));
        }
    }

    return results;
}

vector<Setting> SettingDB::searchDescription(string targetDescription)
{
    vector<Setting> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getDescription() == targetDescription)
        {
            results.push_back(this->database.at(index));
        }
    }

    return results;
}

Setting SettingDB::getIndex(size_t index)
{
    if (index < this->database.size())
    {
        return this->database.at(index);
    }

    return Setting();
}

size_t SettingDB::getLength()
{
    return this->database.size();
}

void SettingDB::open(string newPath)
{
    this->database_file.open(newPath, ios::in);
    if (this->database_file.fail())
    {
        throw "Unable to open file!";
    }
    this->path = newPath;
    this->database_file.close();
}

void SettingDB::load()
{
    string settingLine;

    this->database_file.open(this->path, ios::in);
    if (this->database_file.fail())
    {
        throw "Unable to open file!";
    }

    this->database_file.seekg(ios::beg);

    while (getline(this->database_file, settingLine))
    {
        if (settingLine != string())
        {
            this->database.push_back(Setting(settingLine));
        }
    }
    this->database_file.close();
}

void SettingDB::clear()
{
    this->database.clear();
}

void SettingDB::save()
{
    struct stat buffer;
    if (stat(this->path.c_str(), &buffer) == 0)
    {
        this->database_file.open(this->path, ios::trunc | ios::out);
    }
    else
    {
        this->database_file.open(this->path, ios::out);
    }

    if (this->database_file.fail())
    {
        throw "Unable to open file!";
    }

    for (size_t index = 0; index < this->database.size(); index++)
    {
        this->database_file << this->database.at(index).toString() << endl;
    }

    this->database_file.close();
}

bool FileExists(string path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
    {
        return true;
    }

    return false;
}

std::string getSet(std::string targetKey)
{
    std::string result = settingDatabase.searchKey(targetKey).getValue();
    return result;
}

int getIntSet(std::string targetKey)
{
    Setting targetSetting = settingDatabase.searchKey(targetKey);
    if(targetSetting.empty())
    {
        return 100;
        throw "Unable to find setting!";
    }

    string query = targetSetting.getValue();
    int result;
    try
    {
        result = stoi(query);
    }
    catch (const std::exception &e)
    {
        try
        {
            result = stoi(targetSetting.getDefault());
        }
        catch(const std::exception& e)
        {
            throw "Unable to get default!";
        }
    }

    return result;
}