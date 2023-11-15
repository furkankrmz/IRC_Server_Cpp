NAME = server
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	main.cpp ft_irc.cpp parser.cpp\

OBJ =	$(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
		$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
		rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all
