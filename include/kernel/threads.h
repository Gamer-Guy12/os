#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include <stdint.h>

enum thread_state {
  THREAD_RUNNING,
  THREAD_WAITING,
  THREAD_TERMINATED,
  THREAD_STARTING,
  THREAD_READY
};

struct thread {
  struct context context;
  uint64_t tid;
  pt_t page_tables;
  enum thread_state state;
  // Last cpu this was run on
  uint32_t cpu_id;
};

void __switch_context(struct context *old_ctx, struct context *new_ctx);
void __create_context(struct context *context);
void switch_threads(struct thread *old_thread, struct thread *new_thread);

#endif
