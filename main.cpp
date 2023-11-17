#include "ft_irc.hpp"
#include "command_handler.hpp"

int main(int ac, char *av[])
{
    if (ac != 3)
    {
        std::cout << "Usage: " << av[0] << " <port number> <password>\r\n";
        exit(EXIT_FAILURE);
    }

    ft_irc irc;
    commandHandler command;
    command.initialize();
    irc.SetPortNumber(atoi(av[1]));
    if (irc.GetPortNumber() < 1024 || irc.GetPortNumber() > 0xffff)
    {
        std::cerr << "Port Error!" << std::endl;
        return (EXIT_FAILURE);
    }
    irc.SetServerPassword(av[2]);
    irc.CreateSocket();
    irc.BindSocket(irc.GetSockFD(), irc.GetPortNumber());
    irc.StartListening(irc.GetSockFD());

    std::cout << "Listening on port " << irc.GetPortNumber() << std::endl;

    std::vector<int> clientSockets;
    fd_set readfds;
    FD_ZERO(&readfds);
    int temp = irc.GetSockFD();
    FD_SET(temp, &readfds);
    irc.SetSockFD(temp);
    int maxfd = irc.GetSockFD(); // Keep track of the highest socket descriptor

    while (true)
    {
        fd_set tmp_fds = readfds; // Create a temporary copy of the set
        int activity = select(maxfd + 1, &tmp_fds, NULL, NULL, NULL);

        if (activity < 0)
        {
            std::cout << "Error in select. errno: " << errno << std::endl;
            exit(1);
        }

        // Check for incoming connection on the main listening socket
        if (FD_ISSET(irc.GetSockFD(), &tmp_fds))
        {
            int connection = irc.AcceptConnection(irc.GetSockFD());
            // Send welcome message and prompt for password
            irc.Welcome(connection);
            FD_SET(connection, &readfds); // Add the new connection to the set
            if (connection > maxfd)
            {
                maxfd = connection; // Update maxfd if necessary
            }
        }

        // Check for data from existing client connections
        for (int fd = irc.GetSockFD() + 1; fd <= maxfd; ++fd)
        {
            if (FD_ISSET(fd, &tmp_fds))
            {
                char buffer[1000];
                int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    std::cout << "Socket " << fd << " hung up" << std::endl;
                    close(fd);
                    irc.removeUser(fd);
                    FD_CLR(fd, &readfds); // Remove the closed socket from the set
                }
                else
                {
                    buffer[bytesRead] = '\0';
                    int i = -1;
                    std::string line;
                    std::string character;
                    while (buffer[++i] != '\0') {
                        character = buffer[i];
                        line.append(character);
                    }
                    std::vector<std::string> args = parse(line);

                    command.command_handler(fd, args, irc);
                    if (strncmp(buffer, "QUIT", 4) == 0)
                    {
                        std::cout << "\033[1;33mClient on socket " << fd << " requested to quit.\033[1;0m" << std::endl;
                        close(fd);
                        irc.removeUser(fd);
                        FD_CLR(fd, &readfds); // Remove the closed socket from the set
                    }
                    if (strncmp(buffer, "DIE", 3) == 0) {
                        if (!irc.findUserBySocket(fd).GetOper())
                            irc.SendMessage(fd,"\033[1;31mYou are not authorized!\033[1;0m\r\n");
                        else{
                            for (int fd = irc.GetSockFD(); fd <= maxfd; ++fd)
                                if (FD_ISSET(fd, &readfds))
                                    close(fd);
                            close(irc.GetSockFD());
                            std::cout << "\033[1;31mShutting down!\033[1;0m\r\n";
                            irc.SendMessage(fd,"\033[1;31mShutting down!\033[1;0m\r\n");
                            return 0;
                            }
                    }
                }
            }
        }
    }

    // Close remaining sockets and clean up
    for (int fd = irc.GetSockFD(); fd <= maxfd; ++fd)
        if (FD_ISSET(fd, &readfds))
            close(fd);
    close(irc.GetSockFD());

    return 0;
}