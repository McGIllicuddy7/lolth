#include "async.h"
#include <stdio.h>
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
	spawn_blocking(async_main, (async_mainArgs){});
	return 0;
}
