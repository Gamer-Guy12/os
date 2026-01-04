#include "kernel/threads.h"
#include <stdint.h>

// The stack in the thread is already copied but not in the context
// Its values are also copied
void __copy_context(struct thread *old_ctx, struct thread *new_ctx) {
  // The entire context is stored on the stack so we just have to update the pointers
  uintptr_t offset = (uintptr_t)old_ctx->context.rsp - (uintptr_t)old_ctx->stack;
  uintptr_t new_val = (uintptr_t)new_ctx->stack + offset;
  new_ctx->context.rsp = (void *)new_val;
}

