#include "arch/threads.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include <stdint.h>

void __create_context(struct thread *thread) {
  thread->context.rsp =
      (void *)((uintptr_t)thread->stack + (PAGE_SIZE * (1 << STACK_ORDER)));

  uint64_t *sp = thread->context.rsp;

  // RIP
  *(--sp) = (uint64_t)thread_trampoline;
  // R15
  *(--sp) = 0;
  // R14
  *(--sp) = 0;
  // R13
  *(--sp) = 0;
  // R12
  *(--sp) = 0;
  // RBP
  *(--sp) = 0;
  // RBX
  *(--sp) = 0;
  // RFLAGS
  *(--sp) = 0x202;

  thread->context.rsp = sp;
}
