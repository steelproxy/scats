//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include "../log/log.h"
#include "../setting/setting.h"
#include "../ui/chatlog/chatlog.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <uuid/uuid.h>

class chat_message
{
  public:
    enum { header_length = 28 + 16 + 2 };
    enum { max_body_length = 512 };

    chat_message() : body_length_(0) {}

    const char *data() const { return data_; }

    char       *data() { return data_; }

    std::size_t length() const { return header_length + body_length_; }

    const char *body() const { return data_ + header_length; }

    char       *body() { return data_ + header_length; }

    std::size_t body_length() const { return body_length_; }

    void        body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

    bool decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        std::string headerStr = header;
        quickLog(VERBOSE, "Header: " << header);

        if (headerStr.find("scats-[", 0) != 0 || headerStr.at(23) != ']') {
            return false;
        }

        std::string bodyLenSubStr = headerStr.substr(24);
        body_length_              = std::atoi(bodyLenSubStr.c_str());
        quickLog(VERBOSE, "Body length: " << body_length_);
        if (body_length_ > max_body_length) {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header()
    {
        char header[header_length + 1] = "";

        // format header with username
        std::sprintf(header, "scats-[%16s][%16s]%4d",
                     _iniStructure["General"]["TempUUID"].c_str(),
                     _iniStructure["General"]["UserHandle"].c_str(),
                     static_cast<int>(body_length_));
        std::memcpy(data_, header, header_length);
    }

  private:
    char        data_[header_length + max_body_length];
    std::size_t body_length_;
};

void StartChatClient(std::string, std::string);
void StartChatServer(std::string);

#endif // CHAT_MESSAGE_HPP