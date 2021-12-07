/**
 *  @file   scats.cpp
 *  @brief  Main program file.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include "./commands/commands.h"
#include "./log/log.h"
#include "./setting/setting.h"
#include "./ui/chatlog/chatlog.h"
#include "./ui/commandline/commandline.h"
#include "./ui/isprint.h"
#include "./ui/statusline/statusline.h"
#include <cstdlib>
#include <curses.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <vector>

WINDOW *wRoot;
WINDOW *wStatusLine;

Log logger("log.txt");

// for messages printed before chatlog is initialized
std::vector<std::string> preUser;

ChatLog *chatLog;
CommandLine *commandLine;
StatusLine *statusLine;
HotkeyManager *hotkeyMan;

sigjmp_buf sigintJumpPoint;

void SignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        quickPrintLog(INFO, "Sigint caught.");
        siglongjmp(sigintJumpPoint, 1);
    }
}

void Cleanup()
{
    endwin();       // end curses mode
    logger.close(); // close log file
}

int main(int argc, char **argv)
{
    // register cleanup function
    atexit(Cleanup);

    // start curses mode
    initscr(); // start curses mode
    cbreak();  // don't require newline
    noecho();  // don't echo commands, we'll control that
    curs_set(0);

    // create log file if necessary
    if (!FileExists(DEFAULT_LOG_FILE))
    {
        std::ofstream logFile;
        preUser.push_back("Log file does not exist!");
        preUser.push_back("Creating log file...");
        logFile.open(DEFAULT_LOG_FILE, std::ios::out);
        if (logFile.fail())
        {
            preUser.push_back("Unable to create log file!");
        }
        logFile.close();
    }

    // create settings file if necessary
    bool newSettingFile = false;
    if (!FileExists(DEFAULT_SETTINGS_FILE))
    {
        std::ofstream settingFile;
        newSettingFile = true;
        preUserPrint(WARNING, "Setting database does not exist!");
        preUserPrint(INFO, "Creating setting database...");
        settingFile.open(DEFAULT_SETTINGS_FILE, std::ios::out);
        if (settingFile.fail())
        {
            preUserPrint(ERROR, "Unable to create setting database!");
        }
        else
        {
            newSettingFile = true;
        }
        settingFile.close();
    }

    // load settings
    try
    {
        LoadSettings();
    }
    catch (const char *msg)
    {
        exceptionLog(ERROR, msg);
    }

    // apply settings
    if (_iniStructure.get("General").get("userHandle").empty())
    {
        InteractiveSetUserHandle();
    }
    if (newSettingFile)
    {
        try
        {
            SaveSettings();
        }
        catch (const char *msg)
        {
            preUserPrint(ERROR, "Unable to save settings database!");
            exceptionLog(ERROR, msg);
        }
    }

    // setup status lie
    statusLine = new StatusLine();

    // setup chat log
    chatLog = new ChatLog();

    // setup hotkey manager
    hotkeyMan = new HotkeyManager();

    // setup command line
    commandLine = new CommandLine();

    // print pre log que
    for (auto message : preUser)
    {
        ncOutUsr(message);
    }

    // start scats
    quickPrintLog(INFO, "Starting scats...");

    // interactive loop
    for (;;)
    {
        while (sigsetjmp(sigintJumpPoint, 1) != 0)
            ; // set jump to beginning of loop

        // capture SIGINT's
        signal(SIGINT, SignalHandler);

        // user prompt
        std::string userInput;
        userInput = std::string(); // clear std::string
        ncOutCmd("[" << _iniStructure["General"]["userHandle"].c_str()
                     << "]: ");
        commandLine->PrintPrompt();
        userInput = GetConsoleInput(true);

        // extract command substring
        std::string commandSubStr;
        if (userInput.empty() ||
            userInput.at(0) != '/') // check if input is a command
        {
            if (/*connectedToServer()*/ !userInput.empty())
            {
                /*SendChat(userInput);*/
                ncOutUsr("[" << _iniStructure["General"]["userHandle"].c_str()
                             << "]: " << userInput);
            }
            else
            {
                quickPrintLog(ERROR, "Not connected to chat!");
            }
            continue;
        }
        commandSubStr = userInput.substr(1); // extract command after '/'
        if (commandSubStr == "build")        // rebuild scats
        {
            quickPrintLog(INFO,
                          "Exiting, rebuilding, and relaunching scats...");
            system("cd ..; make; cd bin; ./scats");
            return 0;
        }
    }
}
