#include <asm.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void kill_cur_thread(void) {
  TCB->state = THREAD_TERMINATED;

  while (1) {
    run_next_thread();
  }
}
