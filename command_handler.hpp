#pragma once
#include "ft_irc.hpp"

typedef void (*command)(int, const std::vector<std::string>&, ft_irc&);

class commandHandler {
        static void PASS(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void NICK(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void HELP(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void USER(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void OPER(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void LIST(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void JOIN(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void KICK(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
        static void PRIVMSG(int sockfd, const std::vector<std::string>& args,ft_irc&irc);
        std::map<std::string, command> functions;
    public:
        void initialize();
        void command_handler(int sockfd, const std::vector<std::string>& args, ft_irc& irc);
};