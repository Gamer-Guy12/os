#include "kernel/threads.h"

void schedule(void) {
  struct thread *thread = pop_thread();
  struct thread *cur_thread = get_cur_thread();

  if (thread == NULL) {
    //     kprintf("Here\n");
    return;
  }

  if (thread == cur_thread)
    return;

  // Because there is always an idle thread, it is gaurenteed to swap away
  // unless something goes terribly wrong
  switch_threads(cur_thread, thread);
}

void terminate(int code) {
  struct thread *thread = get_cur_thread();
  thread->state = THREAD_TERMINATED;
  thread->exit_code = code;

  while (1)
    schedule();
}
