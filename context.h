#pragma once
#include <stdio.h>
typedef struct waiter_t{
	void *data;
	void (*aborter)(struct waiter_t*);
	bool (*check)(struct waiter_t *);
}waiter_t;
typedef struct{
	void *bp;
	void *sp;
	void * ret;
	void * stack_ptr;
	bool runnable;
	bool active;
	bool awaiting;
	waiter_t waiter;
} context_t;
typedef long task_handle_t;
void context_spawn(context_t * prev, context_t * to_switch_to, void (* to_run)(void*),void * args);
void context_switch(context_t * prev, context_t * to_switch_to);
void * debug_stack_ptr();
void * debug_frame_ptr();
void lolth_init();
task_handle_t task_spawn(void(*to_run)(void*), void*args);
void yield();
void task_await(task_handle_t task);
void task_wait_for(waiter_t waiter);
