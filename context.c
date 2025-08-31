#include "context.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
static pthread_mutex_t lock;
#define TASK_COUNT 4096
static context_t tasks[TASK_COUNT] = {0};
thread_local long task_id = 0;
extern void scheduler(bool should_deschedule);
void context_spawn_thunk(context_t * prev, context_t * to_switch_to, void (*to_run)(void*), void * args){
	(void)prev;
	(void)to_switch_to;
	to_run(args);
	//printf("finished task:%ld\n", task_id);
	scheduler(true);
	
}

task_handle_t task_spawn(void (*func)(void *), void * args){
	long id = -1;
	for(int i =0; i<TASK_COUNT;i++){
		if(!tasks[i].active){
			id = i;
			tasks[i].active = true;
			tasks[i].running = true;
			tasks[i].stack_ptr = malloc(4096);
			tasks[i].bp = tasks[i].stack_ptr+4096;
			tasks[i].sp = tasks[i].bp;
			long old= task_id;
			tasks[task_id].running = false;
			task_id = i;
			context_spawn(&tasks[old], &tasks[i], func, args);
			break;
		}
	}
	return id;
}
void yield(){
	scheduler(false);
}

void lolth_init(){
	pthread_mutex_init(&lock,0);
	tasks[0].running = true;
	tasks[0].active = true;
}


void scheduler(bool should_deschedule){
	//pthread_mutex_lock(&lock);
	tasks[task_id].active = !should_deschedule;
	//printf("scheduler\n");
	for(int i =task_id+1; i<TASK_COUNT+1; i++){
		long idx = i%TASK_COUNT;
		if(!tasks[idx].running && tasks[idx].active){
			tasks[task_id].running = false;
			tasks[idx].running = true;
			//printf("switching to %ld\n", idx);
			long id = task_id;
			task_id = idx;
			context_switch(&tasks[id], &tasks[idx]);
		}
	}
	//pthread_mutex_unlock(&lock);
}

void task_await(task_handle_t handle){
	while(tasks[handle].active){
		yield();
	}
}
