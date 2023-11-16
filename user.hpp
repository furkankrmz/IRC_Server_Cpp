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
public:
    User();
    User(int fd);
    void SetUsername(std::string name);
    std::string GetUsername();
    void SetNickname(std::string name);
    std::string GetNickname();
    void SetRealname(std::string r_name);
    std::string GetRealname();
    bool IsAuthenticated();
    void SetPass(bool);
    bool GetPass();
    void SetNick(bool);
    bool GetNick();
    void SetUsr(bool);
    bool GetUsr();
    void SetOper(bool);
    bool GetOper();
};