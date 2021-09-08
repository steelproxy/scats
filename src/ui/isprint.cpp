#include "isprint.h"

bool isPrintKey(int test)
{
    if (test >= 32 && test <= 126)
        return true;
    return false;
}

bool isPrintStr(std::string str)
{
    for (size_t index = 0; index < str.length(); index++)
    {
        if (!isPrintKey(str.at(index)))
        {
            return false;
        }
    }
    return true;
}