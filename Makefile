CC              = gcc
FLAGS           = -Wall -Wextra -Werror
SANIT           = -fsanitize=address
DBG             = -g3
INCLUDES        = -lm

all:
	$(CC) $(FLAGS) $(DBG) ph_malloc.c -o ph_malloc

sanit:
	$(CC) $(FLAGS) ph_malloc.c -o ph_malloc

run:
	./ph_malloc

%: %.c
	$(CC) $(FLAGS) $< -o $@                                                                                       