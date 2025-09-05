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
	context.to_run = context.tasks;
	context.tasks =0;
	while(context.to_run){
		AsyncTask * prev = 0;
		AsyncTask * ptr = context.to_run;
		while(ptr){
			if(ptr->poll(&context,ptr->data) != PollFinished){
				AsyncTask * next = ptr->next;
				ptr->next = context.tasks;
				context.tasks = ptr;
				ptr = next;
			}else{
				ptr = ptr->next;
			}
		
		}
		context.to_run = context.tasks;
		context.tasks = 0;
	}
}


void begin_async_read(struct aiocb * out,FILE * file, char * buf,size_t count){
	memset(out, 0, sizeof(*out));
	out->aio_buf = buf;
	out->aio_nbytes = count;
	out->aio_fildes = fileno(file);
	aio_read(out);
}


