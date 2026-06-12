#include "kernel/threads.h"
#include "arch/threads.h"
#include "kernel/mem.h"
#include <stdint.h>

void __create_context(struct thread *thread) {
  struct context *context = &thread->context;
  uint64_t *rsp =
      (uint64_t *)((uintptr_t)thread->stack + (1 << STACK_ORDER) * PAGE_SIZE);

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
