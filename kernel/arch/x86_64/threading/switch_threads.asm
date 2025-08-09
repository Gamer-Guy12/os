global switch_threads

switch_threads:
  call switch_threads_actual
  ret

; TCB_t* switch_threads(TCB_t* thread);
; Returns the old tcb
switch_threads_actual:
  ; RAX Now contains the current TCB
  cli
  mov rcx, 0xC0000100
  rdmsr
  shl rdx, 32
  or rax, rdx

  ; R9 also contains it
  mov r9, rax

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

  ; Offset 32 in rdi contains the stack
  mov rsp, [rdi + 32]

  ; When saving rax contains the lower 32 bits while rdx contains the upper 32 bits
  mov rcx, 0xC0000100
  mov rax, rdi
  mov rdx, rax
  shr rdx, 32
  wrmsr

  ; Offset 96 into rdi contains the sse state
  mov r11, [rdi + 96]
  mov rax, -1
  mov rdx, rax
  xsave [r11]

  ; Offset 64 contains the pcb
  mov r11, [rdi + 64]
  ; Offset 8 of the pcb contains cr3
  mov r8, [r11 + 8]
  mov cr3, r8

  ; Pop Rflags
  popf

  ; Pop GPRs
  pop rbx
  pop rbp
  pop r12
  pop r13
  pop r14
  pop r15

  mov rax, r9
  
  ; Pop off new rip
  sti
  ret

