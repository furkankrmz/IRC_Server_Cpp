#include "ft_irc.hpp"

ft_irc::ft_irc(/* args */) {
  port_number = 0;
  password = "";
  operator_password = "kedi172";
  sock_fd = -1;
  Channel chnl1("linux");
  Channel chnl2("42");
  Channel chnl3("itu");
  channels.insert(std::pair<std::string, Channel>("linux", chnl1));
  channels.insert(std::pair<std::string, Channel>("42", chnl2));
  channels.insert(std::pair<std::string, Channel>("itu", chnl3));
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

User ft_irc::findUserBySocket(int sockfd) {
  std::map<int, User>::iterator it = users.find(sockfd);
  if (it != users.end())
    return (it->second);
  throw ("User not found");
}

User ft_irc::findUserByNickname(std::string nickname) {
  std::map<int, User>::iterator it = users.begin();
  while (it != users.end()) {
    if (it->second.GetNickname() == nickname) {
      return (it->second);
    }
    it++;
  }
  throw ("User not found");
}

User ft_irc::findUserByUsername(std::string username) {
  std::map<int, User>::iterator it = users.begin();
  while (it != users.end()) {
    if (it->second.GetUsername() == username) {
      return (it->second);
    }
    it++;
  }
  throw ("User not found");
}

void ft_irc::removeUser(int sockfd) {
  std::map<int, User>::iterator it = users.find(sockfd);
  if (it != users.end())
    users.erase(sockfd);
}

void ft_irc::Welcome(int sockfd) {
  const char *promptMessage = "\033[1;35mWelcome to the server! \033[1;0m \r\n";
  SendMessage(sockfd, promptMessage);
  promptMessage =
      "\033[1;35mPlease login to use chat. Use HELP command to learn how to "
      "login. \033[1;0m \r\n";
  SendMessage(sockfd, promptMessage);
}

void ft_irc::HELP(int sockfd) {
  std::map<int, User>::iterator it = users.find(sockfd);
  if (!it->second.IsAuthenticated()) {
    const char *promptMessage = "\033[1;32mSTEP 1: PASS \033[1;0m\r\n";
    SendMessage(sockfd, promptMessage);
    promptMessage =
        "    Use PASS command to set a password. e.g: PASS <server password>\r\n";
    SendMessage(sockfd, promptMessage);
    promptMessage = "\033[1;32mSTEP 2: NICK \033[1;0m\r\n";
    SendMessage(sockfd, promptMessage);
    promptMessage =
        "    Use NICK command to set a nickname. e.g: NICK <nickname> \r\n";
    SendMessage(sockfd, promptMessage);
    promptMessage = "\033[1;32mSTEP 3: USER \033[1;0m\r\n";
    SendMessage(sockfd, promptMessage);
    promptMessage =
        "    Use USER command to register your username and fullname.e.g: USER "
        "<username> * * :<fullname> \r\n";
    SendMessage(sockfd, promptMessage);
  } else {
    const char *promptMessage = "\033[1;32mYComing soon! \033[1;0m\r\n";
    SendMessage(sockfd, promptMessage);
  }
}
// Function to authenticate password
void ft_irc::PASS(int clientSocket, const std::vector<std::string> &args) {
  std::map<int, User>::iterator it = users.find(clientSocket);
  std::string clientPassword = args[1];
  std::cout << clientPassword << std::endl;
  if (clientPassword != password) {
    const char *invalidMessage =
        "\033[1;31mInvalid password! Please try again.\033[1;0m\r\n";
    SendMessage(clientSocket, invalidMessage);
  } else {
    const char *successMessage = "\033[1;32mClient authenticated!\033[1;0m\r\n";
    SendMessage(clientSocket, successMessage);
    it->second.SetPass(true);
    std::cout << "\033[1;32mClient on socket " << clientSocket
              << " authenticated!\033[1;0m\r\n";
  }
}

void ft_irc::NICK(int connection, const std::vector<std::string> &args) {
  std::string oldNickname = findUserBySocket(connection).GetNickname();
  std::string newNickname = args[1];
  try {
    findUserByNickname(newNickname);
    const char *message =
        "\033[1;31mNickname is already in use. Please choose a different "
        "one.\033[1;0m\r\n";
    send(connection, message, strlen(message), 0);
  }
  catch(...) {
    const char *successMessage =
        "\033[1;32mNickname changed successfully!\033[1;0m\r\n";
    SendMessage(connection, successMessage);
    std::map<int, User>::iterator it = users.find(connection);
    it->second.SetNickname(newNickname);
    it->second.SetNick(true);
  }
}

void ft_irc::PRIVMSG(int connection, const std::vector<std::string>& args){
  std::string nick = args[1];
  std::string msg = args[2] + "\n";
  const char* p_message = msg.c_str();
  try {
    User target = findUserByNickname(nick);
    User sender = findUserBySocket(connection);
    std::string senderinfo = sender.GetNickname() + ": ";
    const char* sndr = senderinfo.c_str();
    SendMessage(target.GetSocket(), sndr);
    SendMessage(target.GetSocket(), p_message);
  }
  catch(const std::exception& e) {
    const char *invalidMessage = "\033[1;31mUser not found!\033[1;0m\r\n";
    SendMessage(connection, invalidMessage);
  }
}

void ft_irc::OPER(int sockfd, const std::vector<std::string> &args) {
  if (args[2] != operator_password) {
    const char *invalidMessage = "\033[1;31mIncorrect password\033[1;0m\r\n";
    SendMessage(sockfd, invalidMessage);
    return;
  }
  try {
    User usr = findUserByUsername(args[1]);
    std::map<int, User>::iterator it = users.find(usr.GetSocket());
    if(it->second.GetOper())
    {
      std::string msg = "\033[1;33m" + args[1] + " is already operator! \033[1;0m\r\n";
      const char *invalidMessage = msg.c_str();
      SendMessage(sockfd, invalidMessage);
      return;
    }
    it->second.SetOper(true);
    std::string msg = "\033[1;32m" + args[1] + " is now operator\033[1;0m\r\n";
    const char *message = msg.c_str();
    send(sockfd, message, strlen(message), 0);
  }
  catch(const std::exception& e) {
    const char *invalidMessage = "\033[1;31mUser not found!\033[1;0m\r\n";
    SendMessage(sockfd, invalidMessage);
  }
}

void ft_irc::LIST(int sockfd){
  std::map<std::string, Channel>::iterator it = channels.begin();
  while (it != channels.end()) {
    std::string message = "\033[1;35m" + it->first + "\033[1;0m\r\n";
    const char *promptMessage = message.c_str();
    SendMessage(sockfd, promptMessage);
    it++;
  }
}

void ft_irc::JOIN(int sockfd, const std::vector<std::string> &args) {
  std::map<std::string, Channel>::iterator it = channels.find(args[1]);
  try {
    if (it != channels.end()) {
      it->second.addUser(users.find(sockfd)->second);
      const char *message = "\033[1;32mYou succesfully joined to channel\033[1;0m\r\n";
      send(sockfd, message, strlen(message), 0);
    }
    else {
      Channel chnl(args[1]);
      channels.insert(std::pair<std::string, Channel>(args[1], chnl));
      const char *successMessage = "\033[1;32mChannel successfully created!\033[1;0m\r\n";
      SendMessage(sockfd, successMessage);
      std::map<std::string, Channel>::iterator it = channels.find(args[1]);
      it->second.addUser(users.find(sockfd)->second);
      const char *message = "\033[1;32mYou succesfully joined to channel\033[1;0m\r\n";
      send(sockfd, message, strlen(message), 0);
    }
  }
  catch(...) {
    const char *message = "\033[1;31mYou are already in channel\033[1;0m\r\n";
    send(sockfd, message, strlen(message), 0);
  }
}

void ft_irc::KICK(int sockfd, const std::vector<std::string> &args) {
  std::string channel = args[1];
  std::string client = args[2];
  if (channels.find(channel) == channels.end()) {
    const char *message = "\033[1;31mChannel not found\033[1;0m\r\n";
    send(sockfd, message, strlen(message), 0);
    return;
  }
  std::map<std::string, Channel>::iterator it = channels.find(channel);
  try {
    it->second.kickUser(findUserByUsername(client));
    const char *message = "\033[1;32mYou succesfully kicked user\033[1;0m\r\n";
    send(sockfd, message, strlen(message), 0);
  }
  catch(...) {
    const char *message = "\033[1;31mUser not in channel!\033[1;0m\r\n";
    send(sockfd, message, strlen(message), 0);
  }
}

void ft_irc::USER(int sockfd, const std::vector<std::string> &args) {
  std::map<int, User>::iterator it = users.find(sockfd);
  SendMessage(sockfd, "\033[1;32mWelcome \033[1;0m\r\n");
  it->second.SetUsr(true);
  it->second.SetUsername(args[1]);
  it->second.SetRealname(args[4]);
}


