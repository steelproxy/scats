#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>

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
