#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include <stdint.h>

#define STACK_ORDER 2

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
struct context *__clone_context(struct context *old_ctx, struct context *new_ctx);
void switch_threads(struct thread *old_thread, struct thread *new_thread);
// Returns 0 for child, child tid for parent
uint64_t fork_thread(struct thread *thread);

#endif
