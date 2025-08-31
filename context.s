
.intel_syntax noprefix
.global context_spawn
.global context_switch
.global debug_stack_ptr
.global debug_frame_ptr
.extern context_spawn_thunk
.extern _printf

debug_stack_ptr:
	mov rax, rsp
	ret
debug_frame_ptr:
	mov rax, rbp
	ret
//prev is in rdi, to_switch to is in rsi
context_spawn:	
	push rbp
	mov rbp,rsp
	push r11
	mov r11, rsp		
	//stuff
	mov [rdi], rbp
	mov [rdi+8], rsp
	mov rbp, [rsi]
	mov rsp, [rsi+8]
	push r11
	push r12
	call context_spawn_thunk
	//end stuff
	pop r12
	pop r11
	mov rsp, r11 
	mov rbp, r11
	pop r11	
	pop rbp
	ret

context_switch:
	push rbp
	mov rbp,rsp
	//stuff
	mov [rdi], rbp
	mov [rdi+8], rsp
	mov rbp, [rsi]
	mov rsp, [rsi+8]
	//end stuff
	mov rsp, rbp
	pop rbp
	ret
	














