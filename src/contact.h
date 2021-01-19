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

///
/// @brief Class for managing contact info
///
class Contact
{
public:
    
    ///
    /// @brief Default constructor, all values are empty.
    ///
    Contact(); // default constructor
    
    ///
    /// @brief String constructor, all values parsed from argument.
    /// @param info Comma seperated values containing alias, endpoint, and port.
    Contact(std::string info);

    /// @brief Full constructor, all values initialized to respective arguments.
    /// @param newAlias New alias.
    /// @param newEndpoint New endpoint.
    /// @param newPort New port.
    Contact(std::string newAlias, std::string newEndpoint, short newPort);

    ///
    /// @brief Sets new alias.
    /// @param New alias.
    void setAlias(std::string newAlias);

    ///
    /// @brief Sets new endpoint.
    /// @param newEndpoint New endpoint.
    void setEndpoint(std::string newEndpoint);

    ///
    /// @brief Sets new port.
    /// @param newPort New port.
    void setPort(int newPort);

    ///
    /// @brief Gets alias.
    /// @return Returns string containing alias.
    std::string getAlias();

    ///
    /// @brief Gets endpoint.
    /// @return Returns string containing endpoint.
    std::string getEndpoint();

    ///
    /// @brief Gets port.
    /// @returns Returns integer of port.
    short getPort();

    ///
    /// @brief Converts contact to string.
    /// @return Returns string of comma seperated values containing alias, endpoint, and port.
    std::string toString();

    ///
    /// @brief Checks if contact is empty.
    /// @return Returns true if empty, false if otherwise.
    bool empty();


    /// @brief == overload for comparing two contacts.
    /// @param c1 First operand.
    /// @param c2 Second operand.
    /// @return Returns true if c1 contains the same info as c2.
    friend bool operator==(const Contact &c1, const Contact &c2);

private:
    std::string alias;
    std::string endpoint;
    short port;
};

///
/// @brief Class for managing database of Contacts.
///
class ContactDB
{
public:
    
    ///
    /// @brief Default constructor, all values are empty.
    ///
    ContactDB();

    /// @brief Checks if provided Contact is the same as any in database.
    /// @param testContact Contact to scan for duplicates in database.
    /// @return Returns true if Contact is duplicate, false if otherwise.
    bool checkDuplicate(Contact testContact);

    /// @brief Adds a Contact to the database.
    /// @param newContact Contact to be added to database.
    /// @exception "Duplicate contact!" Duplicate contact found.
    void addContact(Contact newContact);

    /// @brief Creates and adds a new Contact to the database
    /// @param info Comma seperated values containing alias, endpoint, and port of Contact to be added.
    /// @exception "Duplicate contact!" Duplicate Contact found.
    void addContact(std::string info);

    /// @brief Deletes a Contact from the database.
    /// @param targetContact Contact to delete from database.
    /// @exception "Contact not found!" Contact not found.
    void deleteContact(Contact targetContact);

    /// @brief Deletes a Contact from the database, provided CSVs.
    /// @param targetInfo Comma seperated values containing alias, endpoint, and port.
    /// @exception "Contact not found!" Contact not found.
    void deleteContact(std::string targetInfo);

    /// @brief Searches for Contact with provided alias in database.
    /// @param targetAlias Alias of target Contact.
    Contact searchAlias(std::string targetAlias);
    std::vector<Contact> searchEndpoint(std::string targetEndpoint);
    std::vector<Contact> searchPort(std::string targetPort);
    std::vector<Contact> searchPort(short targetPort);

    Contact getIndex(size_t index);
    size_t getLength();

    void open(std::string newPath);
    void load();
    void clear();
    void save();

private:
    std::vector<Contact> database;
    std::string path;
    std::fstream database_file;
};

#endif