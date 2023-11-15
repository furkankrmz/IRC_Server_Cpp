#include "command_handler.hpp"

void commandHandler::initialize() {
    commandHandler::functions.insert(std::pair<std::string,command>("PASS", commandHandler::PASS));
    commandHandler::functions.insert(std::pair<std::string,command>("NICK", commandHandler::NICK));
    commandHandler::functions.insert(std::pair<std::string,command>("HELP", commandHandler::HELP));
    commandHandler::functions.insert(std::pair<std::string,command>("USER", commandHandler::USER));
}

void commandHandler::PASS(int sockfd, const std::vector<std::string>& args, ft_irc& irc) {
    if (args.size() != 2) {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: PASS <server password>\033[1;0m\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    irc.PASS(sockfd, args);
}

void commandHandler::NICK(int sockfd, const std::vector<std::string>& args, ft_irc& irc) {
    if (args.size() != 2) {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: NICK <nickname>\033[1;0m\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    irc.NICK(sockfd, args);
}

void commandHandler::HELP(int sockfd, const std::vector<std::string>& args, ft_irc& irc) {
    if (args.size() != 1) {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: HELP\033[1;0m\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    irc.HELP(sockfd);
}

void commandHandler::USER(int sockfd, const std::vector<std::string>& args, ft_irc& irc) {
    // if (args.size() != 1) {
    //     const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: HELP\033[1;0m\n";
    //     irc.SendMessage(sockfd, invalidMessage);
    //     return;
    // }
    irc.USER(sockfd, args);
}

void commandHandler::command_handler(int sockfd, const std::vector<std::string>& args, ft_irc& irc) {
    if (args.size() == 0)
      return;
    if (!args[0].compare("QUIT"))
      return;
    if (commandHandler::functions.find(args[0]) == commandHandler::functions.end())
      return;
    commandHandler::functions[args[0]](sockfd, args, irc);
}