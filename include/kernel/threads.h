#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include <stdbool.h>
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
  // Don't put anything before the context
  struct context context;
  // Used for freeing the stack
  void *stack;
  void (*entry)(void);
  pt_t page_tables;
  enum thread_state state;
};

// Arch dependent switch
// The parameter registers at the end should contain the old and new threads
// Ex: (on x86_64) rdi: old thread, rsi: new thread
void __switch_context(struct context *old_ctx, struct context *new_ctx);
void __switch_pages(pt_t tables);
int __pages_null(pt_t tables);
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
void __copy_context(struct thread *old_ctx, struct thread *new_ctx);
pt_t __null_pages(void);

// Utils
struct thread *get_cur_thread(void);

// Switch
void switch_threads(struct thread *old_thread, struct thread *new_thread);
void switch_tail(struct thread *old_thread, struct thread *new_thread);
void thread_trampoline(struct thread *old_thread, struct thread *new_thread);

// Init
void init_threading(void *stack);

// Creation
struct thread *create_thread(void (*entry)(void));
void destroy_thread(struct thread *thread);

#endif
