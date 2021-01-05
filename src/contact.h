/** 
 *  @file   contact.cpp
 *  @brief  Contact definition file. 
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#ifndef CONTACT_H
#define CONTACT_H

#include <vector>
#include <string>
#include <fstream>

class Contact
{
public:
    Contact(); // default constructor
    Contact(std::string info);
    Contact(std::string newAlias, std::string newEndpoint, short newPort);

    void SetAlias(std::string newAlias);
    void SetEndpoint(std::string newEndpoint);
    void SetPort(int newPort);

    std::string GetAlias();
    std::string GetEndpoint();
    short GetPort();

    std::string ToString();

    bool Empty();

    friend bool operator==(const Contact &c1, const Contact &c2);

private:
    std::string alias;
    std::string endpoint;
    short port;
};

class ContactDB
{
public:
    ContactDB();

    bool IsDuplicate(Contact testContact);

    int AddContact(Contact newContact);
    int AddContact(std::string info);

    int DeleteContact(Contact targetContact);
    int DeleteContact(std::string targetInfo);

    Contact SearchAlias(std::string tarGetAlias);
    std::vector<Contact> SearchEndpoint(std::string tarGetEndpoint);
    std::vector<Contact> SearchPort(std::string tarGetPort);
    std::vector<Contact> SearchPort(short tarGetPort);

    Contact GetIndex(size_t index);
    size_t GetLength();

    int Open(std::string newPath);
    int Load();
    void Clear();
    int Save();

private:
    std::vector<Contact> database;
    std::string path;
    std::fstream database_file;
};

#endif