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
#include <fstream>
#include <string>
#include <sstream>

///
/// @brief Macro for formatting and printing messages to the log.
/// @param level Log severity level.
/// @param message Message to print to log.
#define quickLog(level, message)                             \
    {                                                        \
        logger.stringBuilder.str(string());                  \
        logger.stringBuilder << message;                     \
        logger.writeLine(level, logger.stringBuilder.str()); \
    }

///
/// @brief Macro for formatting and printing messages to the log and user.
/// @param level Log severity level.
/// @param message Message to print to log and user.
#define quickPrintLog(level, message) \
    {                                 \
        ncoutln(message);             \
        quickLog(level, message);     \
    }

#define exceptionLog(level, message)   \
{   \
    quickLog(level, "Exception caught (" << __FUNCTION__ << "," << __LINE__ << "): " << message); \
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


///
/// @brief Class for managing log.
class Log
{
public:

    ///
    /// @brief Default constructor, all values empty, default level is INFO.
    ///
    Log();
    
    ///
    /// @brief String constructor, opens log file at provided path.
    /// @param newPath Path to log file.
    Log(std::string newPath);

    /// @brief Opens the log for writing.
    /// @param newPath Path to log file.
    /// @exception "Unable to open file!" Unable to open log file.
    void open(std::string newPath);

    ///
    /// @brief Closes the log file.
    ///
    void close();

    /// @brief Writes a line to the log.
    /// @param level Severity level of message.
    /// @param message Message to print to log.
    /// @exception "Unable to open file!" Unable to open log file.
    void writeLine(LogLevel level, std::string message);

    ///
    /// @brief Sets log severity level
    /// @param LogLevel Log severity level.
    void setLevel(LogLevel newLevel);

    ///
    /// @brief Truncates log.
    /// @exception "Unable to open file!" Unable to open log file.
    void truncate();

    std::ostringstream stringBuilder;

private:
    int level;
    std::ofstream file;
    std::string path;
};


/// @brief Converts a severity level string into LogLevel type.
/// @param level Severity level string to be converted.
/// @return Returns a LogLevel type of the equivalent severity, or INFO if invalid.
LogLevel LevelToI(std::string level);

extern int curY;
extern int curX;
extern Log logger;

#endif
