#include <sys/socket.h> // for socket func.
#include <netinet/in.h> // for sockaddr_in
#include <cstdlib>     // for exit and EXIT_FAILURE
#include <iostream>    // for cout
#include <unistd.h>    // for read

int main(int ac, char *av[]) {
  if (ac != 3) {
    std::cout << "Usage: " << av[0] << "<port number> <password>\n";
    exit(EXIT_FAILURE);
  }

  int portNumber = atoi(av[1]);
  int password = atoi(av[2]);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(portNumber);

  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cout << "Failed to bind to port " << portNumber << " errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 100) < 0) {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Listening on port " << portNumber << std::endl;

  while (true) {
    auto addrlen = sizeof(sockaddr);
    int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
      std::cout << "Failed to grab connection. errno: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }

    char buffer[100];
    int bytesRead = 0;

    std::cout << "Welcome to the server! Please enter your password: \n";

    bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == 0) {
      break;
    } else if (bytesRead == -1) {
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      } else {
        std::cout << "Error receiving message from client. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    buffer[bytesRead] = '\0';

    int clientPassword = atoi(buffer);

    if (clientPassword == password) {
      std::cout << "Password is correct! You are now connected to the server.\n";

      while (true) {
        bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == 0) {
          break;
        } else if (bytesRead == -1) {
          if (errno == EINTR || errno == EAGAIN) {
            continue;
          } else {
            std::cout << "Error receiving message from client. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
          }
        }

        buffer[bytesRead] = '\0';

        std::cout << "Client: " << buffer << std::endl;

       // std::cout << "Server: ";

        //char response[100];
        //std::cin.getline(response, 100);

       // send(connection, response, sizeof(response), 0);
      }
    } else {
      std::cout << "Password is incorrect! You are not connected to the server.\n";
    }

    close(connection);
  }

  close(sockfd);
}
