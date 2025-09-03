#include "async.h"
#include <stdio.h>
async(int, test, {}, {int i;},
      { 
	for(v->i=0; v->i<10; v->i++){
		printf("%d\n",v->i);
		yield();
	}
	return PollFinished;
      }
);
async(int, test_spawn, {}, {int i;testFuture futs[10]; bool finished;},
      {
	for(v->i =0; v->i<10; v->i++){
		v->futs[v->i] = test((testArgs){});
	}
	do {	
		v->finished=true;
		for(v->i =0; v->i<10; v->i++){
			printf("%d:", v->i);
			if(poll(v->futs[v->i])){
				v->finished = false;
			}
		}
	}while(!v->finished);
		return PollFinished;
	}
)
int main(){
	test_spawnFuture t= test_spawn((test_spawnArgs){});
	while(poll(t)){
		printf("testing 123\n");
	}
	return 0;
}
