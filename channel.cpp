#include "channel.hpp"

Channel::Channel() {
    name = "";
}

Channel::Channel(std::string channel_name) {
    name = channel_name;
}
void Channel::addUser(User usr) {
    users.insert(std::pair<int, User>(usr.GetSocket(), usr));
}

std::string Channel::getName() {
    return (name);
}