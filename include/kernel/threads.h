#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include <stdint.h>

enum thread_state {
  THREAD_RUNNING,
  THREAD_WAITING,
  THREAD_TERMINATED,
  THREAD_STARTING
};

struct thread {
  struct context context;
  uint64_t tid;
  pt_t page_tables;
  enum thread_state state;
  // Last cpu this was run on
  uint32_t cpu_id;
};

// Do not call
void __switch_context(struct context *old_ctx, struct context *new_ctx);
// Do not call
void __switch_page_tables(pt_t new_tables);
// Do not call
void __create_context(void (*entry)(void), struct context *context);
void switch_threads(struct thread *old_thread, struct thread *new_thread);

#endif
