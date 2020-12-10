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
    string keyBuf;
    string valueBuf;
    string descriptionBuf;
    stringstream stringBuilder(newSetting);

    getline(stringBuilder, keyBuf, '=');
    getline(stringBuilder, valueBuf, ':');
    getline(stringBuilder, descriptionBuf);

    this->key = keyBuf;
    this->value = valueBuf;
    this->description = descriptionBuf;
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
    stringBuilder << this->key << "=" << this->value << ":" << this -> description;
    return stringBuilder.str();
}