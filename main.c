#include "async.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
async(int, test, {}, {int i;},
      { 
	for(v->i=0; v->i<10; v->i++){
		printf("%d\n",v->i);
		lolth_yield();
	}
	return PollFinished;
	}
)
async(int, async_main, {}, {}, 
      {
	printf("spawned\n");
	async_spawn(test,(testArgs){});
	printf("spawned 1\n");
	async_spawn(test,(testArgs){});
	printf("spawned 2\n");
	return PollFinished;
      }
)

int main(){
//	spawn_blocking(async_main, (async_mainArgs){});
	FILE * f = fopen("main.c", "r");
	struct aiocb io;
	char buff[16000] = {0};
	begin_async_read(&io, f, buff, 16000);
	int d = 0;
	while((d = aio_error(&io))!= 0){
	  printf("waiting:%d\n",d);
	}
	printf("%s\n", buff);
	return 0;
}
