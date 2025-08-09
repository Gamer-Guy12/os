global cmpxchg16b:function

; uint64_t cmpxchg16b(void *dest, uint64_t value, uint64_t new);

; RDI contains the destination pointer
; RSI contains the value
; RDX contains the new value
cmpxchg16b:
  push rbx

  mov rcx, rdx
  mov rbx, rdx

  mov rax, rsi
  mov rdx, rsi

  lock cmpxchg16b [rdi]

  ; If sucessful the zero flag will be set
  jnz .failure
  
  ; Return value is stored in RAX
  mov rax, 1
  jmp .success

.failure:
  mov rax, 0

.success:

  pop rbx

  ret

