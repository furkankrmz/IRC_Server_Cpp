#pragma once
#include "ft_irc.hpp"

class Channel
{
private:
    std::string name;
    std::map<int, User> users;
public:
    Channel();
    Channel(std::string name);
    void addUser(User);
    void kickUser(User);
    std::string getName();
    void SendNotice(std::string);
    void SendToChannelUsers(std::string sender, std::string message);
};