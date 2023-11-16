#include "ft_irc.hpp"

ft_irc::ft_irc(/* args */) {
  int port_number = 0;
  std::string password = "";
  int sock_fd = -1;
  (void)sock_fd;
  (void)port_number;
};
ft_irc::~ft_irc(){};

void ft_irc::SetPortNumber(int port_number) {
  this->port_number = port_number;
};
int ft_irc::GetPortNumber() { return this->port_number; };
void ft_irc::SetServerPassword(std::string password) {
  this->password = password;
};
std::string ft_irc::GetServerPassword() { return this->password; };
void ft_irc::SetSockFD(int sock_fd) { this->sock_fd = sock_fd; };
int ft_irc::GetSockFD() { return this->sock_fd; };

// Function to create a socket
void ft_irc::CreateSocket() {
  this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock_fd == -1) {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
};

// Function to bind the socket to a port
void ft_irc::BindSocket(int sockfd, int portNumber) {
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(portNumber);

  if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cout << "Failed to bind to port " << portNumber << " errno: " << errno
              << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Function to listen for connections
void ft_irc::StartListening(int sockfd) {
  if (listen(sockfd, 100) < 0) {
    std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Function to accept a connection
int ft_irc::AcceptConnection(int sockfd) {
  socklen_t addrlen;
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;

  int connection =
      accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
  if (connection < 0) {
    std::cout << "Failed to grab connection. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }
  User user(connection);
  users.insert(std::pair<int, User>(connection, user));
  // Print the port number on which the connection was established
  std::cout << "New connection from " << inet_ntoa(sockaddr.sin_addr)
            << " on socket " << connection << std::endl;
  return connection;
}
void ft_irc::SendMessage(int sockfd, const char *message) {
  send(sockfd, message, strlen(message), 0);
}

void ft_irc::Welcome(int sockfd) {
  const char *promptMessage = "\033[1;35mWelcome to the server! \033[1;0m \n";
  SendMessage(sockfd, promptMessage);
  promptMessage =
      "\033[1;35mPlease login to use chat. Use HELP command to learn how to "
      "login. \033[1;0m \n";
  SendMessage(sockfd, promptMessage);
}

void ft_irc::HELP(int sockfd) {
  std::map<int, User>::iterator it = users.find(sockfd);
  if (!it->second.IsAuthenticated()) {
    const char *promptMessage = "\033[1;32mSTEP 1: PASS \033[1;0m\n";
    SendMessage(sockfd, promptMessage);
    promptMessage =
        "    Use PASS command to set a password. e.g: PASS <server password>\n";
    SendMessage(sockfd, promptMessage);
    promptMessage = "\033[1;32mSTEP 2: NICK \033[1;0m\n";
    SendMessage(sockfd, promptMessage);
    promptMessage =
        "    Use NICK command to set a nickname. e.g: NICK <nickname> \n";
    SendMessage(sockfd, promptMessage);
    promptMessage = "\033[1;32mSTEP 3: USER \033[1;0m\n";
    SendMessage(sockfd, promptMessage);
    promptMessage =
        "    Use USER command to register your username and fullname.e.g: USER "
        "<username> * * :<fullname> \n";
    SendMessage(sockfd, promptMessage);
  } else {
    const char *promptMessage = "\033[1;32mYComing soon! \033[1;0m\n";
    SendMessage(sockfd, promptMessage);
  }
}
// Function to authenticate password
void ft_irc::PASS(int clientSocket, const std::vector<std::string> &args) {
  std::map<int, User>::iterator it = users.find(clientSocket);
  if (!it->second.GetPass()) {
    std::map<int, User>::iterator it = users.find(clientSocket);
    std::string clientPassword = args[1];
    std::cout << clientPassword << std::endl;
    if (clientPassword != password) {
      const char *invalidMessage =
          "\033[1;31mInvalid password! Please try again.\033[1;0m\n";
      SendMessage(clientSocket, invalidMessage);
    } else {
      const char *successMessage = "\033[1;32mClient authenticated!\033[1;0m\n";
      SendMessage(clientSocket, successMessage);
      it->second.SetPass(true);
      std::cout << "\033[1;32mClient on socket " << clientSocket
                << " authenticated!\033[1;0m\n";
    }
  } else {
    const char *invalidMessage =
        "\033[1;33mYou already authorized your password.\033[1;0m\n";
    SendMessage(clientSocket, invalidMessage);
  }
}

void ft_irc::NICK(int connection, const std::vector<std::string> &args) {
  if (args.size() != 2) {
    const char *invalidMessage =
        "\033[1;31mIncorrect use of command! Correct usage: NICK "
        "<nickname>\033[1;0m\n";
    SendMessage(connection, invalidMessage);
    return;
  }
  static std::unordered_set<std::string> usedNicknames;

  std::string oldNickname = connectionNicknameMap[connection];
  std::string newNickname = args[1];
  if (usedNicknames.find(args[1]) != usedNicknames.end()) {
    const char *message =
        "\033[1;31mNickname is already in use. Please choose a different "
        "one.\033[1;0m\n";
    send(connection, message, strlen(message), 0);
  } else {
    const char *successMessage =
        "\033[1;32mNickname changed successfully!\033[1;0m\n";
    SendMessage(connection, successMessage);
    usedNicknames.erase(oldNickname);
    usedNicknames.insert(newNickname);
    std::map<int, User>::iterator it = users.find(connection);
    it->second.SetNickname(newNickname);
  }
}

void ft_irc::PRIVMSG(int connection, const std::vector<std::string>& args){
  if (args.size() < 3) {
    const char *invalidMessage =
        "\033[1;31mIncorrect use of command! Correct usage: PRIVMSG <nickname> "
        "<message>\033[1;0m\n";
    SendMessage(connection, invalidMessage);
    return;
  }
  std::string nick = args[1];
  const char* p_message = args[2].c_str();
  std::map<int, User>::iterator it = users.begin();
  while (it != users.end()) {
    if (it->second.GetNickname() == nick) {
      SendMessage(it->first, p_message);
      return;
    }
    it++;
  }
  const char *invalidMessage = "\033[1;31mUser not found!\033[1;0m\n";
  SendMessage(connection, invalidMessage);
}
void ft_irc::USER(int sockfd, const std::vector<std::string> &args) {
  std::map<int, User>::iterator it = users.find(sockfd);
  if (!it->second.GetUsr()) {
    const char *promptMessage = "\033[1;32mWelcome \033[1;0m\n";
    SendMessage(sockfd, promptMessage);
    std::map<int, User>::iterator it = users.find(sockfd);
    it->second.SetUsr(true);
    it->second.SetUsername(args[1]);
  } else {
    const char *invalidMessage = "\033[1;33mYou already set user.\033[1;0m\n";
    SendMessage(sockfd, invalidMessage);
  }
}


