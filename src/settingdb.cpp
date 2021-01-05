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

bool SettingDB::IsDuplicate(Setting testSetting)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetKey() == testSetting.GetKey())
            return true;
    }

    return false;
}

int SettingDB::AddSetting(Setting newSetting)
{
    if (!newSetting.Empty())
    {
        if (!IsDuplicate(newSetting))
            this->database.push_back(newSetting);
        else
            return 1;
    }
    else
        return 1;
    return 1;
}

int SettingDB::AddSetting(string newSettingLine)
{
    Setting newSetting;

    newSetting = Setting(newSettingLine);
    if (!newSetting.Empty())
    {
        if (!IsDuplicate(newSetting))
            this->database.push_back(newSetting);
        else
            return 1;
    }
    else
        return 1;
    return 0;
}

int SettingDB::DeleteSetting(Setting targetSetting)
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
        return 1;
    return 0;
}

int SettingDB::DeleteSetting(string targetSettingLine)
{
    size_t startingSize = this->database.size();

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).ToString() == targetSettingLine)
        {
            this->database.erase(this->database.begin() + index);
            break;
        }
    }
    if (startingSize <= this->database.size())
        return 1;
    return 0;
}

Setting SettingDB::SearchKey(string targetKey)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetKey() == targetKey)
            return this->database.at(index);
    }

    return Setting();
}

vector<Setting> SettingDB::SearchValue(string targetValue)
{
    vector<Setting> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetValue() == targetValue)
            results.push_back(this->database.at(index));
    }

    return results;
}

vector<Setting> SettingDB::SearchDescription(string targetDescription)
{
    vector<Setting> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetDescription() == targetDescription)
            results.push_back(this->database.at(index));
    }

    return results;
}

Setting SettingDB::GetIndex(size_t index)
{
    if (index < this->database.size())
        return this->database.at(index);
    return Setting();
}

size_t SettingDB::GetLength()
{
    return this->database.size();
}

int SettingDB::Open(string newPath)
{
    this->database_file.open(newPath, ios::in);
    if (this->database_file.fail())
        return 1;
    this->path = newPath;
    this->database_file.close();
    return 0;
}

int SettingDB::Load()
{
    string settingLine;

    this->database_file.open(this->path, ios::in);
    if (this->database_file.fail())
        return 1;

    this->database_file.seekg(ios::beg);

    while (getline(this->database_file, settingLine))
    {
        if (settingLine != string())
            this->database.push_back(Setting(settingLine));
    }
    this->database_file.close();
    return 0;
}

void SettingDB::Clear()
{
    this->database.clear();
}

int SettingDB::Save()
{
    struct stat buffer;
    if (stat(this->path.c_str(), &buffer) == 0)
        this->database_file.open(this->path, ios::trunc | ios::out);
    else
        this->database_file.open(this->path, ios::out);

    if (this->database_file.fail())
        return 1;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        this->database_file << this->database.at(index).ToString() << endl;
    }
    this->database_file.close();
    return 0;
}

bool FileExists(string path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
        return true;
    return false;
}