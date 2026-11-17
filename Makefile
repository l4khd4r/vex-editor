NAME = vex

CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude

SRC = src/main.c \
      src/terminal.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re