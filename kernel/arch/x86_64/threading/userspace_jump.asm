global userspace_jump:function

; extern void userspace_jump(tss_t *tss);
userspace_jump:
  cli

  ; RAX contains the pointer to the current TCB
  mov rcx, 0xC0000100
  rdmsr
  shl rdx, 32
  or rax, rdx

  ; RDX contains the pointer to the CLS
  push rax
  mov rcx, 0xC0000101
  rdmsr
  shl rdx, 32
  or rdx, rax
  pop rax

  ; Offset 0 into the cls contains the place to save the TCB
  mov [rax], rdx

  ; Load the new gs
  swapgs

  ; Store old rip into the tcb
  pop r10

  mov [rax + 48], r10

  ; Store old rsp
  mov [rax + 32], rsp

  ; Push stack segment (user data segment: 0x18 | ring 3: 0x3)
  push 0x1B

  ; Push stack pointer
  push qword [rax + 24]

  ; Push rflags (they are cleared except for the interrupt flag which is set to enable them)
  push (1 << 9)

  ; Push code segment (user code segment: 0x20 | ring 3: 0x3)
  push 0x23

  ; Push rip
  push qword [rax + 40]

  iretq

