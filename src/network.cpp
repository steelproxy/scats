/** 
 *  @file   network.cpp
 *  @brief  Implementation of Server class.
 *  @author Collin Rodes
 *  @date   2020-12-11
 ***********************************************/

#include <boost/asio.hpp>
#include "network.h"

Server::Server(short port)
{
    this->io_context = new boost::asio::io_context;
    this->tcp_acceptor = new boost::asio::ip::tcp::acceptor(*(this->io_context), tcp::endpoint(tcp::v4(), port));
    this->client_socket = new boost::asio::ip::tcp::socket(*(this->io_context));
}

void Server::Listen()
{
    this->tcp_acceptor->accept(*(this->client_socket));
}
