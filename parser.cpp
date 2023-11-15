#include "ft_irc.hpp"

std::vector<std::string> parse(std::string line) {
  size_t iterator = 0;
  size_t start = 0;
  std::cout << "line " << line << std::endl; 
  std::vector<std::string> args;
  while (iterator < line.length()) {
    while (line[iterator] == ' ' || line[iterator] == '\t') {
      iterator++;
    }
    if (line[iterator] == '"') {
      iterator++;
      start = iterator;
      while (line[iterator] != '"') {
        iterator++;
      }
      args.push_back(line.substr(start, (iterator - start)));
      std::cout << "args " << line.substr(start, (iterator - start)) << std::endl;
      iterator++;
      continue;
    }
    start = iterator;
    while (line[iterator] != ' ' && line[iterator] != '\t' && line[iterator] != '\0') {
      iterator++;
    }
    args.push_back(line.substr(start, (iterator - start)));
    std::cout << "args  " << line.substr(start, (iterator - start)) << std::endl;
  }
  return (args);
}
