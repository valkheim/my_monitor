all:
	gcc main.c utils.c -I . -lpthread -o my_monitor -Wall -Wextra -Werror
