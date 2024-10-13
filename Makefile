CC = cc
CFLAGS = -Wall -Werror -Wextra -I include/
SRCS_DIR=src/
OBJS_DIR=obj/
SRCS = $(wildcard $(SRCS_DIR)*.c)
OBJS = $(SRCS:$(SRCS_DIR)%.c=$(OBJS_DIR)%.o)
EXEC = ft_ping

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c | $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

all: $(EXEC)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: clean fclean re
