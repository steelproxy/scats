/**
 *  @file   log.cpp
 *  @brief  Implementation of Log class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include "log.h"
#include <ctime>
#include <fmt/format.h>
#include <fmt/os.h>
#include <iomanip>
#include <iostream>
#include <sstream>

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
    stringBuilder << "(" << buffer << ")"; // insert seconds

    std::string timestamp;           // used for storing timestamp
    timestamp = stringBuilder.str(); // store std::string timestamp
    return timestamp;
}

Log::Log(const std::string newPath) // std::string constructor, open path
{
    this->level = INFO;
    this->file = fmt::output_file(newPath, ios::) // open file
    this->path = newPath;                    // set new path
}

void Log::open(std::string newPath) // opens the log file
{
    this->path = newPath;                    // set new path
    this->file.open(newPath, std::ios::app); // open file
    if (this->file.fail())                   // if file fails
    {
        throw "Unable to open file!";
    }
}

void Log::close() // close the log file
{
    this->file.flush();
}

void Log::writeLine(LogLevel level, const char *func, const int line,
                    const std::string &message) // write a line to the log file
{
    if (level < this->level)
    {
        return;
    }

    // format function and line number
    std::string formattedFunc;
    formattedFunc = fmt::format("{{{}:{}}}: ", func, line);
    file.print(formattedFunc);

    // check longest function line
    static size_t longestFunc = 0;
    if (formattedFunc.length() > longestFunc)
    {
        longestFunc = formattedFunc.length();
    }

    static const char *severityString[5] = {
        " [verbose] ", " [info] ", " [warning] ", " [error] ", " [severe] "};

    this->file.print("({%H}:{%M}:{%S}) {:<d}")
    this->file << makeTimestamp() << std::setw(12) << std::fixed
               << severityString[level] << std::setw(longestFunc)
               << formattedFunc << message << std::endl;
}

void Log::setLevel(LogLevel newLevel) { this->level = newLevel; }

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