#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
	PollFinished, 
	PollWait,
}Poll;
#define ASYNC_FINISHED -1
#define ASYNC_START 0
struct AsyncContext;
typedef struct AsyncTask{
	Poll (*poll)(struct AsyncContext * context,void*);
	void * data;		
	struct AsyncTask * next;
}AsyncTask;
typedef struct AsyncContext{
	AsyncTask * tasks;
} AsyncContext;
#define async(out, name, args,vars,instructions)\
typedef struct name##Args args name##Args;\
typedef struct name##Vars vars name##Vars;\
typedef struct name##Future{out returned; name##Args ag; name##Vars vs;long state;Poll(*poll)(AsyncContext *asyncon,struct name##Future*f, name##Args * a, name##Vars * vs);}name##Future;\
Poll name##Poll(AsyncContext * asyncon,name##Future* fut, name##Args*a, name##Vars*v){\
	(void)a;\
	(void)v;\
	switch(fut->state){\
		case ASYNC_FINISHED: \
			return PollFinished;\
		case ASYNC_START:\
		default:\
		instructions\
	}\
}\
Poll name##InterfacePoll(AsyncContext * asyncon,void *ptr){\
	name##Future* fut = (name##Future*)ptr;\
	return (fut->poll)(asyncon,fut, &fut->ag, &fut->vs);\
}\
name##Future name##_call(name##Args ag){\
	name##Future ret;\
	memset(&ret, 0, sizeof(ret));\
	ret.ag = ag;\
	ret.state =ASYNC_START;\
	ret.poll = name##Poll;\
	return ret;\
}\
AsyncTask name##virtual(name##Args ag) {\
	name##Future * ptr = (name##Future*)malloc(sizeof(name##Future));\
	*ptr = name##_call(ag);\
	AsyncTask tmp;\
	tmp.poll = name##InterfacePoll;\
	tmp.data = ptr;\
	return tmp;\
}\

#define await(f, args) fut->state = __LINE__; case __LINE__: if((f).poll(&f, &f.ag, &f.s)== PollWait) return PollWait;
#define await_imm(f, v,args...) vars->v = f(args); await(vs->v);
#define await_eq(l,f, args) fut->state = __LINE__; case __LINE__: if((f).poll(&f, &f.ag, &f.vs)== PollWait) return PollWait; l = f.out;
#define lolth_yield() fut->state = __LINE__; return PollWait;case __LINE__:(void)(fut); 
#define lolth_poll(f) ((f).poll(asyncon,&f, &f.ag, &f.vs) == PollWait)
#define async_spawn(name, args...) lolth_spawn_task(asyncon,name##virtual(args))
void lolth_spawn_task(AsyncContext * asyncon,AsyncTask task);
#define spawn_blocking(name, args...) lolth_spawn_blocking(name##virtual(args))
void lolth_spawn_blocking(AsyncTask task);

