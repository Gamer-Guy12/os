#include <threading/tcb.h>
#include <asm.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

void kill_thread(void) {
  TCB_t* tcb = (TCB_t*) rdmsr(FS_MSR);

  tcb->state = THREAD_TERMINATED;
  
  execute_next_thread();
}

