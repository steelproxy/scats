#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server
{
public:
    Server(short port);

    void Listen();
    void AcceptConnection();

    void Write();
    void Read();
private:
    friend int ValidData(Server *checkServer);

    boost::asio::io_context* io_context;  // io_context, used for communication between OS I/O services
    boost::asio::ip::tcp::socket* client_socket; // TCP socket, used as I/O object, forwards requests to io_context
    boost::asio::ip::tcp::acceptor* tcp_acceptor; // used to listen for new connections
};

#endif