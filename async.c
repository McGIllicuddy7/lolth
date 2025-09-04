#include "async.h"
#include <stdio.h>
static AsyncContext create_context(){
	AsyncContext out = {};
	return out;
}
void lolth_spawn_task(AsyncContext * asyncon,AsyncTask task){
	AsyncTask * tsk= (AsyncTask*)malloc(sizeof(task));	
	task.next = asyncon->tasks;
	*tsk = task;
	asyncon->tasks = tsk;
}
void lolth_spawn_blocking(AsyncTask task){
	AsyncContext context = create_context();
	lolth_spawn_task(&context, task);
	while(context.tasks){
		AsyncTask * prev = 0;
		AsyncTask * ptr = context.tasks;
		while(ptr){
			if(ptr->poll(&context,ptr->data) == PollFinished){
				printf("polled finished\n");
				if(prev){
					prev->next = ptr->next;
				}else{
					context.tasks= ptr->next;
				}
				AsyncTask *old = ptr;
				ptr = ptr->next;
				free(old->data);
				free(old);
			}else{
				printf("polled\n");
				prev = ptr;
				ptr = ptr->next;
			}
		}
	}
}


