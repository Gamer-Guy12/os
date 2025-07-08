global change_gdt

change_gdt:

  cli

  xor rax, rax

  mov ax, 0x10
  mov ds, ax
  mov es, ax

  ret
