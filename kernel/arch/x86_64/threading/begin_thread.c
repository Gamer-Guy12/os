#include <asm.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void begin_thread(TCB_t* old) {
  TCB->state = THREAD_RUNNING;

  if (old->state == THREAD_RUNNING) {
    queue_thread(old, old->priority);
  }

  __asm__ volatile("jmp %0" :: "r"(TCB->entry_point));
}

