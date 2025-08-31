
.intel_syntax 
.global context_spawn
.global context_switch
.global debug_stack_ptr
.global debug_frame_ptr
.extern context_spawn_thunk
.extern _printf

debug_stack_ptr:
	mov %rax, %rsp
	ret
debug_frame_ptr:
	mov %rax, %rbp
	ret

context_spawn:	
	push %rbp
	mov %rbp,%rsp
	mov [%rax], %rbp
	mov [%rax+8], %rsp
	mov %r10, [%rbp-8]
	mov [%rax+16], %r10
	mov %rbp, [%rbx]
	mov %rsp, [%rbx+8]
	call context_spawn_thunk
	mov %rsp, %rbp
	pop %rbp
	ret

context_switch:
	push %rbp
	mov %rbp,%rsp
	mov [%rax], %rbp
	mov [%rax+8],%rsp
	mov %r10, [%rbp-8]
	mov [%rax+16],%r10
	mov %rbp, [%rbx]
	mov %rsp, [%rbx+8]
	mov %rsp, %rbp
	pop %rbp
	mov %r10, [%rbx+16]
	jmp %r10
	ret

