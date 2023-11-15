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

typedef void (*command)(int, const std::vector<std::string>&);

class ft_irc
{
private:
    int port_number;
    std::string password;
    int sock_fd;

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
    void Welcome(int sockfd, std::string password);
    void HELP(int sockfd, const std::vector<std::string>& args);
    void PASS(int sockfd, const std::vector<std::string>& args);
    void NICK(int sockfd, const std::vector<std::string>& args);
	void USER();
};

std::vector<std::string> parse(std::string);