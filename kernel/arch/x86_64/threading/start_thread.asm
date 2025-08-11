global start_thread
extern begin_thread

start_thread:
  call start_thread_actual
  ret

; TCB_t* start_thread(TCB_t* thread);
; Says it returns the old thread
; It actually passes in the old thread into rdi
start_thread_actual:
  cli
  ; RAX Now contains the current TCB
  mov rcx, 0xC0000100
  rdmsr
  shl rdx, 32
  or rax, rdx

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

  ; PCB is at offset 64
  mov r10, [rdi + 64]
  ; Cr3 is at offset 8
  mov r8, [r10 + 8]
  mov cr3, r8

  mov rcx, 0xC0000100
  mov rax, rdi
  mov rdx, rax
  shr rdx, 32
  wrmsr

  ; Setup IRETQ stack
  ; SS
  ; RSP
  ; RFLAGS
  ; CS
  ; RIP

  ; Push SS
  push 0x10 

  ; Push RSP (offset 32)
  push qword [rdi + 32]

  ; Push rflags
  push 0x202

  ; Push CS
  push 0x8

  ; Push Entry Point (begin thread)
  push qword begin_thread

  ; Make sure segments are correct
  mov ax, 0x10
  mov ds, ax
  mov es, ax

  ; Zero out general purpose registers
  ; RAX contains the return value
  mov rax, 0
  mov rbx, 0
  mov rcx, 0
  mov rdx, 0
  mov rdi, r9
  mov rsi, 0
  mov rbp, 0
  mov r8,  0
  mov r9,  0
  mov r10, 0
  mov r11, 0
  mov r12, 0
  mov r13, 0
  mov r14, 0
  mov r15, 0

  iretq

