#include <string>
#include <iostream>
#include <sstream>
#include "setting.h"

using namespace std;

Setting::Setting()
{
    this -> key = string();
    this -> value = string();
}

Setting::Setting(string newSetting)
{
    string keyBuf;
    string valueBuf;
    stringstream stringBuilder(newSetting);
    
    getline(stringBuilder, keyBuf, '=');
    getline(stringBuilder, valueBuf);
    this -> key = keyBuf;
    this -> value = valueBuf;
}

Setting::Setting(string newKey, string newValue)
{
    this -> key = newKey;
    this -> value = newValue;
}

string Setting::GetKey()
{
    return this -> key;
}

string Setting::GetValue()
{
    return this -> value;
}

void Setting::SetKey(string newKey)
{
    this -> key = newKey;
}

void Setting::SetValue(string newValue)
{
    this -> value = newValue;
}

string Setting::ToString()
{
    ostringstream stringBuilder;
    stringBuilder << this -> key << "=" << this -> value;
    return stringBuilder.str();
}