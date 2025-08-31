#include "context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#define TASK_DEBUG
pthread_mutex_t tasks_lock;

void deschedule_current_task();
bool runtime_active = false;
bool locked = false;
void* lolth_runtime(void*);
void context_spawn_thunk(context_t * prev, context_t * to_switch_to, void (* to_run)(void*), void * args){
	(void)prev;
	(void)to_switch_to;
	pthread_mutex_unlock(&tasks_lock);
	to_run(args);
	deschedule_current_task();
}
void print_unreachable(){
	printf("unreachable\n");
}
void context_spawn(context_t * prev, context_t * to_switch_to, void (* to_run)(void*), void *args);
void context_switch(context_t * prev, context_t * to_switch_to);
size_t take_count =0;
static void take_globals(){
	pthread_mutex_lock(&tasks_lock);
	locked = true;
	take_count ++;
}
static void give_up_globals(){
	take_count--;
	locked = false;
	pthread_mutex_unlock(&tasks_lock);
}
#define TASK_MAX_COUNT 16384
#define THREAD_COUNT 0
thread_local size_t current_task = 0;
thread_local size_t thread_id =0;
context_t tasks[TASK_MAX_COUNT] = {0};
pthread_t threads[THREAD_COUNT] = {0};
context_t * get_context(){
	return &tasks[current_task];
}
void lolth_runtime_update(){
	take_globals();
	assert(locked);
	int gt = -1;
	context_t * prev = get_context();
	prev->active = false;
//	printf("current task:%zu\n",current_task);
	printf("thread id:%zu\n", thread_id);
	for(int i =1; i<TASK_MAX_COUNT+1; i++){
		int x = (i+current_task)%TASK_MAX_COUNT;
		
		if(x<=THREAD_COUNT+1 && x != thread_id){
			continue;
		}
		if(tasks[x].runnable && !tasks[x].active){
			if(tasks[x].sp || !tasks[x].bp){continue;}
			/*if(tasks[x].awaiting){
				if(!tasks[i].waiter.check(&tasks[i].waiter)){
					continue;
				}
			}*/
			#ifdef TASK_DEBUG
			printf("thread:%zu switching task to %d\n", thread_id,x);
			#endif
			current_task = x;	
			tasks[x].active = true;
//			give_up_globals();
			context_switch(prev,&tasks[x]);	
//			take_globals();
			break;
		}
	}
	for(int i =0; i<TASK_MAX_COUNT; i++){
		if(!tasks[i].runnable){
			if(i == current_task){
				printf("error running unrunnable task:%d\n", i);
				abort();
			}	
			if(tasks[i].stack_ptr){
				free(tasks[i].stack_ptr);
				tasks[i].stack_ptr =0;
			}
		}
	}		
	if(current_task == thread_id){
	}
	give_up_globals();
}
void deschedule_current_task(){
	take_globals();
	#ifdef TASK_DEBUG
	printf("descheduling %zu\n", current_task);
	if(current_task<=THREAD_COUNT+1){
		__builtin_trap();
	}
	#endif
	get_context()->runnable = false;
	get_context()->active = false;
	give_up_globals();
	lolth_runtime_update();
	while(true){
		print_unreachable();
	}
}

task_handle_t task_spawn(void(*to_run)(void*), void* args){	
	take_globals();
	int tidx =-1;
	for(int i = THREAD_COUNT+2; i<TASK_MAX_COUNT; i++){
		if(!tasks[i].runnable){
			tidx = i;
			break;
		}
	}
	if(tidx == -1){
		printf("ERROR ATTEMPTED TO SPAWN THREAD AT CAPACITY");
		return -1;
	}
	context_t * prev = get_context();
	prev->active = false;
	current_task = tidx;
	printf("spawning task:%zu on thread:%zu\n", current_task, thread_id);
	context_t* cur = get_context();
	cur->runnable = true;
	cur->sp = (char*)malloc(4096)+4096;
	cur->bp = cur->sp;
	cur->ret = 0;
	cur->awaiting = false;
	cur->active = true;
	memset(&cur->waiter, 0, sizeof(cur->waiter));
	cur->stack_ptr = (char*)cur->sp-4096;		
	give_up_globals();
	context_spawn(prev, cur, to_run, args);
	
	return tidx;
}
void yield(){
	/*if(thread_id == 0){
		return;
	}*/	
	lolth_runtime_update();	
}
void spawn_rt_thread(size_t idx){
	size_t * t = (size_t*)malloc(sizeof(idx));
	*t = idx;
	pthread_create(&threads[*t], 0,lolth_runtime,t);
	return;
}
void lolth_init(){
	pthread_mutex_init(&tasks_lock, 0);
	pthread_mutex_lock(&tasks_lock);
	runtime_active = true;
	memset(tasks,0, sizeof(tasks));
	thread_id =0;
	for(int i =0; i<THREAD_COUNT; i++){
		spawn_rt_thread(i);
	}
	current_task =0;
	thread_id =0;
	tasks[0].runnable = true;
	tasks[0].active = false;
	tasks[0].awaiting = false;
	pthread_mutex_unlock(&tasks_lock);
}
void task_await(task_handle_t t){
	while(tasks[t].runnable){
		yield();
	}
}
void task_wait_for(waiter_t waiter){
	context_t * cur = get_context();
	cur->awaiting = true;
	cur->waiter = waiter;
	yield();
}
void *lolth_runtime(void*args){
	size_t * t = (size_t*)args;
	current_task = *t+1;
	thread_id = *t+1;
	get_context()->runnable = true;
	get_context()->active = false;
	while(runtime_active){
		get_context()->active = false;
//		printf("pooled current task current_task:%zu\n", current_task);
		yield();
		sleep(1);
	}
	return 0;
}
