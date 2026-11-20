NAME = vex
TEST_NAME = test_gb
TEST_SRC  = tests/test_gap_buffer.c src/buffer/gap_buffer.c




CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude

SRC = src/main.c \
	  src/terminal/raw_mode.c \
	  src/terminal/input.c \
	  src/terminal/output.c \
	  src/terminal/window.c \
	  src/buffer/gap_buffer.c \
	  src/document/document.c 


OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)



test :
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_NAME)
	./$(TEST_NAME)
clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

re: fclean all

.PHONY: all clean fclean re test