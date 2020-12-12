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

#define ncout(message)                                    \
    {                                                     \
        getyx(root, curY, curX);                          \
        if (curY == getmaxy(root))                        \
            scroll(root);                                 \
        logger.stringBuilder.str(string());               \
        logger.stringBuilder << message;                  \
        printw("%s", logger.stringBuilder.str().c_str()); \
        refresh();                                        \
    }

#define ncoutln(message)        \
    {                           \
        ncout(message << endl); \
    }

#define ngetstr(var)                               \
    {                                              \
        var = getch();                             \
        refresh();                                 \
        while (var.at(var.length() - 1) != '\n')   \
        {                                          \
            if (isprint(var.at(var.length() - 1))) \
                addch(var.at(var.length() - 1));   \
            refresh();                             \
            var += getch();                        \
        }                                          \
        var.erase(var.end() - 1);                  \
        addch('\n');                               \
    }
typedef enum
{
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

    void SetPrint(bool newPrint);
    int Truncate();

    std::ostringstream stringBuilder;

private:
    std::ofstream file;
    std::string path;
    bool print;
};

extern WINDOW *root;
extern int curY;
extern int curX;

#endif
