#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

class Setting
{
public:
    Setting();
    Setting(std::string newSetting);
    Setting(std::string newKey, std::string newValue);

    std::string GetKey();
    std::string GetValue();

    void SetKey(std::string newKey);
    void SetValue(std::string newValue);

    std::string ToString();
private:
    std::string key;
    std::string value;
};

#endif
