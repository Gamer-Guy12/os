#include "kernel/threads.h"

void schedule(void) {
  struct thread *thread = pop_thread();
  struct thread *cur_thread = get_cur_thread();

  if (thread == NULL) {
    return;
  }

  if (thread == cur_thread) return;

  switch_threads(cur_thread, thread);
}

void terminate(void) {
  struct thread* thread = get_cur_thread();
  thread->state = THREAD_TERMINATED;

  while (1)
    schedule();
}

