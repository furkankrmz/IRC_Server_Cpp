NAME = ircserv
CC = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	main.cpp ft_irc.cpp parser.cpp command_handler.cpp user.cpp\

OBJ =	$(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
		$(CC) $(CXXFLAGS) -o $(NAME) $(OBJ)

clean:
		rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all
