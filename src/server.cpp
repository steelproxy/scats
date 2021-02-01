#include <boost/asio.hpp>
#include "server.h"
#include "cursesmode.h"
#include "log.h"

using namespace std;

void ChatServer::startAccept()
{
    ChatConnection::pointer new_connection = ChatConnection::create(acceptor_.get_executor().context());

    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&ChatServer::handleAccept, this, new_connection,
                                       boost::asio::placeholders::error));
}

void ChatServer::handleAccept(ChatConnection::pointer new_connection,
                              const boost::system::error_code &error)
{
    quickPrintLog(VERBOSE, "")
    if (!error)
    {
        new_connection->start();
        startAccept();
    }
}

void ChatConnection::start()
{
    /*boost::asio::async_write(socket_, boost::asio::buffer(recvBuf),
                             boost::bind(&ChatConnection::handleWrite, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));*/
    startRead();
}

void ChatConnection::handleRead(const boost::system::error_code &error,
                                std::size_t)
{
    if (!error || error == boost::asio::error::message_size)
    {
        //socket_.async_send(boost::asio::buffer(recvBuf), boost::bind(&ChatConnection::handleWrite, this, recvBuf,
        //                                  boost::asio::placeholders::error,
        //                                  boost::asio::placeholders::bytes_transferred));

        ncoutln("Recieved " << boost::asio::placeholders::bytes_transferred << " characters:");

        for(size_t byteBuf = 0; byteBuf < recvBuf.size(); byteBuf++)
        {
            ncout(recvBuf.at(byteBuf));
        }


        startRead();
    }
}

void ChatConnection::startRead()
{
    socket_.async_receive(boost::asio::buffer(recvBuf), boost::bind(&ChatConnection::handleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}