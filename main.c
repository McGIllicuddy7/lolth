#include <stdio.h>
#include "context.h"
#include <stdlib.h>
#include <stdatomic.h>
bool done = false;
atomic_size_t counter =0;
extern thread_local size_t current_task;
void test(void*ptr);
void test_spawner(void* arg){
	task_handle_t tasks[100] = {0};
	for(int i =0; i<10; i++){
		tasks[i] = task_spawn(test, 0);
	}
/*	for(int i =0; i<10; i++){

		task_await(tasks[i]);
	}*/

}
void test(void*ptr ){
	for(int i =0; i<10; i++){
		counter++;
		printf("%d\n",i);
		yield();
	}		
}
int main(){
	lolth_init();
	task_handle_t tsks[100] = {0};
	for(int i =0; i<100; i++){
		tsks[i] = task_spawn(test,0);
	}
	for(int i =0; i<100; i++){	
		printf("context?\n");
		task_await(tsks[i]);
	}	
	printf("%zu, %zu\n",counter, current_task);
	exit(0);

}
