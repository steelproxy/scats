/** 
 *  @file   log.cpp
 *  @brief  Implementation of Log class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <sstream>
#include <iostream>
#include "log.h"

using namespace std;

Log::Log() // default constructor
{
    this->print = false;     // set default false
    this->path = string();   // set default empty string
}

Log::Log(string newPath) // string constructor, open path
{
    this->file.open(newPath, ios::app); // open file
    this->print = false;  // set default false
    this->path = newPath; // set new path
}

int Log::Open(string newPath) // opens the log file
{
    this->path = newPath;               // set new path
    this->file.open(newPath, ios::app); // open file
    if (this->file.fail())              // if file fails
    {
        return 1;
    }
    return 0;
}

void Log::Close() // close the log file
{
    this->file.close();
}

void Log::SetPrint(bool newPrint) // set print value
{
    this->print = newPrint;
}

int Log::Truncate() // truncate log, reopen in trunc mode
{
    this->file.close();                                 // close file for reopening
    this->file.open(this->path, ios::trunc | ios::out); // reopen file in truncated mode
    if (this->file.fail())                              // if it fails to open
    {
        this->file = ofstream(); // set default empty ofstream
        return 1;
    }
    return 0;
}

int Log::WriteLine(logLevel level, std::string message) // write a line to the log file
{
    time_t rawTime;        // used for storing result of time()
    struct tm *hTime;      // used for accessing time data
    std::string timestamp; // used for storing timestamp

    if (!(this->file.is_open()) || !(this->file.good())) // check if file is okay for writing
    {
        this->file = ofstream();
        return 1;
    }

    time(&rawTime); // get time

    hTime = localtime(&rawTime);                       // get tm struct
    stringBuilder.str(string());
    stringBuilder << "(" << hTime->tm_hour + 5 << ":"; // build timestamp, insert hours
    stringBuilder << hTime->tm_min + 30 << ":";        // insert minutes
    stringBuilder << hTime->tm_sec << ")";             // insert seconds

    timestamp = stringBuilder.str(); // store string timestamp

    switch (level)
    {
    case INFO: // if INFO level
        if (this->print)
            cout << timestamp << " [info]: " << message << endl;
        this->file << timestamp << " [info]: " << message << endl;
        break;

    case WARNING: // if warning level
        if (this->print)
            cout << timestamp << " [warning]: " << message << endl;
        this->file << timestamp << " [warning]: " << message << endl;
        break;

    case ERROR: // if error level
        if (this->print)
            cout << timestamp << " [error]: " << message << endl;
        this->file << timestamp << " [error]: " << message << endl;
        break;

    case SEVERE: // if severe level
        if (this->print)
            cout << timestamp << " [severe]: " << message << endl;
        this->file << timestamp << " [severe]: " << message << endl;
        break;
    }

    stringBuilder.str(string()); // clear stringBuilder
    return 0;
}
