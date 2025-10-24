make: main.c 
	gcc main.c -Wall -Wextra -pedantic -std=c99 -g3 -fsanitize=address
