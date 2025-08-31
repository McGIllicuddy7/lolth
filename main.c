#include <stdio.h>
#include "context.h"
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
atomic_size_t counter =0;
volatile bool done = false;
extern thread_local size_t task_id;
context_t a;
context_t b;
void test(void*ptr);
/*void test_spawner(void* arg){
	task_handle_t tasks[100] = {0};
	for(int i =0; i<10; i++){
		tasks[i] = task_spawn(test, 0);
	}
	for(int i =0; i<10; i++){

		task_await(tasks[i]);
	}
}*/
void test(void*ptr ){
	for(int i =0; i<100000; i++){
		counter++;
		printf("%d\n",i);
		yield();	
	}		
	done  = true;
}
/*
void old_main(){
	lolth_init();
	task_handle_t tsks[100] = {0};
	for(int i =0; i<10; i++){
		tsks[i] = task_spawn(test,0);
		printf("spawned\n");
	}
	for(int i =0; i<10; i++){	
		printf("context?\n");
		task_await(tsks[i]);
	}	
	printf("%zu, %zu\n",counter, current_task);
	exit(0);

}*/
void basic_main(){
	b.bp = malloc(4096)+4096;
	b.sp = b.bp;
	b.ret = 0;
	b.stack_ptr =b.bp-4096;
	b.running =false;
	b.active = true;
	b.awaiting = false;
	memset(&b.waiter,0, sizeof(b.waiter));
	context_spawn(&a, &b, test,0);
	int count =0;
	while(!done){
		printf("i:%d\n",count);
		count++;
		context_switch(&a, &b);
	}

}
int main(){
	lolth_init();
	task_handle_t tsks[10] = {0};
	for(int i =0; i<10; i++){
		tsks[i] = task_spawn(test,0);
		printf("spawned\n");
	}
	for(int i =0; i<10; i++){	
		printf("context?\n");
		task_await(tsks[i]);
	}	
	printf("%zu, %zu\n",counter, task_id);
	exit(0);

}
