#include "command_handler.hpp"

void commandHandler::initialize()
{
    commandHandler::functions.insert(std::pair<std::string, command>("PASS", commandHandler::PASS));
    commandHandler::functions.insert(std::pair<std::string, command>("NICK", commandHandler::NICK));
    commandHandler::functions.insert(std::pair<std::string, command>("HELP", commandHandler::HELP));
    commandHandler::functions.insert(std::pair<std::string, command>("USER", commandHandler::USER));
    commandHandler::functions.insert(std::pair<std::string, command>("OPER", commandHandler::OPER));
    commandHandler::functions.insert(std::pair<std::string, command>("LIST", commandHandler::LIST));
    commandHandler::functions.insert(std::pair<std::string, command>("JOIN", commandHandler::JOIN));
    commandHandler::functions.insert(std::pair<std::string, command>("KICK", commandHandler::KICK));
    commandHandler::functions.insert(std::pair<std::string, command>("NOTICE", commandHandler::NOTICE));
    commandHandler::functions.insert(std::pair<std::string, command>("PRIVMSG", commandHandler::PRIVMSG));
}

void commandHandler::PASS(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 2)
    {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: PASS <server password>\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (usr.GetPass())
            {
                const char *invalidMessage =
                    "\033[1;33mYou already authorized your password.\033[1;0m\r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.PASS(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::NICK(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 2)
    {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: NICK <nickname>\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!usr.GetPass())
            {
                const char *invalidMessage = "\033[1;31mPlease first authorize your password using PASS command! \033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.NICK(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::HELP(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 1)
    {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: HELP\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
        irc.HELP(sockfd);
}

void commandHandler::USER(int sockfd, const std::vector<std::string> &args, ft_irc &irc){
    if (args.size() != 5 || args[2] != "*" || args[3] != "*")
    {
        irc.SendMessage(sockfd, "\033[1;31mIncorrect use of command! Correct usage: USER <username> * * :<realname>\033[1;0m\r\n");
        return;
    }
    try
    {
        User usr = irc.findUserBySocket(sockfd);
        if (!(usr.GetPass() && usr.GetNick()))
        {
            const char *invalidMessage = "\033[1;31mPlease first use PASS and NICK commands!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
        else if (usr.GetUsr())
        {
            const char *invalidMessage = "\033[1;33mYou already set user.\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
        else
            irc.USER(sockfd, args);
    }
    catch (...)
    {
        const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
    }
}

void commandHandler::OPER(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 3)
    {
        const char *invalidMessage = "\033[1;31mIncorrect use of command! Correct usage: OPER <username> <password>\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!usr.IsAuthenticated())
            {
                const char *invalidMessage = "\033[1;31mPlease login to use chat. Use HELP command to learn how to login. \033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.OPER(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::LIST(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 1)
    {
        const char *invalidMessage =
            "\033[1;31mIncorrect use of command! Correct usage: LIST\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!usr.IsAuthenticated())
            {
                const char *invalidMessage = "\033[1;31mPlease login to use chat. Use HELP command to learn how to login. \033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.LIST(sockfd);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::JOIN(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() < 2)
    {
        const char *invalidMessage =
            "\033[1;31mIncorrect use of command! Correct usage: JOIN <channels>\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!usr.IsAuthenticated())
            {
                const char *invalidMessage = "\033[1;31mPlease login to use chat. Use HELP command to learn how to login. \033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.JOIN(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::KICK(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 3)
    {
        const char *invalidMessage =
            "\033[1;31mIncorrect use of command! Correct usage: KICK <channel> <client>\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!(usr.IsAuthenticated() && usr.GetOper()))
            {
                const char *invalidMessage = "\033[1;31mYou are not an operator!\033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.KICK(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::PRIVMSG(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() < 3)
    {
        const char *invalidMessage =
            "\033[1;31mIncorrect use of command! Correct usage: PRIVMSG <nickname> "
            "<message>\033[1;0m\r\n";
        
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!usr.IsAuthenticated())
            {
                const char *invalidMessage = "\033[1;31mPlease login to use chat. Use HELP command to learn how to login. \033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.PRIVMSG(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::NOTICE(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() != 3)
    {
        const char *invalidMessage =
            "\033[1;31mIncorrect use of command! Correct usage: NOTICE <target> <message>\033[1;0m\r\n";
        irc.SendMessage(sockfd, invalidMessage);
        return;
    }
    else
    {
        try
        {
            User usr = irc.findUserBySocket(sockfd);
            if (!(usr.IsAuthenticated() && usr.GetOper()))
            {
                const char *invalidMessage = "\033[1;31mYou are not an operator!\033[1;0m \r\n";
                irc.SendMessage(sockfd, invalidMessage);
            }
            else
                irc.NOTICE(sockfd, args);
        }
        catch (...)
        {
            const char *invalidMessage = "\033[1;33mUser not found!\033[1;0m\r\n";
            irc.SendMessage(sockfd, invalidMessage);
        }
    }
}

void commandHandler::command_handler(int sockfd, const std::vector<std::string> &args, ft_irc &irc)
{
    if (args.size() == 0)
        return;
    if (!args[0].compare("QUIT") || !args[0].compare("DIE"))
        return;
    if (commandHandler::functions.find(args[0]) == commandHandler::functions.end())
    {
        irc.SendMessage(sockfd, "\033[1;31mThis command is not defined!\033[1;0m\r\n");
        return;
    }
    commandHandler::functions[args[0]](sockfd, args, irc);
}