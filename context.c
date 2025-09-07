#include "context.h"
#include <setjmp.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
typedef struct {
	jmp_buf buf;
	bool valid;
	bool running;
}Task;
#define TASK_COUNT (4096*4)
#define THREAD_COUNT 8
Task tasks [TASK_COUNT] = {0};
_Thread_local long current_task =0;
_Thread_local long prev_running =-1;
_Thread_local long thread_id =0;
pthread_mutex_t task_lock;
Task * get_current_task(){
	return &tasks[current_task];
}
extern void task_switch(Task* from, Task* to);
extern void task_spawn_asm(void * stack, void(*to_call)(void*), void* args);
extern void scheduler(bool is_done);
void task_spawn_thunk(void * stack, void(*to_call)(void*), void* args){
	pthread_mutex_lock(&task_lock);
	if(prev_running != current_task){
		tasks[prev_running].running =false;
		prev_running = -1;
	}
	pthread_mutex_unlock(&task_lock);
	to_call(args);
	scheduler(true);
	assert(false);
}
void yield(){
	scheduler(false);
}
TaskHandle spawn(void (*to_call)(void*),void * args){
	void * stack = malloc(4096)+4096;	
	Task * cur = get_current_task();
	long tsk = -1;
	pthread_mutex_lock(&task_lock);
	for(int i =0; i<TASK_COUNT; i++){
		if(!tasks[i].valid){
			tasks[i].valid = true;
			tasks[i].running = true;
			tsk = i;
			break;
		}
	}
	if(tsk == -1){
		return tsk;
	}
	prev_running = current_task;
	current_task = tsk;	
	pthread_mutex_unlock(&task_lock);
	if(!setjmp(cur->buf)){
		task_spawn_asm(stack, to_call, args);
	}	
	pthread_mutex_lock(&task_lock);
	if(prev_running != current_task){
		tasks[prev_running].running =false;
		prev_running = -1;
	}
	pthread_mutex_unlock(&task_lock);
	return tsk;
}
void task_switch(Task*from, Task* to){		
	if(!setjmp(from->buf)){
		longjmp(to->buf, 0);
	}
	pthread_mutex_lock(&task_lock);
	if(prev_running != current_task){
		tasks[prev_running].running =false;
		prev_running = -1;
	}
	pthread_mutex_unlock(&task_lock);
	return;
}
void scheduler(bool is_done){
	pthread_mutex_lock(&task_lock);
	get_current_task()->valid = !is_done;
	int to_jump_to =-1;
	for(int j =1; j<TASK_COUNT+1; j++){
		int i = (j+current_task)%TASK_COUNT;
		if(i != thread_id && i<THREAD_COUNT){
			continue;
		}
		if(tasks[i].valid&& !tasks[i].running){
			tasks[i].running = true;
			to_jump_to = i;
			break;
		}
	}		
	if(current_task == to_jump_to){
		prev_running = current_task;
		pthread_mutex_unlock(&task_lock);
		return;
	}else{
		long prev = current_task;
		prev_running = prev;
		current_task = to_jump_to;
		pthread_mutex_unlock(&task_lock);
		task_switch(&tasks[prev],get_current_task());
	}
}
void* thread_loop(void*args){
	extern void usleep(long);
	thread_id = (size_t)args;
	current_task = thread_id;
	while(true){
		usleep(5000);
		yield();	
		
	}
}

void lolth_init(){
	pthread_mutex_init(&task_lock,0);
	for(int i =0; i<TASK_COUNT; i++){
		tasks[i].valid = false;
	}
	pthread_mutex_lock(&task_lock);
	tasks[0].valid = true;
	tasks[0].running = true;
	for(int i =1; i<THREAD_COUNT; i++){
		tasks[i].valid = true;
		tasks[i].running = true;
		pthread_t thread;
		pthread_create(&thread,0, thread_loop,(void*)i);
	}
	pthread_mutex_unlock(&task_lock);

}
void lolth_await(TaskHandle handle){
	while(tasks[handle].valid){
		yield();		
	}
}
#ifdef __x86_64__
__asm(
	".intel_syntax noprefix\n"
	".global task_spawn_asm\n"
	".extern task_spawn_thunk\n"
	"task_spawn_asm:\n"
	"	mov rsp, rdi\n"
	"	mov rbp, rdi\n"
	"       call task_spawn_thunk\n"	
	"       ret\n"
	".att_syntax prefix\n"
);
#else
__asm(
	"_task_spawn_asm:\n"
	"	mov sp, x0\n"
	"	mov bp, x0\n"
	"       call task_spawn_thunk\n"	
	"       ret\n"
);
#endif


