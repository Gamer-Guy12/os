#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "util.h"
#include <stdint.h>

// Holds the current thread for each core
CLS(struct thread *, cpu_thread);

static tid_t cur_tid = 0;
#define GET_TID __atomic_fetch_add(&cur_tid, 1, __ATOMIC_ACQ_REL)

static GHEAP_CACHE(thread_cache);

void init_threading(void *stack) {
  BSP { gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY); }
  init_queues();

  struct thread *thread = gheap_cache_alloc(&thread_cache);
  thread->tid = GET_TID;
  thread->pages = __pages_null();
  thread->stack = stack;
  thread->state = THREAD_RUNNING;
  thread->priority = THREAD_HIGH;

  struct thread **cur_thread = GET_CLS(cpu_thread);
  *cur_thread = thread;
}

void switch_threads(struct thread *old, struct thread *new) {
  if (new == old) {
    kprintf("Cannot switch to same thread: 0x%x\n", old->tid);
    panic();
  }

  disable_interrupts();
  RMEMB();
  struct thread **thread = GET_CLS(cpu_thread);
  new->prev = old;
  *thread = new;

  if (new->pages != __pages_null() && new->pages != old->pages)
    __switch_pages(new->pages);
  __switch_context(&old->context, &new->context);
  switch_tail();
}

void switch_tail(void) {
  struct thread *new = get_cur_thread();
  struct thread *old = new->prev;
  switch (old->state) {
  case THREAD_RUNNING:
    old->state = THREAD_READY;
    schedule_thread(old, old->priority);
    break;
  case THREAD_READY:
    kprintf("Invalid thread readiness after running\n");
    panic();
  case THREAD_TERMINATED:
    // Handle death
    destroy_thread(old);
    break;
  default:
    kprintf("Invalid thread state: 0x%x\n", old->state);
    panic();
  }

  new->state = THREAD_RUNNING;
  WMEMB();
  enable_interrupts();
}

struct thread *create_thread(void (*entry)(void *), void *param, int priority) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);
  if (!thread)
    return NULL;

  thread->tid = GET_TID;
  thread->pages = __pages_null();
  thread->entry = entry;
  thread->param = param;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  thread->state = THREAD_READY;
  __create_context(thread);

  thread->priority = priority;
  if (priority != THREAD_NO_SCHED)
    schedule_thread(thread, thread->priority);

  return thread;
}

void destroy_thread(struct thread *thread) {
  RMEMB();
  free_pages(thread->stack, STACK_ORDER);
  WMEMB();
}

void thread_trampoline(void) {
  switch_tail();
  struct thread *thread = get_cur_thread();
  thread->entry(thread->param);
}

struct thread *get_cur_thread(void) {
  struct thread **thread = GET_CLS(cpu_thread);
  return *thread;
}
