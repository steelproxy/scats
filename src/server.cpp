#include <string>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "server.h"
#include "cursesmode.h"
#include "commandline.h"
#include "chatlog.h"
#include "log.h"
#include "setting.h"

#define DELIM "\n"

using namespace std;

using boost::asio::ip::tcp;

string makeAFuckingString(boost::asio::streambuf &_streambuf)
{
    return {boost::asio::buffers_begin(_streambuf.data()),
            boost::asio::buffers_end(_streambuf.data())};
}

void StartServer(short port)
{
    ncOutCmd("wait...");
    try
    {
        boost::asio::io_service _io_service;
        tcp::acceptor _acceptor(_io_service, tcp::endpoint(tcp::v4(), port));

        tcp::socket _socket(_io_service);
        _acceptor.accept(_socket);
    //    quickPrintLog(INFO, "Got connection!");

        for (;;)
        {
            boost::asio::streambuf buffer;
            string _recievebuf;
            size_t bytes_read;
            bytes_read = boost::asio::read_until(_socket, buffer, DELIM);
            ncOutUsr("yes i can");
            buffer.consume(bytes_read / sizeof(char));

            string response;
            ncOutCmd("Send: ");
            response = GetConsoleInput(false);
            response += DELIM;
            boost::system::error_code _error;
            boost::asio::write(_socket, boost::asio::buffer(response), _error);
            ncOutUsr("can you hear me?");
            ncOutCmd("wait...");
        }
    }
    catch (std::exception &e)
    {
        ncOutUsr("Lost connection!");
        exceptionLog(ERROR, e.what());
    }
}

void ConnectServer(short port)
{
    try
    {
        boost::asio::io_service _io_service;
        tcp::socket _socket(_io_service);

        _socket.connect(tcp::endpoint(tcp::v4(), port));
//        quickPrintLog(INFO, "Connected!");

        for (;;)
        {
            boost::system::error_code _error;

            ncOutCmd("Send: ");
            string sendbuf;
            sendbuf = GetConsoleInput(false);
            sendbuf += DELIM;
            boost::asio::write(_socket, boost::asio::buffer(sendbuf), _error);
            ncOutUsr("can you hear me?");
            ncOutCmd("wait...");

            boost::asio::streambuf buffer;
            string _recievebuf;
            size_t bytes_read;
            bytes_read = boost::asio::read_until(_socket, buffer, DELIM);
            chatLog -> Print("yes heard you");
            buffer.consume(bytes_read / sizeof(char));
        }
    }
    catch(std::exception &e)
    {
        ncOutUsr("Lost connection!");
        exceptionLog(ERROR, e.what());
    }
}

bool connectedToServer()
{
    return true;
}

void SendChat(string chat)
{
    return;
}