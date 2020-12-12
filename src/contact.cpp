/** 
 *  @file   contact.cpp
 *  @brief  Contact class implementation. 
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <iostream>
#include <sstream>
#include "contact.h"

using namespace std;

Contact::Contact() // default constructor
{
    this->alias = "";    // default empty
    this->endpoint = ""; // default empty
    this->port = 0;      // default 0
}

Contact::Contact(string info) // string constructor, meant from initializing from string containing csv's
{
    stringstream csv_stream(info); // used for extracting csv's from info, initialize with info
    string portStr;                // used for storing the string equivalent port value extracted from csv_stream
    unsigned int commaCount = 0;   // used for counting commas (error checking)

    for (size_t index = 0; index < info.length(); index++) // for all characters in info
    {
        if (info.at(index) == ',') // if character is a comma
            commaCount++;          // count it
    }

    if (commaCount < 2) // if there is not enough information (alias, endpoint, port)
    {
        this->alias = ""; // initialize defaults
        this->endpoint = "";
        this->port = 0;
        return;
    }

    getline(csv_stream, this->alias, ',');    // extract alias, first csv
    getline(csv_stream, this->endpoint, ','); // extract endpoint, second csv
    getline(csv_stream, portStr);             // extract port number, third csv, ignore delim, read till end of line
    try                                       // exception handing for stoi(str)
    {
        this->port = stoi(portStr); // convert portStr to integer and store it as port
    }
    catch (const std::exception &e) // if value is invalid argument type
    {
        this->port = 0; // resort to default
    }
}

Contact::Contact(string newAlias, string newEndpoint, short newPort) // constructor with all data
{
    this->alias = newAlias;
    this->endpoint = newEndpoint;
    this->port = newPort;
}

void Contact::SetAlias(string newAlias) // set alias
{
    this->alias = newAlias;
}

void Contact::SetEndpoint(string newEndpoint) // set endpoint
{
    this->endpoint = newEndpoint;
}

void Contact::SetPort(int newPort) // set port
{
    this->port = newPort;
}

string Contact::GetAlias() // get alias
{
    return this->alias;
}

string Contact::GetEndpoint() // get endpoint
{
    return this->endpoint;
}

short Contact::GetPort() // get port
{
    return this->port;
}

string Contact::ToString()
{
    ostringstream stringBuilder;

    stringBuilder << this->alias << ',';
    stringBuilder << this->endpoint << ',';
    stringBuilder << this->port;

    return stringBuilder.str();
}

bool Contact::Empty()
{
    if (this->GetAlias().empty() || this->GetEndpoint().empty() || this->GetPort() == 0)
        return true;
    return false;
}

bool operator==(const Contact &c1, const Contact &c2)
{
    if ((c1.alias == c2.alias) &&
        (c1.endpoint == c2.endpoint) &&
        (c1.port == c2.port))
        return true;
    return false;
}
