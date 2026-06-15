#include "kernel/threads.h"
#include "arch/threads.h"
#include "kernel/mem.h"
#include <stddef.h>
#include <stdint.h>

void __create_context(struct thread *thread) {
  struct context *context = &thread->context;
  uint64_t *rsp =
      (uint64_t *)((uintptr_t)thread->stack + (1 << STACK_ORDER) * PAGE_SIZE);

  // Fake RIP
  //
  // This is here because it is supposed to imitate the rip that will be pushed
  // on by the call function so that functions get the alignment they want
  *(--rsp) = 0;
  // RIP
  *(--rsp) = (uint64_t)thread_trampoline;
  // RBX
  *(--rsp) = 0;
  // RBP
  *(--rsp) = 0;
  // R12
  *(--rsp) = 0;
  // R13
  *(--rsp) = 0;
  // R14
  *(--rsp) = 0;
  // R15
  *(--rsp) = 0;
  // RFLAGS
  *(--rsp) = 0x202;

  context->rsp = rsp;
}

extern void __do_stack_switch(void (*entry)(void *), void *stack,
                              size_t stack_size);

void __switch_stacks(void (*entry)(void *)) {
  __do_stack_switch(entry, _alloc_pages(STACK_ORDER, ZONE_ANY),
                    (1 << STACK_ORDER) * PAGE_SIZE);
}
