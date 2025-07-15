#include <threading/tcb.h>
#include <threading/threading.h>

void execute_next_thread(void) {
  TCB_t* tcb = pop_thread();

  if (tcb->state == THREAD_TERMINATED) delete_thread(tcb);

  tcb->state = THREAD_RUNNING;

  swap_threads(tcb);
}

