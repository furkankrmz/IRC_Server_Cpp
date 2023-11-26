#include "channel.hpp"

Channel::Channel()
{
    name = "";
}

Channel::Channel(std::string channel_name)
{
    if (channel_name[0] != '&' && channel_name[0] != '#' && channel_name[0] != '+' && channel_name[0] != '!')
        throw("\033[1;31mChannel name is invalid!\033[1;0m\r\n");
    name = channel_name;
}

void Channel::addUser(User usr)
{
    if (users.find(usr.GetSocket()) == users.end())
        users.insert(std::pair<int, User>(usr.GetSocket(), usr));
    else
        throw("User already in channel!");
}

void Channel::kickUser(User usr)
{
    if (users.find(usr.GetSocket()) != users.end())
        users.erase(usr.GetSocket());
    else
        throw("User is not channel!");
}

std::string Channel::getName()
{
    return (name);
}

void Channel::SendNotice(std::string message)
{
    std::map<int, User>::iterator it = users.begin();
    while (it != users.end())
    {
        send(it->first, message.c_str(), strlen(message.c_str()), 0);
        it++;
    }
}
void Channel::SendToChannelUsers(std::string sender, std::string message)
{
    std::map<int, User>::iterator it = users.begin();
    while (it != users.end())
    {
        if (it->second.GetPrefix() == sender) {
            it++;
            continue;
        }
        std::string reply = sender + " PRIVMSG " + name + " :" + message + "\n";
        send(it->first, reply.c_str(), reply.size(), 0);
        it++;
    }
}