global __do_stack_switch

; void __do_stack_switch(void (*entry)(void *new_stack), void *stack, size_t stack_size);
; RDI: new entry point, RSI: new stack, RDX: stack_size
; at return: RDI: new stack
__do_stack_switch:
  ; Save the entry point in rcx
  mov rcx, rdi
  ; Copy the new stack into r8 and rdi
  mov rdi, rsi
  mov r8, rdi
  ; Move r8 up so that it can be the stack pointer
  add r8, rdx
  ; Load rsp with the new stack pointer
  mov rsp, r8
  mov rbp, rsp
  ; Save a blank rbp to the stack to keep track of stack frames
  push 0

  jmp rcx

