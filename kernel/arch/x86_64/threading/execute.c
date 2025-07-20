#include <asm.h>
#include <libk/kio.h>
#include <stdbool.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

bool bad_tcb(TCB_t *tcb) {
  if (tcb == NULL)
    return true;
  if (tcb->state == THREAD_TERMINATED || tcb->state == THREAD_WAITING)
    return true;

  return false;
}

void run_next_thread(void) {
  TCB_t *tcb = (TCB_t *)rdmsr(FS_MSR);
  // This means that if there are no threads this thread could be queued again
  queue_thread(tcb);
  TCB_t *next = pop_thread();

  while (bad_tcb(next)) {
    if (next == NULL) {
      next = pop_thread();
    } else if (next->state == THREAD_TERMINATED) {
      if (next == tcb) {
        kio_printf("Ja\n");
        // Faulting if u try to delete urself (suicide kills!)
        queue_thread(next);
        continue;
      }

      PCB_t* pcb = next->pcb;
      delete_thread(next);
      if (pcb->tcbs == NULL) {
        delete_process(pcb);
      }

      next = pop_thread();
    } else {
      // Handle waiting
    }
  }

  next->state = THREAD_RUNNING;
  swap_threads(next);
}

