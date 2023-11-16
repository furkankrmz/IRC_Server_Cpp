#include "ft_irc.hpp"

User::User() {
  this->sock_fd = -1;
  this->is_operator = false;
  this->pass = false;
  this->nick = false;
  this->usr = false;
  this->nickname = "";
  this->username = "";
  this->realname = "";
}
User::User(int fd) {
  sock_fd = fd;
  is_operator = false;
  pass = false;
  nick = false;
  usr = false;
  nickname = "";
  username = "";
  realname = "";
}
void User::SetNickname(std::string name) { this->nickname = name; }
std::string User::GetNickname() { return this->nickname; }
void User::SetUsername(std::string name) { this->username = name; }
std::string User::GetUsername() { return this->username; }
void User::SetRealname(std::string r_name) { this->realname = r_name; }
std::string User::GetRealname() { return this->realname; }
int User::GetSocket() { return this->sock_fd; }
bool User::IsAuthenticated() { return (pass && nick && usr); }
void User::SetPass(bool pass) {this->pass = pass;}
bool User::GetPass() {return this->pass;}
void User::SetNick(bool nick) {this->nick = nick;}
bool User::GetNick() {return this->nick;}
void User::SetUsr(bool usr) {this->usr = usr;}
bool User::GetUsr() {return this->usr;}
void User::SetOper(bool oper) {this->is_operator = oper;}
bool User::GetOper() {return this->is_operator;}
