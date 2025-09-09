make: main.c context.c
	gcc main.c context.c -Wall -Wextra -g3 -std=c2x -Wall -Wextra  -fsanitize=address -lm 

windows: main.c context.c
	x86_64-w64-mingw32-gcc main.c context.c -Wall -Wextra -g3 -std=c99 -Wall -Wextra  -fsanitize=address -lm

