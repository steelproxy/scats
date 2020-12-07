#ifndef CONTACT_H
#define CONTACT_H

#include <string>

class Contact
{
public:
    Contact(); // default constructor
    Contact(std::string info);
    Contact(std::string newAlias, std::string newHostname, int newPort);

    void setAlias(std::string newAlias);
    void setHostname(std::string newHostname);
    void setPort(int newPort);

    std::string getAlias();
    std::string getHostname();
    int getPort();

    std::string toString();

private:
    std::string alias;
    std::string hostname;
    int port;
};

#endif