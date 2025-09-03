#pragma once
#include <stdlib.h>
#include <string.h>
typedef enum{
	PollFinished, 
	PollWait,
}Poll;
#define ASYNC_FINISHED -1
#define ASYNC_START 0
#define async(out, name, args,vars,instructions)\
typedef struct name##Args args name##Args;\
typedef struct name##Vars vars name##Vars;\
typedef struct name##Future{out returned; name##Args ag; name##Vars vs;long state;Poll(*poll)(struct name##Future*f, name##Args * a, name##Vars * vs);}name##Future;\
Poll name##Poll(name##Future* fut, name##Args*a, name##Vars*v){\
	switch(fut->state){\
		case ASYNC_FINISHED: \
			return PollFinished;\
		case ASYNC_START:\
		default:\
		instructions\
	}\
}\
Poll name##InterfacePoll(void *ptr){\
	return \
}\
name##Future name (name##Args ag){\
	name##Future ret;\
	memset(&ret, 0, sizeof(ret));\
	ret.ag = ag;\
	ret.state =ASYNC_START;\
	ret.poll = name##Poll;\
	return ret;\
}
#define await(f, args) fut->state = __LINE__; case __LINE__: if((f).poll(&f, &f.ag, &f.s)== PollWait) return PollWait;
#define await_imm(f, v,args...) vars->v = f(args); await(vs->v);
#define await_eq(l,f, args) fut->state = __LINE__; case __LINE__: if((f).poll(&f, &f.ag, &f.vs)== PollWait) return PollWait; l = f.out;
#define yield() fut->state = __LINE__; return PollWait;case __LINE__:(void)(fut); 
#define poll(f) ((f).poll(&f, &f.ag, &f.vs) == PollWait)
#define spawn(f,args...) spawn_future(f(args...))
