#include "networking.h"

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

  while (true) {
    int connection = acceptConnection(sockfd);
if (connection < 0) {
      std::cout << "Failed to grab connection. errno: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }

    char buffer[100];
    int bytesRead = 0;

    char message[] = "Welcome to the server! Please enter your password: \n";
    send(connection, message, sizeof(message), 0);
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
      const char *successMessage = "Password is correct! You are now connected to the server.\n";
      send(connection, successMessage, strlen(successMessage), 0);

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

      }
    } else {
        const char *failureMessage = "Password is incorrect! You are not connected to the server.\n";
        send(connection, failureMessage, strlen(failureMessage), 0);    
      }
      
    close(connection);
  }

  close(sockfd);
}