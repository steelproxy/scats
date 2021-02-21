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

void StartServer(short port);
void ConnectServer(short port);
bool connectedToServer();
void SendChat(std::string chat);

#endif