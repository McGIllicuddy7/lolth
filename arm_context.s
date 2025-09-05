
	/*
typedef struct {
	void * bp;
	void * sp;
	void * ret;
	bool runnable;
	void * stack_ptr;
	void * waiter_data;
	bool (*wait)(void*);
}context_t;
*/
.global _context_spawn ;void context_spawn(context_t * prev, context_t * to_switch_to, void(*to_run)(void* args), void * args)
.global _context_switch ; void context_switch(context_t * prev, context_t * to_switch_to
.global _debug_stack_ptr ;void * debug_stack_ptr ()
.global _debug_frame_ptr ; void * debug_frame_ptr()
.extern _context_spawn_thunk; 

_debug_stack_ptr:
	mov x0, sp
	ret

_debug_frame_ptr:
	mov x0, fp
	ret

_context_spawn:	
	str fp, [x0]
	mov x10, sp
	str x10, [x0, #8]
	str lr, [x0, #16]
	mov x10, #0
	str x10, [x1, #16]
	ldr fp, [x1]
	ldr x10, [x1, #8]
	mov sp, x10
	bl _context_spawn_thunk
	ret
_context_switch:
	str fp, [x0]
	mov x10, sp
	str x10, [x0, #8]
	str lr, [x0, #16]
	ldr fp, [x1, #0]
	ldr x10, [x1, #8]
	mov sp, x10
	ldr lr, [x1, #16]
	ret
