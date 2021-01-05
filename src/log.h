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

#define quickLog(level, message)                             \
    {                                                        \
        logger.stringBuilder.str(string());                  \
        logger.stringBuilder << message;                     \
        logger.WriteLine(level, logger.stringBuilder.str()); \
    }

#define quickPrintLog(level, message) \
    {                                 \
        ncoutln(message);             \
        quickLog(level, message);     \
    }

typedef enum
{
    VERBOSE,
    INFO,
    WARNING,
    ERROR,
    SEVERE
} logLevel;

class Log
{
public:
    Log();
    Log(std::string newPath);

    int Open(std::string newPath);
    void Close();

    int WriteLine(logLevel level, std::string message);
    void SetLevel(int logLevel);

    int Truncate();

    std::ostringstream stringBuilder;

private:
    int level;
    std::ofstream file;
    std::string path;
};

extern WINDOW *root;
extern int curY;
extern int curX;

#endif
