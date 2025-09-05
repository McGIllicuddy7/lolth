make: main.c async.c
	gcc main.c async.c -Wall -Wextra -g3 -std=c11 
expand: main.c context.c  context.s
	gcc main.c context.c context.s -Wall -Wextra -g3 -S

