#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include <stdint.h>

#ifdef _x86_64_
typedef uint64_t tid_t;
#else
typedef uint32_t tid_t;
#endif

struct thread {
  struct context context;
  tid_t tid;
  struct thread *prev;
  // Used for freeing
  void *stack;
  // Page tables
  pt_t pages;
};

// Arch specific
void __switch_context(struct context *old, struct context *new);
void __switch_pages(pt_t pages);
// Returns a null page table
pt_t __pages_null(void);
// Gets the current page tables
pt_t __cur_pages(void);

// Switching
void switch_threads(struct thread *old, struct thread *new);
// Called at the end of a thread switch
void switch_tail(void);

struct thread *get_cur_thread(void);

#endif
