//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "chatlog.h"
#include "commandline.h"
#include "statusline.h"
#include "log.h"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(boost::asio::io_context &io_context,
              const tcp::resolver::results_type &endpoints)
      : io_context_(io_context),
        socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const chat_message &msg)
  {
    boost::asio::post(io_context_,
                      [this, msg]() {
                        bool write_in_progress = !write_msgs_.empty();
                        write_msgs_.push_back(msg);
                        if (!write_in_progress)
                        {
                          do_write();
                        }
                      });
  }

  void close()
  {
    boost::asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type &endpoints)
  {
    boost::asio::async_connect(socket_, endpoints,
                               [this](boost::system::error_code ec, tcp::endpoint) {
                                 if (!ec)
                                 {
                                   quickPrintLog(INFO, "Connected.");
                                   statusLine->SetConnect(true);
                                   do_read_header();
                                 }
                               });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.data(), chat_message::header_length),
                            [this](boost::system::error_code ec, std::size_t /*length*/) {
                              if (!ec && read_msg_.decode_header())
                              {
                                do_read_body();
                              }
                              else
                              {
                                quickPrintLog(INFO, "Lost connection.");
                                statusLine->SetConnect(false);
                                socket_.close();
                              }
                            });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                            [this](boost::system::error_code ec, std::size_t /*length*/) {
                              if (!ec)
                              {
                                if (read_msg_.body_length() > 0)
                                {

                                  char header[28];
                                  strncpy(header, read_msg_.data(), 28);

                                  std::string headerStr;
                                  headerStr = header;

                                  std::string username;
                                  username = headerStr.substr(7, 16);
                                  username.erase(std::remove_if(username.begin(), username.end(), ::isspace), username.end());

                                  std::string strMsg = read_msg_.body();
                                  strMsg.erase(strMsg.begin() + read_msg_.body_length(), strMsg.end());

                                  ncOutUsr("[" << username << "]: " << strMsg);
                                  quickLog(VERBOSE, "user: " << username << " message: " << strMsg);
                                }
                                do_read_header();
                              }
                              else
                              {
                                quickPrintLog(INFO, "Lost connection.");
                                statusLine->SetConnect(false);
                                socket_.close();
                              }
                            });
  }

  void do_write()
  {
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(),
                                                 write_msgs_.front().length()),
                             [this](boost::system::error_code ec, std::size_t /*length*/) {
                               if (!ec)
                               {
                                 write_msgs_.pop_front();
                                 if (!write_msgs_.empty())
                                 {
                                   do_write();
                                 }
                               }
                               else
                               {
                                 quickPrintLog(INFO, "Lost connection.");
                                 statusLine->SetConnect(false);
                                 socket_.close();
                               }
                             });
  }

private:
  boost::asio::io_context &io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

void StartChatClient(std::string host, std::string port)
{
  try
  {
    quickPrintLog(INFO, "Starting client...");
    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);
    chat_client c(io_context, endpoints);

    std::thread t([&io_context]() { io_context.run(); });

    char line[chat_message::max_body_length + 1];
    std::string user_input;
    while ((user_input = GetConsoleInput(false)) != "/leave")
    {
      std::fill_n(line, chat_message::max_body_length, 0);
      std::memcpy(line, user_input.c_str(), (user_input.length() < chat_message::max_body_length) ? user_input.length() : chat_message::max_body_length);
      
      chat_message msg;
      msg.body_length(user_input.length());
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }

    quickPrintLog(INFO, "Leaving server....");
    c.close();
    t.join();
  }
  catch (std::exception &e)
  {
    exceptionLog(ERROR, e.what());
  }
}