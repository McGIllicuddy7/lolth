#include "context.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
int usleep(long);
typedef struct {
	long id;
}worker_data_t;
static pthread_mutex_t lock;
#define WORKER_COUNT 7
static pthread_t workers[WORKER_COUNT] = {0};
#define TASK_COUNT 4096
static thread_local context_t tasks[TASK_COUNT] = {0};
static volatile bool run_tasks = true;
thread_local long task_id = 0;
thread_local long cpu_id = 0;
extern void scheduler(bool should_deschedule);
extern void worker_routine(worker_data_t * data);
void context_spawn_thunk(context_t * prev, context_t * to_switch_to, void (*to_run)(void*), void * args){
	(void)prev;
	(void)to_switch_to;	
	pthread_mutex_unlock(&lock);
	to_run(args);
	//printf("finished task:%ld\n", task_id);	
	scheduler(true);
	
}

task_handle_t task_spawn(void (*func)(void *), void * args){
	long id = -1;
	pthread_mutex_lock(&lock);
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
	pthread_mutex_lock(&lock);
	for(int i =0; i<WORKER_COUNT; i++){
		worker_data_t * data = malloc(sizeof(worker_data_t));
		data->id = i+1;	
		tasks[i+1].running = true;
		tasks[i+1].active = true;
		pthread_create(&workers[i], 0, (void*)worker_routine,data);
	}
	pthread_mutex_unlock(&lock);

}


void scheduler(bool should_deschedule){
	pthread_mutex_lock(&lock);
	tasks[task_id].active = !should_deschedule;	
	for(int i = 1; i<TASK_COUNT+1; i++){
		long idx = (i+task_id)%TASK_COUNT;
		if(idx<WORKER_COUNT&& idx!= cpu_id){
			continue;
		}
		if((!tasks[idx].running || idx == task_id) && tasks[idx].active){
			tasks[task_id].running = false;
			tasks[idx].running = true;
			//printf("switching to %ld\n", idx);
			long id = task_id;
			task_id = idx;
			pthread_mutex_unlock(&lock);
			context_switch(&tasks[id], &tasks[idx]);
		}
	}
//	assert(tasks[task_id].active);
//	pthread_mutex_unlock(&lock);

}

void task_await(task_handle_t handle){
	while(tasks[handle].active){
		yield();
	}
}
void worker_routine(worker_data_t * data){
	cpu_id = data->id;
	task_id = cpu_id;	
	while(run_tasks){
		printf("schedluing");
		scheduler(false);
		usleep(100);
	}
}
