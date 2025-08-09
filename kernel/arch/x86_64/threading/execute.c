#include <asm.h>
#include <cls.h>
#include <libk/kio.h>
#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void run_next_thread(void) {
  TCB_t *tcb = TCB;
  tcb->rb_node.value++;

  TCB_t *next = pop_thread();

  // If there is nothign to do then keep going
  if (next == NULL) {
    return;
  }

  // The thread that was just executed will be in rax when this thread resumes
  // which means it gets treated as a return value Therefore the thread that was
  // just preempted can be queued
  TCB_t *old = NULL;

  if (next->state == THREAD_STARTING) {
    old = start_thread(next);
  } else if (next->state == THREAD_RUNNING) {
    old = switch_threads(next);
  }

  if (old->state == THREAD_RUNNING) {
    queue_thread(old, old->priority);
  } else if (old->state == THREAD_TERMINATED) {
    delete_thread(old);
  }
}
