#include <asm.h>
#include <threading/threading.h>
#include <threading/tcb.h>

#define FS_MSR 0xC0000100

void kill_cur_thread(void) {
  TCB_t* tcb = (TCB_t*)rdmsr(FS_MSR);
  tcb->state = THREAD_TERMINATED;

  run_next_thread();
}

