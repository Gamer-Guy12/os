#include "kernel/threads.h"
#include <stdbool.h>

void terminate(int code) {
  struct thread *thread = get_cur_thread();
  thread->exit_code = code;
  thread->state = THREAD_TERMINATED;
  while (true)
    schedule();
}

void schedule(void) {
  struct thread *cur = get_cur_thread();

retry:;
  struct thread *next = pop_thread();
  do_preemption();
  if (next != cur && next != NULL) {
    switch_threads(cur, next);
  }

  if (cur->state != THREAD_RUNNING && cur->state != THREAD_READY)
    goto retry;
}

void preempt(void) { schedule(); }
