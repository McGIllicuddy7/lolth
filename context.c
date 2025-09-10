#include "context.h"
#include <setjmp.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"
#ifdef WIN32
#include <Windows.h>
#define atomic_bool int
#define _Thread_local __declspec( thread )
#else
#include <stdatomic.h>
#include <unistd.h>
#include <pthread.h>
#include <aio.h>
#endif



typedef struct {
	jmp_buf buf;
	void * stack;
	bool valid;
	bool running;
}Task;
volatile atomic_bool threads_should_continue = false;
volatile atomic_bool valid_rt = false;
#define TASK_COUNT (16000)
#define THREAD_COUNT_MAX 16
size_t THREAD_COUNT= 4;
Task tasks [TASK_COUNT] = {0};
_Thread_local long current_task =0;
_Thread_local long prev_running =-1;
_Thread_local long thread_id =0;
Mutex_t task_lock;
Mutex_t threads[THREAD_COUNT_MAX-1];
jmp_buf tsaves[THREAD_COUNT_MAX-1];
atomic_bool ready_to_finish[THREAD_COUNT_MAX-1] = {0};
#ifdef __linux__
extern void usleep(long);
#endif
extern int fileno(FILE * f);
Task * get_current_task(){
	return &tasks[current_task];
}
extern void task_switch(Task* from, Task* to);
extern void task_spawn_asm(void * stack, void(*to_call)(void*), void* args);
extern void scheduler(bool is_done);
extern void context_gc();
void task_spawn_thunk(void * stack, void(*to_call)(void*), void* args){
	(void)stack;
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
	if(!valid_rt){
		return;
	}
	scheduler(false);
}

TaskHandle spawn(void (*to_call)(void*),void * args){
	if(!valid_rt){
		return -1;
	}
	void * stack = malloc(8000);
	Task * cur = get_current_task();	
	long tsk = -1;
	mutex_lock(&task_lock);	
	for(int i =0; i<TASK_COUNT; i++){
		if(!tasks[i].valid){
			tasks[i].valid = true;
			tasks[i].running = true;
			if(tasks[i].stack){
				free(tasks[i].stack);
			}
			tasks[i].stack = stack;
			tsk = i;
			break;
		}
	}
	if(tsk == -1){
		return tsk;
	}
	prev_running = current_task;
	current_task = tsk;	
	mutex_unlock(&task_lock);
	if(!setjmp(cur->buf)){
		task_spawn_asm((char*)stack+8000, to_call, args);
	}	
	mutex_lock(&task_lock);
	if(prev_running != current_task){
		tasks[prev_running].running =false;
		prev_running = -1;
	}
	mutex_unlock(&task_lock);
	return tsk;
}

void task_switch(Task*from, Task* to){		
	if(!setjmp(from->buf)){
		longjmp(to->buf, 0);
	}
	mutex_lock(&task_lock);
	if(prev_running != current_task){
		tasks[prev_running].running =false;
		prev_running = -1;
	}
	mutex_unlock(&task_lock);
	return;
}

