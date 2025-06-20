global extend
extern extend_ret

extend:
  ; This prolly inefficient but idgaf

  push rax
  push rbx

  ; Enable mmx
  ; Clear EM in cr0
  mov rax, cr0
  mov rbx, (1 << 2)
  not rbx
  and rax, rbx

  ; Set MP bit in cr0
  or rax, (1 << 1)

  ; Clear TS bit in cr0
  mov rbx, (1 << 3)
  not rbx
  and rax, rbx
  
  mov cr0, rax

  ; Enable sse
  mov rax, cr4

  or rax, (1 << 9)
  or rax, (1 << 10)

  mov cr4, rax

  pop rbx
  pop rax
  jmp extend_ret
