#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

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
private:
    std::string key;
    std::string value;
    std::string description;
};

#endif
