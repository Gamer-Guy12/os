#include "kernel/threads.h"

void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  new_thread->state = THREAD_RUNNING;

  __switch_page_tables(new_thread->page_tables);
  __switch_context(&old_thread->context, &new_thread->context);
}

