#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "util.h"
#include <stdint.h>

#define STACK_ORDER 2

enum thread_state {
  // Currently running
  THREAD_RUNNING,
  // Currently waiting
  THREAD_WAITING,
  // Thread dead
  THREAD_TERMINATED,
  // It is in the queue
  THREAD_READY
};

struct thread {
  struct context context;
  uint64_t tid;
  // Used for freeing the stack
  void *stack;
  __attribute__((noreturn)) void (*entry)(void);
  pt_t page_tables;
  enum thread_state state;
};

// Arch dependent switch
void __switch_context(struct context *old_ctx, struct context *new_ctx);
void __switch_pages(pt_t tables);
int __pages_null(pt_t tables);
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
pt_t __null_pages(void);

// Switch
void switch_threads(struct thread *old_thread, struct thread *new_thread);
void switch_tail(struct thread *old_thread, struct thread *new_thread);
void thread_trampoline(struct thread *old_thread, struct thread *new_thread);

// Utils
struct thread *get_cur_thread(void);

// Lifecycle
// Puts the thread into thread queues
struct thread *create_thread(NORETURN void (*entry)(void));
void destroy_thread(struct thread *thread);

// Init
void init_general_threading(void);
void init_threading(void *stack);

#endif
