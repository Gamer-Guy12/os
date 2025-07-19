#include <asm.h>
#include <libk/kio.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

void run_next_thread(void) {
  TCB_t *tcb = (TCB_t *)rdmsr(FS_MSR);

  queue_thread(tcb);

  TCB_t *next = pop_thread();

  if (next == NULL) {
    while (1) {
    }
  }

  while (next->state == THREAD_TERMINATED) {
    // Delete it

    next = pop_thread();

    if (next == NULL) {
      while (1) {
      }
    }
  }

  next->state = THREAD_RUNNING;
  swap_threads(next);
}
