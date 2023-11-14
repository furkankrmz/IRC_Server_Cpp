#include "ft_irc.hpp"

ft_irc::ft_irc(/* args */)
{
  int port_number = 0;
  std::string password = "";
  int sock_fd = -1;
};
ft_irc::~ft_irc(){};

void ft_irc::SetPortNumber(int port_number)
{
  this->port_number = port_number;
};
int ft_irc::GetPortNumber()
{
  return this->port_number;
};
void ft_irc::SetServerPassword(std::string password)
{
  this->password = password;
};
std::string ft_irc::GetServerPassword()
{
  return this->password;
};
void ft_irc::SetSockFD(int sock_fd)
{
  this->sock_fd = sock_fd;
};
int ft_irc::GetSockFD()
{
  return this->sock_fd;
};

// Function to create a socket
void ft_irc::CreateSocket()
{
  this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock_fd == -1)
  {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
};

// Function to bind the socket to a port
void ft_irc::BindSocket(int sockfd, int portNumber)
{
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(portNumber);

  if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
  {
    std::cout << "Failed to bind to port " << portNumber << " errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Function to listen for connections
void ft_irc::StartListening(int sockfd)
{
  if (listen(sockfd, 100) < 0)
  {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Function to accept a connection
int ft_irc::AcceptConnection(int sockfd)
{
  socklen_t addrlen;
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;

  int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
  if (connection < 0)
  {
    std::cout << "Failed to grab connection. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  // Print the port number on which the connection was established
  std::cout << "New connection from " << inet_ntoa(sockaddr.sin_addr) << " on socket " << connection << std::endl;
  return connection;
}

// Function to set the nickname
void ft_irc::ChangeNickname(int connection, const std::string &newNickname, std::unordered_set<std::string> &usedNicknames)
{
  if (usedNicknames.find(newNickname) != usedNicknames.end())
  {
    const char *message = "Nickname is already in use. Please choose a different one.\n";
    send(connection, message, strlen(message), 0);
  }
  else
  {
    const char *successMessage = "Nickname changed successfully!\n";
    send(connection, successMessage, strlen(successMessage), 0);
    usedNicknames.erase(newNickname);
    usedNicknames.insert(newNickname);
  }
}

void ft_irc::SendMessage(int sockfd, const char *message)
{
  send(sockfd, message, strlen(message), 0);
}

// Function to authenticate password
void ft_irc::AuthenticateClient(int clientSocket, std::string password)
{
  const char *promptMessage = "Please enter your password: \n";
  SendMessage(clientSocket, promptMessage);

  char buffer[8];
  int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0)
  {
    // Connection closed or error occurred
    std::cout << "Socket " << clientSocket << " hung up" << std::endl;
    close(clientSocket);
    return;
  }
  buffer[bytesRead] = '\0';

  int attempts = 3; // Number of password entry attempts
  std::cout << buffer << std::endl;
  std::string clientPassword = (std::string)buffer;
  std::cout << clientPassword << std::endl;
  while (clientPassword != password && attempts > 0)
  {
    const char *invalidMessage = "Invalid password! Please try again.\n";
    SendMessage(clientSocket, invalidMessage);

    --attempts;

    // Prompt for password again
    SendMessage(clientSocket, promptMessage);
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0)
    {
      // Connection closed or error occurred
      std::cout << "Socket " << clientSocket << " hung up" << std::endl;
      close(clientSocket);
      return;
    }
    buffer[bytesRead] = '\0';
    clientPassword = atoi(buffer);
  }

  if (clientPassword != password)
  {
    // All attempts exhausted, disconnect the client
    const char *disconnectMessage = "Max password attempts reached. Disconnecting.\n";
    SendMessage(clientSocket, disconnectMessage);
    close(clientSocket);
    return;
  }

  // Password is valid, so the client is authenticated
  const char *successMessage = "Client authenticated!\n";
  SendMessage(clientSocket, successMessage);
  std::cout << "Client on socket " << clientSocket << " authenticated!\n";
}