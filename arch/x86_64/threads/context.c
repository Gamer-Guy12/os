#include "kernel/threads.h"
#include "arch/threads.h"
#include <stdint.h>

void __clone_context(struct context *old_ctx, struct context *new_ctx) {
  new_ctx->cr3 = old_ctx->cr3;

  struct thread* old_thread = (void*)((uintptr_t)old_ctx - offsetof(struct thread, context));
  struct thread* new_thread = (void*)((uintptr_t)new_ctx - offsetof(struct thread, context));
  uintptr_t stack_offset = (uintptr_t)old_ctx->rsp - (uintptr_t)old_thread->stack;
  new_ctx->rsp = (void*)((uintptr_t)new_thread->stack + stack_offset);
}
