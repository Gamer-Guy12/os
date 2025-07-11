global change_stacks:function
extern kernel_secondary_start

change_stacks:
  
  mov rsp, 0xfffffe8000000000
  mov rbp, rsp
  
  call kernel_secondary_start

  .stop:
  cli
  hlt
  jmp .stop
