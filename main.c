/*#include "async.h"*/
#define CTILS_IMPLEMENTATION 
#include "utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "context.h"

int c = 0;
void test(void * arg){	
	for(int i =0; i<1000; i++){
//		printf("testing %d\n",i);
		c++;
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
void io_test(void * ptr){
	printf("%s\n", lolth_read_to_string(0,"main.c").items);
}

int main(){
	lolth_init();
	TaskHandle h[10];
	for(int i =0; i<10; i++){
		h[i] = spawn(test_spawn,0);
	}
	for(int i =0; i<10; i++){
		lolth_await(h[i]);
	}
	printf("done, count:%d\n",c);
	lolth_finish();
	return 0;
}
