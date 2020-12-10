#include <boost/asio.hpp>
#include "network.h"

Server::Server()
{
    this->io_context = new boost::asio::io_context;
    this->tcp_acceptor = nullptr;
    this->client_socket = new boost::asio::ip::tcp::socket(*(this->io_context));;
}

Server::Server(short port)
{
    this->io_context = new boost::asio::io_context;
    this->tcp_acceptor = new boost::asio::ip::tcp::acceptor(*(this->io_context), tcp::endpoint(tcp::v4(), port));
    this->client_socket = new boost::asio::ip::tcp::socket(*(this->io_context));
}

int ValidData(Server *checkServer)
{
    if(checkServer -> io_context == nullptr || checkServer -> tcp_acceptor == nullptr || checkServer -> client_socket == nullptr)
        return 1;
    return 0;
}

void Server::Listen()
{
    if(ValidData(this))
        this->tcp_acceptor->accept(*(this->client_socket));
    else
        throw "Invalid network objects!";
}

void Server::Free()
{
    delete this->io_context;
    delete this->tcp_acceptor;
    delete this->client_socket;
}

void operator delete(void * p)
{
    static_cast<Server*>(p)->Free();
    free(p);
}