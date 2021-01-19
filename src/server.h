#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

// boost::asio::io_service boostIOService;
// tcp

class ChatConnection
    : public boost::enable_shared_from_this<ChatConnection>
{
public:
    typedef boost::shared_ptr<ChatConnection> pointer;

    static pointer create(boost::asio::io_service &io_service)
    {
        return pointer(new ChatConnection(io_service));
    }

    tcp::socket &socket()
    {
        return socket_;
    }

    void start();

private:
    ChatConnection(boost::asio::io_service &io_service)
        : socket_(io_service)
    {
    }

    void handleWrite(const boost::system::error_code& error,
      std::size_t );

    void handleRead(const boost::system::error_code& error,
      std::size_t );

    void startRead();

    tcp::socket socket_;
    boost::array<char , 1024> recvBuf = { 0 };
    boost::array<char, 1024> sendBuf = { 0 };
};

class ChatServer
{
public:
    ChatServer(boost::asio::io_service &io_service, int port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        startAccept();
    }

private:
    void startAccept();

    void handleAccept(ChatConnection::pointer new_connection,
                      const boost::system::error_code &error);

    tcp::acceptor acceptor_;
};

#endif