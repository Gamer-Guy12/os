#include <asm.h>
#include <stdint.h>
#include <threading.h>
#include <threading/tcb.h>
#include <threading/threading.h>

static void handler(void *data) {
  TCB_t *thread = data;

  thread->state = THREAD_RUNNING;
  schedule_thread(thread, thread->priority == TP_IO ? TP_IO : TP_HIGH);
}

void sleep_for(uint64_t ms) {
  disable_preemption();
  schedule_event(ms, TCB, handler);

  TCB->state = THREAD_WAITING;
  enable_preemption();
  run_next_thread();
}
