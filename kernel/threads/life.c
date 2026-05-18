#include "kernel/kprintf.h"
#include "kernel/threads.h"
#include "util.h"

struct event death_event;

void init_thread_lifecycle(void) { event_create(&death_event, true); }

void schedule(void) {
  // AP {
  //   return;
  // }

  struct thread *thread = get_cur_thread();
  struct thread *new_thread = pop_thread();

  if (new_thread == NULL || new_thread == thread) {
    // kprintf("quit\n");
    return;
  }

  switch_threads(thread, new_thread);
}

void terminate(int code) {
  RMEMB();
  struct thread *thread = get_cur_thread();
  thread->exit_code = code;
  thread->state = THREAD_TERMINATED;
  WMEMB();

  while (1)
    schedule();
}

int wait_thread(tid_t thread) {
  event_wait(&death_event, thread);
  return thread_id(thread)->exit_code;
}

void __trigger_death_event(tid_t thread, int code) {
  event_trigger(&death_event, thread);
}
