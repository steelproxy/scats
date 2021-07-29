/** 
 *  @file   log.cpp
 *  @brief  Implementation of Log class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include "log.h"


std::string makeTimestamp()
{
    std::ostringstream stringBuilder;
    time_t rawTime; // used for storing result of time()
    time(&rawTime); // get time

    struct tm *hTime;            // used for accessing time data
    hTime = localtime(&rawTime); // get tm struct
    
    char buffer[80];
    strftime(buffer, 80, "%T", hTime);

    stringBuilder.str(std::string());
    stringBuilder << "(" << buffer << ")";           // insert seconds

    std::string timestamp;           // used for storing timestamp
    timestamp = stringBuilder.str(); // store std::string timestamp
    return timestamp;
}

Log::Log() // default constructor
{
    this->level = INFO;
    this->path = std::string(); // set default empty std::string
}

Log::Log(std::string newPath) // std::string constructor, open path
{
    this->level = INFO;
    this->file.open(newPath, std::ios::app); // open file
    this->path = newPath;               // set new path
}

void Log::open(std::string newPath) // opens the log file
{
    this->path = newPath;               // set new path
    this->file.open(newPath, std::ios::app); // open file
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
    this->file.open(this->path, std::ios::trunc | std::ios::out); // reopen file in truncated mode
    if (this->file.fail())                              // if it fails to open
    {
        this->file = std::ofstream(); // set default empty std::ofstream
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
        this->file = std::ofstream();
        //throw "Unable to open file!";
    }

    std::ostringstream ssformattedFunc;
    std::string formattedFunc;
    ssformattedFunc << "{" << func << ":" << line << "}: ";
    formattedFunc = ssformattedFunc.str();

    static size_t longestFunc = 0;
    if (formattedFunc.length() > longestFunc)
    {
        longestFunc = formattedFunc.length();
    }

    static const char *severityString[5] = {" [verbose] ", " [info] ", " [warning] ", " [error] ", " [severe] "};

    this->file << makeTimestamp() << std::setw(12) << std::fixed << severityString[level] << std::setw(longestFunc) << formattedFunc << message << std::endl;
}

void Log::setLevel(LogLevel newLevel)
{
    this->level = newLevel;
}

LogLevel LevelToI(std::string level)
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