NAME = server
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	main.cpp connection_functions.cpp socket_functions.cpp\

OBJ =	$(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
		$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
		rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all
