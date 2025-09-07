/*#include "async.h"*/
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "context.h"
#include <pthread.h>
void test(void * arg){	
	for(int i =0; i<10; i++){
		printf("testing %d\n",i);
		yield();
	}

}
void test_spawn(void *arg){
	TaskHandle h[10];
	for(int i =0; i<10; i++){
		h[i] =spawn(test,0);
	}
	for(int i =0; i<10; i++){
		lolth_await(h[i]);
	}	
}

int main(){
	lolth_init();
	TaskHandle h[10];
	for(int i =0; i<10; i++){
		h[i] =spawn(test_spawn,0);
	}
	for(int i =0; i<10; i++){
		lolth_await(h[i]);
	}	
	printf("exit success\n");
	return 0;
}
