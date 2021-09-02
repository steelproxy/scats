#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog.h"

void InteractiveListContacts()
{
    ncOutUsr("Contacts: ");

    for (auto const &it : _iniStructure)
    {
        auto const &section = it.first;
        auto const &collection = it.second;
        size_t foundAt = 0;
        if ((foundAt = section.find("contact/", 0)) != std::string::npos)
        {
            ncOutUsr("[" << section.substr(foundAt) << "]");
            for (auto const &it2 : collection)
            {
                auto const &key = it2.first;
                auto const &value = it2.second;
                ncOutUsr(key << "=" << value);
            }
        }
    }
}