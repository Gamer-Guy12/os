; void __switch_context(struct context *old, struct context *new);
; RDI: old
; RSI: new
global __switch_context
__switch_context:
  push r15
  push r14
  push r13
  push r12
  push rbp
  push rbx
  pushf

  ; Store into regs
  mov [rdi], rsp
  mov rsp, [rsi]

  popf
  pop rbx
  pop rbp
  pop r12
  pop r13
  pop r14
  pop r15
  ret

; void __switch_pages(pt_t pages);
; RDI: new cr3 value
global __switch_pages
__switch_pages:
  mov cr3, rdi
  ret

; Returns a null page table
; pt_t __pages_null(void);
global __pages_null
__pages_null:
  mov rax, 0
  ret

; Gets the current page tables
; pt_t __cur_pages(void);
global __cur_pages
__cur_pages:
  mov rax, cr3
  ret

; Switches to a new stack at the entry
; extern void __do_stack_switch(void (*entry)(void *), void *stack, size_t stack_size);
; RDI: new entry point
; RSI: stack address
; RDX: stack size
global __do_stack_switch
__do_stack_switch:
  ; Shift some registers for more space
  mov rax, rdi
  mov rdi, rsi
  ; Add the size to the address to get the rsp value
  add rsi, rdx
  mov rsp, rsi
  ; Used to mark the bottom of the stack
  mov rbp, 0
  ; Go to the entry point
  jmp rax

