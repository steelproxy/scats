/** 
 *  @file   setting.cpp
 *  @brief  Implementation of Setting class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <string>
#include <iostream>
#include <sstream>
#include "setting.h"

using namespace std;

Setting::Setting()
{
    this->key = string();
    this->value = string();
    this->description = string();
}

Setting::Setting(string newSetting)
{
    unsigned specialCount;
    string keyBuf;
    string valueBuf;
    string descriptionBuf;
    stringstream stringBuilder(newSetting);

    for (size_t index = 0; index < newSetting.length(); index++)
    {
        if (newSetting.at(index) == '=' || newSetting.at(index) == ':')
            specialCount++;
    }

    getline(stringBuilder, keyBuf, '=');
    getline(stringBuilder, valueBuf, ':');
    getline(stringBuilder, descriptionBuf);

    if (specialCount < 2)
    {
        this->key = string();
        this->value = string();
        this->description = string();
    }
    else
    {
        this->key = keyBuf;
        this->value = valueBuf;
        this->description = descriptionBuf;
    }
}

Setting::Setting(string newKey, string newValue, string newDescription)
{
    this->key = newKey;
    this->value = newValue;
    this->description = newDescription;
}

string Setting::GetKey()
{
    return this->key;
}

string Setting::GetValue()
{
    return this->value;
}

string Setting::GetDescription()
{
    return this->description;
}

void Setting::SetKey(string newKey)
{
    this->key = newKey;
}

void Setting::SetValue(string newValue)
{
    this->value = newValue;
}

void Setting::SetDescription(string newDescription)
{
    this->description = newDescription;
}

string Setting::ToString()
{
    ostringstream stringBuilder;
    stringBuilder << this->key << "=" << this->value << ":" << this->description;
    return stringBuilder.str();
}

bool Setting::Empty()
{
    if(this -> key.empty() || this -> value.empty() || this -> description.empty())
        return true;
    return false;
}

bool operator ==(Setting s1, Setting s2)
{
    if(s1.key == s2.key &&
       s1.value == s2.value &&
       s1.description == s2.description)
       return true;
    return false;
}