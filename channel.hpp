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
    std::string getName();
};