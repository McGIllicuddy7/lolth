#include "context.h"
#include <setjmp.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
typedef struct {
	jmp_buf buf;
	bool valid;
}Task;
#define TASK_COUNT 16
Task tasks [TASK_COUNT] = {0};
thread_local long current_task =0;
Task * get_current_task(){
	return &tasks[current_task];
}
extern void task_switch(Task* from, Task* to);
extern void [[no_return]] task_spawn_asm(void * stack, void(*to_call)(void*), void* args);
extern void scheduler(bool is_done);
void task_spawn_thunk(void * stack, void(*to_call)(void*), void* args){
	printf("ptr:%p\n", to_call);
	to_call(args);
	for(int i =0; i<TASK_COUNT; i++){
		printf("task %d:%s\n",i,tasks[i].valid ? "true" :"false");
	}
	scheduler(true);
	assert(false);
}
void yield(){
	scheduler(false);
}
TaskHandle spawn(void (*to_call)(void*),void * args){
	void * stack = malloc(4096)+4096;
	printf("stack ptr = %p\n", stack);
	Task * cur = get_current_task();
	long tsk = -1;
	for(int i =0; i<TASK_COUNT; i++){
		if(!tasks[i].valid){
			tasks[i].valid = true;
			printf("%d is _valid\n", i);
			tsk = i;
			break;
		}
	}
	if(tsk == -1){
		return tsk;
	}
	printf("spawning task:%ld from %ld\n", tsk, current_task);
	current_task = tsk;
	if(!setjmp(cur->buf)){
		task_spawn_asm(stack, to_call, args);
	}	
	return tsk;
}
void task_switch(Task*from, Task* to){
	if(!setjmp(from->buf)){
		longjmp(to->buf, 0);
	}
	return;
}
void scheduler(bool is_done){
	get_current_task()->valid = !is_done;
	int to_jump_to =-1;
	for(int j =1; j<TASK_COUNT+1; j++){
		int i = (j+current_task)%TASK_COUNT;
		if(tasks[i].valid){
			to_jump_to = i;
			break;
		}
	}	
	printf("jumping to %d\n",to_jump_to);
	long prev = current_task;
	current_task = to_jump_to;
	task_switch(&tasks[prev],get_current_task());
}
void lolth_init(){
	for(int i =0; i<TASK_COUNT; i++){
		tasks[i].valid = false;
	}
	tasks[0].valid = true;

}
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
