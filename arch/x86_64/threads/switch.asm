global swap_regs

; void *swap_regs(void *rsp);
; RDI: new rsp, RAX: returned (old) rsp
swap_regs:
  push r15
  push r14
  push r13
  push r12
  push rbp
  push rbx
  pushf

  mov rax, rsp
  mov rsp, rdi

  popf
  pop rbx
  pop rbp
  pop r12
  pop r13
  pop r14
  pop r15

  ret

