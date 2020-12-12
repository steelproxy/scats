/** 
 *  @file   settingdb.cpp
 *  @brief  Implementation of SettingDB class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <iostream>
#include <fstream>
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

void SettingDB::AddSetting(Setting newSetting)
{
    if (!newSetting.Empty())
    {
        if (!IsDuplicate(newSetting))
            this->database.push_back(newSetting);
        else
            throw "Duplicate key!";
    }
    else
        throw "Setting empty!";
}

void SettingDB::AddSetting(string newSettingLine)
{
    if (!newSettingLine.empty())
    {
        if (!IsDuplicate(Setting(newSettingLine)))
            this->database.push_back(Setting(newSettingLine));
        else
            throw "Duplicate key!";
    }
    else
        throw "Setting empty!";
}

void SettingDB::DeleteSetting(Setting targetSetting)
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
        throw "Contact not found!";
}

void SettingDB::DeleteSetting(string targetSettingLine)
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
        throw "Contact not found!";
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
    if(index < this->database.size())
        return this->database.at(index);
    return Setting();
}

size_t SettingDB::GetLength()
{
    return this->database.size();
}

void SettingDB::Open(string newPath)
{
    this->database_file.open(newPath, ios::in);
    if (this->database_file.fail())
        throw "Failed to open contact database file!";
    this->path = newPath;
    this->database_file.close();
}

void SettingDB::Load()
{
    string settingLine;

    this->database_file.open(this->path, ios::in);
    if (this->database_file.fail())
        throw "Failed to open contact database file!";

    this->database_file.seekg(ios::beg);

    while (getline(this->database_file, settingLine))
    {
        if (settingLine != string())
            this->database.push_back(Setting(settingLine));
    }
    this->database_file.close();
}

void SettingDB::Clear()
{
    this->database.clear();
}

void SettingDB::Save()
{
    this->database_file.open(path, ios::trunc | ios::out);
    if (this->database_file.fail())
        throw "Failed to open contact database file!";

    for (size_t index = 0; index < this->database.size(); index++)
    {
        this->database_file << this->database.at(index).ToString() << endl;
    }
    this->database_file.close();
}
