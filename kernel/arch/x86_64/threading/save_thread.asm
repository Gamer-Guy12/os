global save_thread

save_thread:
  call save_actual_thread
  ret

; size_t save_thread(void);
; return 0
save_actual_thread:
  cli
  ; RAX Now contains the current TCB
  mov rcx, 0xC0000100
  rdmsr
  shl rdx, 32
  or rax, rdx

  ; RIP will be stored at the top of the stack
  mov r10, [rsp]

  push r15
  push r14
  push r13
  push r12
  push rbp
  push rbx
  pushf

  ; In the tcb rsp0 is stored at offset 32
  mov [rax + 32], rsp

  ; The registers are at offset 8
  mov [rax + 8], rsp

  ; The xsave page pointer is at offset 96
  mov r11, [rax + 96]

  push rax
  push rdx
  mov rax, -1
  mov rdx, rax
  xrstor [r11]
  pop rdx
  pop rax

  ; This pushes rip onto the stack to use for ret
  push r10

  ; This needs to return 0 so clear rax
  xor rax, rax
  pop rcx

  ret

