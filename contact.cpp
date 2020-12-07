#include <iostream>
#include <sstream>
#include "contact.h"

using namespace std;

Contact::Contact() // default constructor
{
    this->alias = "null"; // default "null"
    this->hostname = "null"; // default "null"
    this->port = 0; // default 0
}

Contact::Contact(string info) // string constructor, meant from initializing from string containing csv's
{
    stringstream csv_stream(info); // used for extracting csv's from info, initialize with info
    string portStr; // used for storing the string equivalent port value extracted from csv_stream
    unsigned int commaCount = 0; // used for counting commas (error checking)

    for (size_t index = 0; index < info.length(); index++) // for all characters in info
    {
        if (info.at(index) == ',') // if character is a comma
            commaCount++; // count it
    }

    if (commaCount < 2) // if there is not enough information (alias, hostname, port)
    {
        this->alias = "null"; // initialize defaults
        this->hostname = "null";
        this->port = 0;
        return;
    }

    getline(csv_stream, this->alias, ','); // extract alias, first csv
    getline(csv_stream, this->hostname, ','); // extract hostname, second csv
    getline(csv_stream, portStr); // extract port number, third csv, ignore delim, read till end of line
    try // exception handing for stoi(str)
    {
        this->port = stoi(portStr); // convert portStr to integer and store it as port
    }
    catch (const std::exception &e) // if value is invalid argument type
    {
        this->port = 0; // resort to default
    }
}

Contact::Contact(string newAlias, string newHostname, int newPort) // constructor with all data
{
    this->alias = newAlias;
    this->hostname = newHostname;
    this->port = newPort;
}

void Contact::setAlias(string newAlias) // set alias
{
    this->alias = newAlias;
}

void Contact::setHostname(string newHostname) // set hostname
{
    this->hostname = newHostname;
}

void Contact::setPort(int newPort) // set port
{
    this->port = newPort;
}

string Contact::getAlias() // get alias
{
    return this->alias;
}

string Contact::getHostname() // get hostname
{
    return this->hostname;
}

int Contact::getPort() // get port
{
    return this->port;
}

string Contact::toString()
{
    ostringstream stringBuilder;

    stringBuilder << this -> alias << ',';
    stringBuilder << this -> hostname << ',';
    stringBuilder << this -> port;
    
    return stringBuilder.str();
}