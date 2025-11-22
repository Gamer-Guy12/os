global __switch_context

; void __switch_context(struct context *old_ctx, struct context *new_ctx);
;
; struct context {
;   union {
;     void *rsp;
;     struct registers *regs;
;   };
;   void *cr3;
; }
; RDI: old, RSI: new
__switch_context:
  push r15
  push r14
  push r13
  push r12
  push rbp
  push rbx
  pushf

  mov [rdi], rsp
  mov rsp, [rsi]

  popf
  pop rbx
  pop rbp
  pop r12
  pop r13
  pop r14
  pop r15

  mov r8, [rdi + 8]
  cmp r8, qword [rsi + 8]
  je .ret

  cmp qword [rsi + 8], 0
  jz .ret

  mov rax, [rsi + 8]
  mov cr3, rax

.ret:
  ret

