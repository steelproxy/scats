#ifndef LOG_H
#define LOG_H

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
        cout << message << endl;      \
        quickLog(level, message)      \
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

#endif
