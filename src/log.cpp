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

// TODO: Fix weird ass time error, potentially related to UTC
string makeTimestamp()
{
    std::ostringstream stringBuilder;
    time_t rawTime; // used for storing result of time()
    time(&rawTime); // get time

    struct tm *hTime;            // used for accessing time data
    hTime = localtime(&rawTime); // get tm struct
    stringBuilder.str(string());
    stringBuilder << "(" << setw(2) << fixed << setfill('0') << hTime->tm_hour - 5 << ":"; // build timestamp, insert hours
    stringBuilder << setw(2) << fixed << setfill('0') << hTime->tm_min << ":";        // insert minutes
    stringBuilder << setw(2) << fixed << setfill('0') << hTime->tm_sec << ")";             // insert seconds

    std::string timestamp;           // used for storing timestamp
    timestamp = stringBuilder.str(); // store string timestamp
    return timestamp;
}

Log::Log() // default constructor
{
    this->level = INFO;
    this->path = string(); // set default empty string
}

Log::Log(string newPath) // string constructor, open path
{
    this->level = INFO;
    this->file.open(newPath, ios::app); // open file
    this->path = newPath;               // set new path
}

void Log::open(string newPath) // opens the log file
{
    this->path = newPath;               // set new path
    this->file.open(newPath, ios::app); // open file
    if (this->file.fail())              // if file fails
    {
        throw "Unable to open file!";
    }
}

void Log::close() // close the log file
{
    this->file.close();
}

void Log::truncate() // truncate log, reopen in trunc mode
{
    this->file.close();                                 // close file for reopening
    this->file.open(this->path, ios::trunc | ios::out); // reopen file in truncated mode
    if (this->file.fail())                              // if it fails to open
    {
        this->file = ofstream(); // set default empty ofstream
        throw "Unable to open file!";
    }
}

void Log::writeLine(LogLevel level, const char *func, const int line, std::string message) // write a line to the log file
{
    if (level < this->level)
    {
        return;
    }

    if (!(this->file.is_open()) || !(this->file.good())) // check if file is okay for writing
    {
        this->file = ofstream();
        throw "Unable to open file!";
    }

    string timestamp = makeTimestamp();
    switch (level)
    {
    case VERBOSE: // if severe level
        this->file << timestamp << setw(12) << fixed << " [verbose] {" << func << ":" << line << "}: " << message << endl;
        break;

    case INFO: // if INFO level
        this->file << timestamp << setw(12) << fixed << " [info] {" << func << ":" << line << "}: " << message << endl;
        break;

    case WARNING: // if warning level
        this->file << timestamp << setw(12) << fixed << " [warning] {" << func << ":" << line << "}: " << message << endl;
        break;

    case ERROR: // if error level
        this->file << timestamp << setw(12) << fixed << " [error] {" << func << ":" << line << "}: " << message << endl;
        break;

    case SEVERE: // if severe level
        this->file << timestamp << setw(12) << fixed << " [severe] {" << func << ":" << line << "}: " << message << endl;
        break;
    }
}

void Log::setLevel(LogLevel newLevel)
{
    this->level = newLevel;
}

LogLevel LevelToI(string level)
{
    if (level == "verbose")
    {
        return VERBOSE;
    }
    else if (level == "info")
    {
        return INFO;
    }
    else if (level == "warning")
    {
        return WARNING;
    }
    else if (level == "error")
    {
        return ERROR;
    }
    else if (level == "severe")
    {
        return SEVERE;
    }
    else
    {
        return INFO;
    }
}