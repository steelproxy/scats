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
    this->defaultValue = string();
}

Setting::Setting(string newSetting)
{
    unsigned specialCount;
    stringstream stringBuilder(newSetting);

    for (size_t index = 0; index < newSetting.length(); index++)
    {
        if (newSetting.at(index) == ',')
        {
            specialCount++;
        }
    }

    string keyBuf;
    string valueBuf;
    string defaultValueBuf;
    string descriptionBuf;
    getline(stringBuilder, keyBuf, ',');
    getline(stringBuilder, valueBuf, ',');
    getline(stringBuilder, defaultValueBuf, ',');
    getline(stringBuilder, descriptionBuf);

    if (specialCount < 3)
    {
        this->key = string();
        this->value = string();
        this->defaultValue = string();
        this->description = string();
    }
    else
    {
        this->key = keyBuf;
        this->value = valueBuf;
        this->defaultValue = defaultValueBuf;
        this->description = descriptionBuf;
    }
}

Setting::Setting(string newKey, string newValue, string newDefaultValue, string newDescription)
{
    this->key = newKey;
    this->value = newValue;
    this->defaultValue = newDefaultValue;
    this->description = newDescription;
}

string Setting::getKey()
{
    return this->key;
}

string Setting::getValue()
{
    return this->value;
}

string Setting::getDefault()
{
    return this->defaultValue;
}

string Setting::getDescription()
{
    return this->description;
}

void Setting::setKey(string newKey)
{
    this->key = newKey;
}

void Setting::setValue(string newValue)
{
    this->value = newValue;
}

void Setting::setDefault(string newDefaultValue)
{
    this->defaultValue = newDefaultValue;
}

void Setting::setDescription(string newDescription)
{
    this->description = newDescription;
}

string Setting::toString()
{
    ostringstream stringBuilder;
    stringBuilder << this->key << ',' << this->value << ',' << this->defaultValue << ',' << this->description;
    return stringBuilder.str();
}

bool Setting::empty()
{
    if (this->key.empty() || this->value.empty() || this->defaultValue.empty() || this->description.empty())
        return true;
    return false;
}

bool operator==(Setting s1, Setting s2)
{
    if (s1.key == s2.key &&
        s1.value == s2.value && 
        s1.defaultValue == s2.defaultValue &&
        s1.description == s2.description)
    {
        return true;
    }
    return false;
}
