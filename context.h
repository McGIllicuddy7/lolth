#pragma once
#include <stdio.h>
//jump orientated programming
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
	bool running;
	bool active;
	bool awaiting;
	waiter_t waiter;
} context_t;
typedef long task_handle_t;
void context_spawn(context_t * prev, context_t * to_switch_to, void (* to_run)(void*),void * args);
void context_switch(context_t * prev, context_t * to_switch_to);
void yield();
task_handle_t task_spawn(void (*func)(void *), void * args);
void task_await(task_handle_t handle);
void lolth_init();
