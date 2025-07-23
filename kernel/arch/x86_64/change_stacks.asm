global change_stacks:function
extern kernel_secondary_start

section .text

change_stacks:
  mov rcx, 0xC0000100
  rdmsr
  shl rdx, 32
  or rax, rdx

  mov r8, [rax + 64]
  mov r8, [r8 + 8]

  mov cr3, r8
  
  mov rsp, 0xfffffe8000000000 - 8
  mov rbp, rsp

  call kernel_secondary_start

  .stop:
  cli
  hlt
  jmp .stop
