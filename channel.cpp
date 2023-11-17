#include "channel.hpp"

Channel::Channel() {
    name = "";
}

Channel::Channel(std::string channel_name) {
    name = channel_name;
}

void Channel::addUser(User usr) {
    if (users.find(usr.GetSocket()) == users.end())
        users.insert(std::pair<int, User>(usr.GetSocket(), usr));
    else
        throw ("User already in channel!");
}

void Channel::kickUser(User usr) {
    if (users.find(usr.GetSocket()) != users.end())
        users.erase(usr.GetSocket());
    else
        throw ("User is not channel!");
}

std::string Channel::getName() {
    return (name);
}