//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog.h"
#include "../ui/commandline.h"
#include "chat_message.hpp"
#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
  public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message &msg) = 0;
    std::string username;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
  public:
    void join(chat_participant_ptr participant)
    {
        quickPrintLog(INFO, "Joining room....");
        participants_.insert(participant);
        for (auto msg : recent_msgs_)
            participant->deliver(msg);
    }

    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
    }

    void deliver(const chat_message &msg)
    {
        if (msg.body_length() > 0)
        {

            char header[28];
            strncpy(header, msg.data(), 28);

            std::string headerStr;
            headerStr = header;

            std::string username;
            username = headerStr.substr(7, 16);
            username.erase(
                std::remove_if(username.begin(), username.end(), ::isspace),
                username.end());

            std::string strMsg = msg.body();
            strMsg.erase(strMsg.begin() + msg.body_length(), strMsg.end());

            attron(A_BOLD);
            ncOutUsr("[" << username << "]: " << strMsg);
            attroff(A_BOLD);
            quickLog(VERBOSE, "user: " << username << " message: " << strMsg);
        }

        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        for (auto participant : participants_)
            participant->deliver(msg);
    }

  private:
    std::set<chat_participant_ptr> participants_;
    enum
    {
        max_recent_msgs = (1)
    };
    chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session : public chat_participant,
                     public std::enable_shared_from_this<chat_session>
{
  public:
    chat_session(boost::asio::ip::tcp::socket socket, chat_room &room)
        : socket_(std::move(socket)), room_(room)
    {
    }

    void start()
    {
        room_.join(shared_from_this());
        do_read_header();
    }

    void deliver(const chat_message &msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            do_write();
        }
    }

  private:
    void do_read_header()
    {
        auto self(shared_from_this());
        boost::asio::async_read(
            socket_,
            boost::asio::buffer(read_msg_.data(), chat_message::header_length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec && read_msg_.decode_header())
                {
                    do_read_body();
                }
                else
                {
                    room_.leave(shared_from_this());
                }
            });
    }

    void do_read_body()
    {
        auto self(shared_from_this());
        boost::asio::async_read(
            socket_,
            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    room_.deliver(read_msg_);
                    do_read_header();
                }
                else
                {
                    room_.leave(shared_from_this());
                }
            });
    }

    void do_write()
    {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(write_msgs_.front().data(),
                                write_msgs_.front().length()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
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
                    room_.leave(shared_from_this());
                }
            });
    }

    boost::asio::ip::tcp::socket socket_;
    chat_room &room_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
  public:
    chat_server(boost::asio::io_context &io_context,
                const boost::asio::ip::tcp::endpoint &endpoint)
        : acceptor_(io_context, endpoint)
    {
        quickPrintLog(INFO, "Starting server...");
        do_accept();
    }

  private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec,
                   boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    quickPrintLog(INFO, "Accepting connection....");
                    std::make_shared<chat_session>(std::move(socket), room_)
                        ->start();
                }

                do_accept();
            });
    }

    boost::asio::ip::tcp::acceptor acceptor_;
    chat_room room_;
};

//----------------------------------------------------------------------

void StartChatServer(std::string port)
{
    try
    {
        boost::asio::io_context io_context;

        std::list<chat_server> servers;
        for (int i = 0; i < 1; i++)
        {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),
                                                    std::atoi(port.c_str()));
            servers.emplace_back(io_context, endpoint);
        }

        commandLine->Clear();
        ncOutCmd("Server running...");
        io_context.run();
    }
    catch (std::exception &e)
    {
        exceptionLog(ERROR, e.what());
    }
}