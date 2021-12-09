/**
 *  @file   log.cpp
 *  @brief  Implementation of Log class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include "log.h"
#include <ctime>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/os.h>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string makeTimestamp()
{
    time_t rawTime; // used for storing result of time()
    time(&rawTime); // get time

    // format timestamp
    std::string formattedTime;
    formattedTime = fmt::format("({:%H:%M:%S})", fmt::localtime(rawTime));

    return formattedTime;
}

void Log::close() // close the log file
{
    this->file.close();
}

std::string
Log::formatLine(LogLevel level, const char *func, const int line,
                const std::string &message) // write a line to the log file
{
    // format function and line number
    std::string formattedFunc;
    formattedFunc             = fmt::format("{{{0}:{1}}}", func, line);

    // check longest function line
    static size_t longestFunc = 0;
    if (formattedFunc.length() > longestFunc) {
        longestFunc = formattedFunc.length();
    }

    // format severity string
    static const char *severityString[5] = {"[verbose]", "[info]", "[warning]",
                                            "[error]", "[severe]"};
    std::string        formattedSeverityString;
    formattedSeverityString = fmt::format("{:>9}", severityString[level]);

    // print message
    std::string formattedMesssage =
        fmt::format("{} {:>{}} {}: \"{}\" \n", makeTimestamp(), formattedFunc,
                    longestFunc, formattedSeverityString, message);
    return formattedMesssage;
}

void Log::writeLine(LogLevel level, const char *func, const int line,
                    const std::string &message)
{
    if (level < this->level) {
        return;
    }
    file.print("{}", this->formatLine(level, func, line, message));
}

void     Log::setLevel(LogLevel newLevel) { this->level = newLevel; }

LogLevel LevelToI(const std::string &level)
{
    if (level == "verbose") {
        return VERBOSE;
    } else if (level == "info") {
        return INFO;
    } else if (level == "warning") {
        return WARNING;
    } else if (level == "error") {
        return ERROR;
    } else if (level == "severe") {
        return SEVERE;
    } else {
        return INFO;
    }
}