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

std::string Channel::getName() {
    return (name);
}