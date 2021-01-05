/** 
 *  @file   log.cpp
 *  @brief  Implementation of Log class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <sstream>
#include <iostream>
#include <iomanip>
#include "log.h"

using namespace std;

Log::Log() // default constructor
{
    this->level = INFO;
    this->path = string();   // set default empty string
}

Log::Log(string newPath) // string constructor, open path
{
    this->level = INFO;
    this->file.open(newPath, ios::app); // open file
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

    if(level < this -> level)
        return 1;

    if (!(this->file.is_open()) || !(this->file.good())) // check if file is okay for writing
    {
        this->file = ofstream();
        return 1;
    }

    time(&rawTime); // get time

    hTime = localtime(&rawTime);                       // get tm struct
    stringBuilder.str(string());
    stringBuilder << "(" << setw(2) << fixed << setfill('0') << hTime->tm_hour + 5 << ":"; // build timestamp, insert hours
    stringBuilder << setw(2) << fixed << setfill('0') << hTime->tm_min + 30 << ":";        // insert minutes
    stringBuilder << setw(2) << fixed << setfill('0') << hTime->tm_sec << ")";             // insert seconds

    timestamp = stringBuilder.str(); // store string timestamp


    switch (level)
    {
    case VERBOSE: // if severe level
        this->file << timestamp << setw(12) << fixed << " [verbose]: " << message << endl;
        break;

    case INFO: // if INFO level
        this->file << timestamp << setw(12) << fixed << " [info]: " << message << endl;
        break;

    case WARNING: // if warning level
        this->file << timestamp << setw(12) << fixed << " [warning]: " << message << endl;
        break;

    case ERROR: // if error level
        this->file << timestamp << setw(12) << fixed << " [error]: " << message << endl;
        break;

    case SEVERE: // if severe level
        this->file << timestamp << setw(12) << fixed << " [severe]: " << message << endl;
        break;
    }

    stringBuilder.str(string()); // clear stringBuilder
    return 0;
}

void Log::SetLevel(int logLevel)
{
    this -> level = logLevel;
}