#include "../chatlog.h"
#include "../log.h"
#include "../setting.h"

using namespace std;

void InteractiveListContacts()
{
            ncOutUsr("Contacts: ");

            for (auto const &it : _iniStructure)
            {
                auto const &section = it.first;
                auto const &collection = it.second;
                size_t foundAt = 0;
                if ((foundAt = section.find("contact/", 0)) != string::npos)
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