make: main.c context.c
	gcc main.c context.c -Wall -Wextra -g3 -std=c23 -Wall -Wextra  -fsanitize=address -lm

