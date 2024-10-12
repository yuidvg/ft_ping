CC = cc
CFLAGS = -Wall -Werror -Wextra -I include/
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=obj/%.o)
EXEC = ft_ping

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

$(OBJ_DIR)%.o: $(SRCS_DIR)%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(EXEC)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: clean fclean re
