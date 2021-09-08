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
#include <fmt/chrono.h>
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

void Log::close() // close the log file
{
    //this->file.flush();
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
    formattedFunc = fmt::format("{{{0}:{1}}}", func, line);
    
    // check longest function line
    static size_t longestFunc = 0;
    if (formattedFunc.length() > longestFunc)
    {
        longestFunc = formattedFunc.length();
    }

    // format severity string
    static const char *severityString[5] = {
        " [verbose] ", " [info] ", " [warning] ", " [error] ", " [severe] "};
    std::string formattedSeverityString;
    formattedSeverityString = fmt::format("{:>12}", severityString[level]);
    
    // print message
    file.print("{} {:>{}} {}: \"{}\" \n", makeTimestamp(), formattedFunc, longestFunc, formattedSeverityString, message);

   //this->file.print("{} {:>12}{}{:>{}}{}{}\n", makeTimestamp(), severityString[level], longestFunc, formattedFunc, message);
    /*this->file << makeTimestamp() << std::setw(12) << std::fixed
               << severityString[level] << std::setw(longestFunc)
               << formattedFunc << message << std::endl;*/
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