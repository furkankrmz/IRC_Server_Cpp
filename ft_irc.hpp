#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <map>
#include <unordered_map> 
#include <exception>

#include "user.hpp"
#include "channel.hpp"

class Channel;

class ft_irc
{
private:
    int sock_fd;
    int port_number;
    std::string password;
    std::string operator_password;
    std::map<int, User> users;
    std::map<std::string, Channel> channels;

public:
    ft_irc();
    ~ft_irc();
    void SetPortNumber(int port_number);
    int GetPortNumber();
    void SetServerPassword(std::string password);
    std::string GetServerPassword();
    void SetSockFD(int sock_fd);
    int GetSockFD();
    void CreateSocket();
    void BindSocket(int sockfd, int portNumber);
    void StartListening(int sockfd);
    int AcceptConnection(int sockfd);
    void SendMessage(int sockfd, const char *message);
    User findUserBySocket(int sockfd);
    User findUserByNickname(std::string nickname);
    User findUserByUsername(std::string username);
    void removeUser(int sockfd);
    void Welcome(int sockfd);
    void HELP(int sockfd);
    void PASS(int sockfd, const std::vector<std::string>& args);
    void NICK(int sockfd, const std::vector<std::string>& args);
	void USER(int connection, const std::vector<std::string>& args);
    void OPER(int sockfd, const std::vector<std::string> &args);
    void LIST(int sockfd);
    void JOIN(int sockfd, const std::vector<std::string> &args);
    void KICK(int sockfd, const std::vector<std::string> &args);
    void PRIVMSG(int connection, const std::vector<std::string>& args);
    void NOTICE(int sockfd, const std::vector<std::string> &args);
};

std::vector<std::string> parse(std::string);