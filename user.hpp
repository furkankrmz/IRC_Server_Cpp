#pragma once
#include "ft_irc.hpp"

class User
{
private:
    int sock_fd;
    bool is_operator;
    bool pass;
    bool nick;
    bool usr;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string uncomplatedcommand;
public:
    User();
    User(int fd);
    void clearCommand();
    void AppendCommand(std::string command);
    void SetUsername(std::string name);
    std::string GetUsername();
    void SetNickname(std::string name);
    std::string GetNickname();
    void SetRealname(std::string r_name);
    std::string GetRealname();
    std::string GetPrefix();
    std::string GetCommand();
    bool IsAuthenticated();
    int GetSocket();
    void SetPass(bool);
    bool GetPass();
    void SetNick(bool);
    bool GetNick();
    void SetUsr(bool);
    bool GetUsr();
    void SetOper(bool);
    bool GetOper();
};