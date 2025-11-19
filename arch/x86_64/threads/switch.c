#include "arch/threads.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "asm.h"
#include "kernel/threads.h"
#include "x86_64.h"
#include <stddef.h>

extern void *swap_regs(void *rsp);

void __switch_context(struct context *old_ctx, struct context *new_ctx) {
  old_ctx->fs = rdmsr(FS_BASE_MSR);
  old_ctx->gs = rdmsr(GS_BASE_MSR);

  WRMSR(FS_BASE_MSR, new_ctx->fs);
  WRMSR(GS_BASE_MSR, new_ctx->gs);

  kprintf("%p\n", new_ctx->rsp);
  while (1) {}
  old_ctx->rsp = swap_regs(new_ctx->rsp);
}

void __switch_page_tables(pt_t new_tables) {
  if (new_tables == NULL)
    return;

  pt_t old_cr3;
  __asm__ volatile("mov %%cr3, %0" : "=r"(old_cr3));

  if (old_cr3 == new_tables)
    return;

  __asm__ volatile("mov %0, %%cr3" ::"r"(new_tables) : "memory");
}

void __create_context(void (*entry)(void), struct context *context) {
  context->fs = 0;
  context->gs = 0;
  context->rsp = (void*)((uintptr_t)alloc_pages(0, ZONE_ANY) + PAGE_SIZE - sizeof(struct registers));
  context->regs->rip = (uint64_t)entry;
  context->regs->rflags = 0x202;
}
