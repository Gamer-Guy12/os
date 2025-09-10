#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void begin_thread(TCB_t *old) {
  TCB_t *tcb = TCB;
  tcb->state = THREAD_RUNNING;

  if (old->state == THREAD_RUNNING) {
    queue_thread(old, old->priority, &get_cls()->thread_queue);
  } else if (old->state == THREAD_TERMINATED) {
    delete_thread(old);
  }

  STI;
  enable_preemption();
  run_preemption();

  __asm__ volatile("jmp *%0" ::"r"(tcb->entry_point));
}

