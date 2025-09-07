make: main.c context.c
	gcc main.c context.c -Wall -Wextra -O3  -std=c99 -Wall -Wextra -fsanitize=address

