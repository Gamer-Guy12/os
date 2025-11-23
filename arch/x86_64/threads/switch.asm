global __switch_context
global __switch_pages
global __pages_null
global __cur_pages

; void __switch_context(struct context *old_ctx, struct context *new_ctx);
;
; struct context {
;   union {
;     void *rsp;
;     struct registers *regs;
;   };
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

  ret

; void __switch_pages(pt_t tables);
; RDI: new cr3
__switch_pages:
  mov cr3, rdi

  ret

; int __pages_null(pt_t tables);
; RDI: tables, RAX: 0 if it isn't null, 1 if it is
__pages_null:
  xor rax, rax
  cmp rdi, 0
  jnz .ret

  inc rax

.ret:
  ret

; pt_t __cur_pages(void);
; RAX: cr3
__cur_pages:
  mov rax, cr3
  ret

