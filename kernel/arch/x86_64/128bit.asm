global add_128
global sub_128
global equals_128
global less_than_128
global mul_128
global div_128

; struct {
;   uint64_t lower; 
;   uint64_t upper;
; }

; uint128_t add_128(uint128_t lhs, uint128_t rhs);

; RDI contains lhs_lower, RSI contains lhs_upper
; RDX contains rhs_lower, RCX contains rhs_upper
; At the end
; RAX should contain lower, RDX should contain upper
add_128:
  ; RDI contains the new lower
  add rdi, rdx

  ; RDX contains lhs_upper
  ; RCX contains rhs_upper
  mov rdx, rsi
  adc rdx, rcx

  ; Since RDI contains the lower part which needs to be in RAX
  mov rax, rdi

  ret

; uint128_t sub_128(uint128_t lhs, uint128_t rhs);

; RDI contains lhs_lower, RSI contains lhs_upper
; RDX contains rhs_lower, RCX contains rhs_upper
; At the end
; RAX should contain lower, RDX should contain upper
sub_128:
  ; The code here should be similar to the code in add_128

  cmp rdi, rdx
  sbb rsi, rcx
  sub rdi, rdx
  mov rax, rdi
  mov rdx, rsi 
  ret

; bool equals_128(uint128_t lhs, uint128_t rhs);

; RDI contains lhs_lower, RSI contains lhs_upper
; RDX contains rhs_lower, RCX contains rhs_upper
; At the end
; RAX should contain 1 for success or 0 for failure
equals_128:

  cmp rdi, rdx
  jne .fail

  cmp rsi, rcx
  jne .fail

  mov rax, 1
  ret

.fail:
  mov rax, 0
  ret

; bool less_than_128(uint128_t lhs, uint128_t rhs);
; RDI contains lhs_lower, RSI contains lhs_upper
; RDX contains rhs_lower, RCX contains rhs_upper
; At the end
; RAX should contain 1 for success or 0 for failure
less_than_128:
  ; The carry flag is set when a subtraction value goes negative
  sub rsi, rcx
  ; If the higher half is smaller then it succeded
  jc .success
  ; If they aren't equal then that means that higher half must be bigger so it failed
  jne .fail

  sub rdi, rdx
  ; If the lower half is bigger or equal then it failed
  jnc .fail

.success:
  mov rax, 1
  ret

.fail:
  mov rax, 0
  ret

; uint128_t mul_128(uint64_t lhs, uint64_t rhs);
; RDI contains lhs
; RSI contains rhs
; At the end RDX should contain upper and RAX should contain lower
mul_128:
  mov rax, rdi
  ; The result goes into RAX:RDX
  mul rsi
  ret

; uint128_t div_128(uint128_t lhs, uint64_t rhs);
; RDI contains lhs_lower, RSI contains lhs_upper
; RDX contains rhs
; At the end RDX should contain upper and RAX should contain lower
div_128:
  mov rax, rsi
  mov rcx, rdx
  mov rdx, 0
  
  ; The higher half is in rax and the remainder in rdx
  div rcx

  ; RDX alr contains the correct value so just move around rdi into rax

  ; Temporary storage
  mov r8, rax
  mov rax, rdi
  div rcx

  ; R8 has the higher half and rax has the lower half
  mov rdx, r8
  ret

