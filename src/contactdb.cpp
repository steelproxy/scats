/** 
 *  @file   contactdb.cpp
 *  @brief  Implementation of ContactDB class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <fstream>
#include <vector>
#include "contact.h"

using namespace std;

ContactDB::ContactDB()
{
}

bool ContactDB::checkDuplicate(Contact testContact)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getAlias() == testContact.getAlias())
            return true;
    }

    return false;
}

void ContactDB::addContact(Contact newContact)
{
    if (!newContact.empty())
    {
        if (!checkDuplicate(newContact))
            this->database.push_back(newContact);
        else
            throw "Duplicate Contact!";
    }
    else
        throw "Contact is empty!";
}

void ContactDB::addContact(std::string info)
{
    Contact newContact(info);

    if (!newContact.empty())
    {
        if (!checkDuplicate(newContact))
            this->database.push_back(newContact);
        else
            throw "Duplicate Contact!";
    }
    else
        throw "Contact is empty!";
}

void ContactDB::deleteContact(Contact targetContact)
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

void ContactDB::deleteContact(string targetInfo)
{
    size_t startingSize = this->database.size();

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).toString() == targetInfo)
        {
            this->database.erase(this->database.begin() + index);
            break;
        }
    }
    if (startingSize <= this->database.size())
        throw "Contact not found!";
}

Contact ContactDB::searchAlias(string targetAlias)
{
    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getAlias() == targetAlias)
            return this->database.at(index);
    }

    return Contact();
}

std::vector<Contact> ContactDB::searchEndpoint(std::string targetEndpoint)
{
    std::vector<Contact> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getEndpoint() == targetEndpoint)
            results.push_back(this->database.at(index));
    }

    return results;
}

std::vector<Contact> ContactDB::searchPort(std::string targetPort)
{
    std::vector<Contact> results;
    short targetPortBuf;

    try
    {
        targetPortBuf = stoi(targetPort);
    }
    catch (const std::exception &e)
    {
        return results;
    }

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getPort() == targetPortBuf)
            results.push_back(this->database.at(index));
    }

    return results;
}

std::vector<Contact> ContactDB::searchPort(short targetPort)
{
    std::vector<Contact> results;

    for (size_t index = 0; index < this->database.size(); index++)
    {
        if (this->database.at(index).getPort() == targetPort)
        {
            results.push_back(this->database.at(index));
        }
    }

    return results;
}

void ContactDB::open(string newPath)
{
    this->database_file.open(newPath, ios::in);
    if (this->database_file.fail())
        throw "Unable to open file!";
    this->path = newPath;
    this->database_file.close();
}

void ContactDB::load()
{
    string contactLine;

    this->database_file.open(this->path, ios::in);
    if (this->database_file.fail())
        throw "Unable to open file!";

    this->database_file.seekg(ios::beg);

    while (getline(this->database_file, contactLine))
    {
        if (contactLine != string())
            this->database.push_back(Contact(contactLine));
    }
    this->database_file.close();
}

void ContactDB::clear()
{
    this->database.clear();
}

void ContactDB::save()
{
    this->database_file.open(path, ios::trunc | ios::out);
    if (this->database_file.fail())
        throw "Unable to open file!";

    for (size_t index = 0; index < this->database.size(); index++)
    {
        this->database_file << this->database.at(index).toString() << endl;
    }
    this->database_file.close();
}

Contact ContactDB::getIndex(size_t targetIndex)
{
    if (targetIndex < this->database.size())
        return this->database.at(targetIndex);
    return Contact();
}

size_t ContactDB::getLength()
{
    return this->database.size();
}
