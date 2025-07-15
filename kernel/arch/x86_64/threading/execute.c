#include <threading/tcb.h>
#include <threading/threading.h>
#include <asm.h>

void execute_next_thread(void) {
  TCB_t* tcb = pop_thread();
  TCB_t* cur_tcb = (TCB_t*)rdmsr(0xC0000100);
  queue_thread(cur_tcb);

  while (tcb->state == THREAD_TERMINATED) {
    delete_thread(tcb);
    tcb = pop_thread();
  }

  tcb->state = THREAD_RUNNING;

  swap_threads(tcb);
}

