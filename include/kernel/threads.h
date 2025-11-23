#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "util.h"
#include <stdint.h>

#define STACK_ORDER 2

enum thread_state {
  THREAD_RUNNING,
  THREAD_WAITING,
  THREAD_TERMINATED,
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
  // Last cpu this was run on
  uint32_t cpu_id;
};

void __switch_context(struct context *old_ctx, struct context *new_ctx);
void __switch_pages(pt_t tables);
int __pages_null(pt_t tables);
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
void switch_threads(struct thread *old_thread, struct thread *new_thread);
void thread_trampoline(struct thread *old_thread, struct thread *new_thread);
struct thread *get_cur_thread(void);
struct thread *create_thread(NORETURN void (*entry)(void));
void init_threading(void);

// Thread Switching full process
//
// call switch_threads
// Do switch preperation
//  - Get next process
//  - other stuff
// __switch_pages
// __switch_context
// switch_tail
//
// for the thread trampoline:
// it resumes after __switch_context is done
// it calls switch_tail

#endif
