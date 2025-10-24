make: main.c reflect.c stream.c
	gcc main.c reflect.c stream.c -Wall -Wextra -pedantic -std=c99 -g3 -fsanitize=address
