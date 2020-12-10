#include <fstream>
#include <vector>
#include "contact.h"

using namespace std;

ContactDB::ContactDB()
{
}

bool ContactDB::IsDuplicate(Contact testContact)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetAlias() == testContact.GetAlias())
            return true;
    }

    return false;
}

void ContactDB::AddContact(Contact newContact)
{
    if (!newContact.Empty())
    {
        if (!IsDuplicate(newContact))
            this->database.push_back(newContact);
        else
            throw "Duplicate contact alias!";
    }
    else
        throw "Contact empty!";
}

void ContactDB::AddContact(std::string info)
{
    Contact newContact(info);

    if (!newContact.Empty())
    {
        if (!IsDuplicate(newContact))
            this->database.push_back(newContact);
        else
            throw "Duplicate contact alias!";
    }
    else
        throw "Contact empty!";
}

void ContactDB::DeleteContact(Contact targetContact)
{
    size_t startingSize = this->database.size();

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index) == targetContact)
        {
            this->database.erase(this->database.begin() + index);
            break;
        }
    }
    if (startingSize <= this->database.size())
        throw "Contact not found!";
}

void ContactDB::DeleteContact(string targetInfo)
{
    size_t startingSize = this->database.size();

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).ToString() == targetInfo)
        {
            this->database.erase(this->database.begin() + index);
            break;
        }
    }
    if (startingSize <= this->database.size())
        throw "Contact not found!";
}


Contact ContactDB::SearchAlias(string tarGetAlias)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetAlias() == tarGetAlias)
            return this->database.at(index);
    }

    return Contact();
}

std::vector<Contact> ContactDB::SearchEndpoint(std::string tarGetEndpoint)
{
    std::vector<Contact> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetEndpoint() == tarGetEndpoint)
            results.push_back(this->database.at(index));
    }

    return results;
}

std::vector<Contact> ContactDB::SearchPort(std::string tarGetPort)
{
    std::vector<Contact> results;
    short tarGetPortBuf;

    try
    {
        tarGetPortBuf = stoi(tarGetPort);
    }
    catch (const std::exception &e)
    {
        throw "Invalid port!";
    }

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetPort() == tarGetPortBuf)
            results.push_back(this->database.at(index));
    }

    return results;
}

std::vector<Contact> ContactDB::SearchPort(short tarGetPort)
{
    std::vector<Contact> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).GetPort() == tarGetPort)
        {
            results.push_back(this->database.at(index));
        }
    }

    return results;
}

void ContactDB::Open(string newPath)
{
    this->database_file.open(newPath, ios::in);
    if (this->database_file.fail())
        throw "Failed to open contact database file!";
    this->path = newPath;
    this->database_file.close();
}

void ContactDB::Load()
{
    string contactLine;

    this->database_file.open(this->path, ios::in);
    if (this->database_file.fail())
        throw "Failed to open contact database file!";

    this->database_file.seekg(ios::beg);

    while (getline(this->database_file, contactLine))
    {
        if (contactLine != string())
            this->database.push_back(Contact(contactLine));
    }
    this->database_file.close();
}

void ContactDB::Clear()
{
    this->database.clear();
}

void ContactDB::Save()
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

Contact ContactDB::GetIndex(size_t targetIndex)
{
    if (targetIndex < this->database.size())
        return this->database.at(targetIndex);
    return Contact();
}

size_t ContactDB::GetLength()
{
    return this->database.size();
}
