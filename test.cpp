#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

// Function to create a socket
int createSocket() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

// Function to bind the socket to a port
void bindSocket(int sockfd, int portNumber) {
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(portNumber);

  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cout << "Failed to bind to port " << portNumber << " errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Function to listen for connections
void startListening(int sockfd) {
  if (listen(sockfd, 100) < 0) {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Function to accept a connection
int acceptConnection(int sockfd) {
  socklen_t addrlen;
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;


  int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
  if (connection < 0) {
    std::cout << "Failed to grab connection. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  // Print the port number on which the connection was established
  std::cout << "New connection from " << inet_ntoa(sockaddr.sin_addr) << " on socket " << connection << std::endl;
  return connection;
}

// Function to change the nickname
void changeNickname(int connection, const std::string& newNickname, std::unordered_set<std::string>& usedNicknames) {
    if (usedNicknames.find(newNickname) != usedNicknames.end()) {
        const char *message = "Nickname is already in use. Please choose a different one.\n";
        send(connection, message, strlen(message), 0);
    } else {
        const char *successMessage = "Nickname changed successfully!\n";
        send(connection, successMessage, strlen(successMessage), 0);
        usedNicknames.erase(newNickname); // Remove old nickname from usedNicknames
        usedNicknames.insert(newNickname); // Add new nickname to usedNicknames
    }
}

void sendMessage(int sockfd, const char* message) {
    send(sockfd, message, strlen(message), 0);
}

// Function to authenticateClient
void authenticateClient(int clientSocket, int password) {
  const char* promptMessage = "Please enter your password: \n";
  sendMessage(clientSocket, promptMessage);

  char buffer[100];
  int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead <= 0) {
    // Connection closed or error occurred
    std::cout << "Socket " << clientSocket << " hung up" << std::endl;
    close(clientSocket);
    return;
  }
  buffer[bytesRead] = '\0';

  int attempts = 3; // Number of password entry attempts
  int clientPassword = atoi(buffer);
  while (clientPassword != password && attempts > 0) {
    const char* invalidMessage = "Invalid password! Please try again.\n";
    sendMessage(clientSocket, invalidMessage);

    --attempts;
    
    // Prompt for password again
    sendMessage(clientSocket, promptMessage);
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
      // Connection closed or error occurred
      std::cout << "Socket " << clientSocket << " hung up" << std::endl;
      close(clientSocket);
      return;
    }
    buffer[bytesRead] = '\0';
    clientPassword = atoi(buffer);
  }

  if (clientPassword != password) {
    // All attempts exhausted, disconnect the client
    const char* disconnectMessage = "Max password attempts reached. Disconnecting.\n";
    sendMessage(clientSocket, disconnectMessage);
    close(clientSocket);
    return;
  }

  // Password is valid, so the client is authenticated
  const char* successMessage = "Client authenticated!\n";
  sendMessage(clientSocket, successMessage);
  std::cout << "Client on socket " << clientSocket << " authenticated!\n";

  // Unblock the client so it can now send messages
  int flags = fcntl(clientSocket, F_GETFL, 0);
  flags &= ~O_NONBLOCK;
  fcntl(clientSocket, F_SETFL, flags);
}


int main(int ac, char *av[]) {
  if (ac != 3) {
    std::cout << "Usage: " << av[0] << " <port number> <password>\n";
    exit(EXIT_FAILURE);
  }

  int portNumber = atoi(av[1]);
  int password = atoi(av[2]);

  int sockfd = createSocket();
  bindSocket(sockfd, portNumber);
  startListening(sockfd);

  std::cout << "Listening on port " << portNumber << std::endl;
  std::unordered_set<std::string> usedNicknames;


  std::vector<int> clientSockets; // Keep track of connected client sockets
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(sockfd, &readfds);

  int maxfd = sockfd; // Keep track of the highest socket descriptor

  while (true) {
    fd_set tmp_fds = readfds; // Create a temporary copy of the set
    int activity = select(maxfd + 1, &tmp_fds, NULL, NULL, NULL);

    if (activity < 0) {
      std::cout << "Error in select. errno: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }

    // Check for incoming connection on the main listening socket
    if (FD_ISSET(sockfd, &tmp_fds)) {
      int connection = acceptConnection(sockfd);
      // Send welcome message and prompt for password
      const char* welcomeMessage = "Welcome to the server! Please enter your password: \n";
      sendMessage(connection, welcomeMessage);
      authenticateClient(connection, password);
      FD_SET(connection, &readfds); // Add the new connection to the set
      if (connection > maxfd) {
        maxfd = connection; // Update maxfd if necessary
      }
    }

    // Check for data from existing client connections
    for (int fd = sockfd + 1; fd <= maxfd; ++fd) {
      if (FD_ISSET(fd, &tmp_fds)) {
        char buffer[100];
        int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
          // Connection closed or error occurred
          std::cout << "Socket " << fd << " hung up" << std::endl;
          close(fd);
          FD_CLR(fd, &readfds); // Remove the closed socket from the set
        } else {
          buffer[bytesRead] = '\0';
          
          if (strncmp(buffer, "NICK ", 5) == 0) {
            std::string newNickname = buffer + 5;
            changeNickname(fd, newNickname, usedNicknames);
          } else if (strncmp(buffer, "QUIT", 4) == 0) {
              std::cout << "Client on socket " << fd << " requested to quit." << std::endl;
              close(fd);
              FD_CLR(fd, &readfds); // Remove the closed socket from the set
          }else {
            std::cout << "Client["<< fd <<"]: " << buffer << std::endl;
          }
        }
      }
    }
  }

  // Close remaining sockets and clean up
  for (int fd = sockfd; fd <= maxfd; ++fd) {
    if (FD_ISSET(fd, &readfds)) {
      close(fd);
    }
  }
  close(sockfd);

  return 0;
}