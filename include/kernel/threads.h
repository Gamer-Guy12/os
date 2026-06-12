#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include <stdint.h>

#ifdef _x86_64_
typedef uint64_t tid_t;
#else
typedef uint32_t tid_t;
#endif

// How big the stack is
#define STACK_ORDER 1

struct thread {
  struct context context;
  tid_t tid;
  struct thread *prev;
  void (*entry)(void *);
  // Parameter passed in at entry
  void *param;
  // Used for freeing
  void *stack;
  // Page tables
  pt_t pages;
};

void init_threading(void *stack);

// Arch specific
void __switch_context(struct context *old, struct context *new);
void __switch_pages(pt_t pages);
// Returns a null page table
pt_t __pages_null(void);
// Gets the current page tables
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
void __switch_stacks(void (*entry)(void *));

// Switching
void switch_threads(struct thread *old, struct thread *new);
// Called at the end of a thread switch
void switch_tail(void);

// Lifecycle
struct thread *create_thread(void (*entry)(void *), void *param);
void thread_trampoline(void);

// Util
struct thread *get_cur_thread(void);

#endif
