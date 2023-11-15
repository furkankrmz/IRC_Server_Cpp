#include "ft_irc.hpp"

std::vector<std::string> parse(std::string line) {
  size_t iterator = 0;
  size_t start = 0;
  size_t length = line.find_first_of('\n');
  std::vector<std::string> args;

  while (iterator < length) {
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
      iterator++;
      continue;
    }
    start = iterator;
    while (line[iterator] != ' ' && line[iterator] != '\t' && line[iterator] != '\n' && line[iterator] != '\0') {
      iterator++;
    }
    args.push_back(line.substr(start, (iterator - start)));
  }
  return (args);
}
