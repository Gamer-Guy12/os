#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "util.h"

// Holds the current thread for each core
CLS(struct thread *, cpu_thread);

void switch_threads(struct thread *old, struct thread *new) {
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
  WMEMB();
  enable_interrupts();
}

struct thread *get_cur_thread(void) {
  struct thread **thread = GET_CLS(cpu_thread);
  return *thread;
}
