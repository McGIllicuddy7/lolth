make: main.c
	gcc main.c -Wall -Wextra -std=c23 -g3
expand: main.c context.c  context.s
	gcc main.c context.c context.s -Wall -Wextra -std=c23 -g3 -S

