global swap_threads

; RDI contains a pointer to the TCB
; FS contains the pointer to the current TCB
; FS BASE MSR is 0xC0000100
swap_threads:
  cli
    ; RAX contains the pointer to the current TCB
  mov rcx, 0xC0000100
  rdmsr
  shl rdx, 32
  or rax, rdx

  ; Save old registers
  ; R8 contains the addr to the register struct (offset of the struct is 8)
  mov r8, [rax + 8]

  mov [r8 + 0], rax
  mov [r8 + 8], rbx
  mov [r8 + 16], rcx
  mov [r8 + 24], rdx
  mov [r8 + 32], rbp
  mov [r8 + 40], rdi
  mov [r8 + 48], rsi
  mov [r8 + 56], r8
  mov [r8 + 64], r9
  mov [r8 + 72], r10
  mov [r8 + 80], r11
  mov [r8 + 88], r12
  mov [r8 + 96], r13
  mov [r8 + 104], r14
  mov [r8 + 112], r15

; R10 contains rip
  pop r10

  ; Save segments
  xor rbx, rbx

  mov bx, cs
  mov [r8 + 120], rbx
  
  mov bx, ds
  mov [r8 + 128], rbx

  mov bx, es
  mov [r8 + 136], rbx

  mov bx, ss
  mov [r8 + 144], rbx

  ; Save Rflags
  pushf
  pop rbx

  mov [r8 + 152], rbx

  ; Save RSP and RIP (rsp0 and rip0)
  mov [rax + 32], rsp 
  mov [rax + 48], r10
  
  ; Save the new TCB pointer (its in rdi)
  mov rax, rdi
  mov rdx, rdi
  shr rdx, 32
  mov rcx, 0xC0000100
  wrmsr

  ; Load new page tables
  ; R8 contains the new cr3
  ; PCB offset is 64 bytes
  ; CR3 is an offset of 8 bytes
  mov r8, [rdi + 64]
  mov r8, [r8 + 8]

  mov cr3, r8

  ; Load new registers
  ; R8 contains the address to the registers
  mov r8, [rdi + 8]

  mov rax, [r8 + 0]
  mov rbx, [r8 + 8]
  mov rcx, [r8 + 16]
  mov rdx, [r8 + 24]
  mov rbp, [r8 + 32]
  ; RDI will be loaded at the end
  mov rsi, [r8 + 48]
  ; R8 Will be loaded at the end 
  mov r9, [r8 + 64]
  mov r10, [r8 + 72]
  mov r11, [r8 + 80]
  mov r12, [r8 + 88]
  mov r13, [r8 + 96]
  mov r14, [r8 + 104]
  mov r15, [r8 + 112]

  ; Load new segment registers
  xor rbx, rbx

  mov rbx, [r8 + 128]
  mov ds, bx

  mov rbx, [r8 + 136]
  mov es, bx

  ; Load new rsp
  ; mov rsp, [rdi + 32]

  ; Load new ss
  push qword [r8 + 144]
  
  ; Load new rsp
  push qword [rdi + 32]

  ; Load rflags
  mov rbx, [r8 + 152]
  push rbx

  ; Load CS
  mov rbx, [r8 + 120]
  push rbx

  ; Load RIP
  mov rbx, [rdi + 48]
  push rbx

  ; Load rdi
  mov rdi, [r8 + 40]

  ; Load r8
  mov r8, [r8 + 56]
  
  sti
  iretq

