#include "kernel/threads.h"

void schedule(void) {
  struct thread *thread = get_cur_thread();
  struct thread *new_thread = pop_thread();

  if (new_thread == NULL || new_thread == thread)
    return;

  switch_threads(thread, new_thread);
}

void terminate(int code) {
  struct thread *thread = get_cur_thread();
  thread->exit_code = code;
  thread->state = THREAD_TERMINATED;

  while (1)
    schedule();
}