void scheduler(bool is_done){
	if(!threads_should_continue){
		if(current_task == thread_id){
			return;
		}else{
			current_task = thread_id;
			longjmp(get_current_task()->buf,0);
		}
	}
	context_gc();
	pthread_mutex_lock(&task_lock);
	get_current_task()->valid = !is_done;
	int to_jump_to =-1;
	for(size_t j =1; j<TASK_COUNT+1; j++){
		long i = (j+current_task)%TASK_COUNT;
		if(i != thread_id && i<(long)THREAD_COUNT){
			continue;
		}
		if(tasks[i].valid&& (!tasks[i].running || i == current_task)){
			tasks[i].running = true;
			to_jump_to = i;
			break;
		}
	}		
	if(to_jump_to == -1){
		if(thread_id != 0){
			pthread_mutex_unlock(&task_lock);
			longjmp(tsaves[thread_id-1],0);
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
	thread_id = (size_t)args;
	current_task = thread_id;
	setjmp(tsaves[thread_id-1]);
	while(threads_should_continue){
		usleep(5000);
		yield();			
	}
	ready_to_finish[thread_id-1]= true;
	printf("thread %ld finished\n",thread_id);
	return 0;
}

void lolth_init(){	
	mutex_create(&task_lock);
	for(int i =0; i<TASK_COUNT; i++){
		tasks[i].valid = false;
	}
	mutex_lock(&task_lock);
	tasks[0].valid = true;
	tasks[0].running = true;
	threads_should_continue = true;
#ifdef WIN32
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);
	THREAD_COUNT = sys.dwNumberOfProcessors;
#else
	THREAD_COUNT = sysconf(_SC_NPROCESSORS_ONLN);
#endif
	printf("thread_count:%zu\n", THREAD_COUNT);
	if(THREAD_COUNT>THREAD_COUNT_MAX){
		THREAD_COUNT = THREAD_COUNT_MAX;
	}
	for(size_t i =1; i<THREAD_COUNT; i++){
		tasks[i].valid = true;
		tasks[i].running = true;
		pthread_create(&threads[i-1],0, thread_loop,(void*)(size_t)i);
	}

	valid_rt = true;
	mutex_unlock(&task_lock);
}

void lolth_finish(){
	threads_should_continue = false;
	bool done = false;
	while(!done){
		done = true;
		for(size_t i =0; i<THREAD_COUNT-1; i++){
			if(!ready_to_finish[i]){
				done = false;
				break;
			}
		}
	}
	for(size_t i = 0; i<THREAD_COUNT-1; i++){
		pthread_join(threads[i],0);
	}	
	valid_rt = false;
}

void lolth_await(TaskHandle handle){	
	if(handle == -1){
		return;
	}
	while(tasks[handle].valid){
		yield();		
	}
}

size_t lolth_read(FILE * file, char * buf, size_t count){
#ifdef WIN32
	return 0;
#else
	struct aiocb reader;
	memset(&reader, 0, sizeof(reader));
	reader.aio_buf= buf;
	reader.aio_nbytes = count;
	reader.aio_fildes = fileno(file);
	aio_read(&reader);
	int er = aio_error(&reader);
	while(er != 0){
		er = aio_error(&reader);
		yield();
	}
	return aio_return(&reader);
#endif
}

size_t lolth_write(FILE * file, char * buf, size_t count){
#ifdef WIN32
	return 0;
#else 
	struct aiocb reader;
	memset(&reader, 0, sizeof(reader));
	reader.aio_buf= buf;
	reader.aio_nbytes = count;
	reader.aio_fildes = fileno(file);
	aio_write(&reader);
	int er = aio_error(&reader);
	while(er >0){
		er = aio_error(&reader);
		yield();
	}
	return aio_return(&reader);
#endif
}

String lolth_read_to_string(Arena * arena, const char *fname){
	FILE * f = fopen(fname, "r");
	fseek(f, 0,SEEK_END);
	size_t count = ftell(f);
	fseek(f, 0, SEEK_SET);
	char * buff = arena_alloc(arena, count+1);
	lolth_read(f,buff, count);
	buff[count] =0;
	fclose(f);
	String out;
	out.items = buff;
	out.length= count;
	out.capacity= count;
	out.arena = arena;
	return out;
}
void lolth_write_to_file(const char* fname, Str to_write){
	FILE * f  = fopen(fname, "w");
	lolth_write_str(f, to_write);
	fclose(f);
}
void lolth_write_str(FILE * file, Str to_write){
	lolth_write(file, to_write.items, to_write.length);
}
void context_gc(){
	mutex_lock(&task_lock);
	for(int i =THREAD_COUNT; i<TASK_COUNT; i++){
		if(!tasks[i].valid && !tasks[i].running){
			if(!tasks[i].stack){
				continue;
			}
			free(tasks[i].stack);
			tasks[i].stack = 0;
		}
	}
	mutex_unlock(&task_lock);
}

#ifdef __x86_64__
#ifdef __MACH__
__asm(
	".intel_syntax noprefix\n"
	".global _task_spawn_asm\n"
	".extern _task_spawn_thunk\n"
	"_task_spawn_asm:\n"
	"	mov rsp, rdi\n"
	"	mov rbp, rdi\n"
	"       call _task_spawn_thunk\n"	
	"       ret\n"
	".att_syntax prefix\n"
);
#else
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
#endif
#else
#ifndef WIN32
__asm(
	"_task_spawn_asm:\n"
	"	mov sp, x0\n"
	"	mov bp, x0\n"
	"       call task_spawn_thunk\n"	
	"       ret\n"
);
#else
void __declspec(naked) task_spawn_asm(void* stack, void(*to_call)(void*), void* args) {
	__asm {
		mov esp,edi
		mov ebp, edi
		call task_spawn_thunk
		ret
	}
}
#endif
#endif



