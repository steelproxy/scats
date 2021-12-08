/**
 *  @file   log.h
 *  @brief  Log class definitions, and some useful curses magic
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#ifndef LOG_H
#define LOG_H

#include <curses.h>
#undef timeout
#include <fmt/core.h>
#include <fmt/os.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define preUserPrint(level, message)                                           \
    {                                                                          \
        std::ostringstream stringBuilder;                                      \
        stringBuilder.str(std::string());                                      \
        stringBuilder << message;                                              \
        preUser.push_back(stringBuilder.str());                                \
        logger.writeLine(level, __FUNCTION__, __LINE__, stringBuilder.str());  \
    }

#define preLog(level, message)                                                 \
    {                                                                          \
        \                              
        std::ostringstream stringBuilder;                                      \
        \                            
        stringBuilder.str(std::string());                                      \
        \                        
        stringBuilder                                                          \
            << message;                                                        \
        \                         
        preLog.push_back(logger.formatLine(level, __FUNCTION__, __LINE__,      \
                                           stringBuilder.str()));              \
    }

///
/// @brief Macro for formatting and printing messages to the log.
/// @param level Log severity level.
/// @param message Message to print to log.
#define quickLog(level, message)                                               \
    {                                                                          \
        std::ostringstream stringBuilder;                                      \
        stringBuilder.str(std::string());                                      \
        stringBuilder << message;                                              \
        logger.writeLine(level, __FUNCTION__, __LINE__, stringBuilder.str());  \
    }

///
/// @brief Macro for formatting and printing messages to the log and user.
/// @param level Log severity level.
/// @param message Message to print to log and user.
#define quickPrintLog(level, message)                                          \
    {                                                                          \
        ncOutUsr(message);                                                     \
        quickLog(level, message);                                              \
    }

#define exceptionLog(level, message)                                           \
    {                                                                          \
        quickLog(level, "Exception caught: " << message);                      \
    }

///
/// @brief Log severity level type.
///
typedef enum
{
    VERBOSE,
    INFO,
    WARNING,
    ERROR,
    SEVERE
} LogLevel;

std::string makeTimestamp();

///
/// @brief Class for managing log.
class Log
{
  public:
    ///
    /// @brief String constructor, opens log file at provided path.
    /// @param newPath Path to log file.
    explicit Log(const std::string &newPath)
        : level(INFO), file(fmt::output_file(newPath))
    {
    }

    /// @brief Opens the log for writing.
    /// @param newPath Path to log file.
    /// @exception "Unable to open file!" Unable to open log file.

    ///
    /// @brief Closes the log file.
    ///
    void close();

    /// @brief Writes a line to the log.
    /// @param level Severity level of message.
    /// @param message Message to print to log.
    /// @exception "Unable to open file!" Unable to open log file.
    void writeLine(LogLevel level, const char *func, const int line,
                   const std::string &message);

    std::string formatLine(LogLevel level, const char *func, const int line,
                           const std::string &message);
    ///
    /// @brief Sets log severity level
    /// @param LogLevel Log severity level.
    void setLevel(LogLevel newLevel);

    ///
    /// @brief Truncates log.
    /// @exception "Unable to open file!" Unable to open log file.

  private:
    int level;
    fmt::ostream file;
    std::string path;
};

/// @brief Converts a severity level string into LogLevel type.
/// @param level Severity level string to be converted.
/// @return Returns a LogLevel type of the equivalent severity, or INFO if
/// invalid.
LogLevel LevelToI(std::string level);

extern Log logger;
extern std::vector<std::string> preUser;

#endif
