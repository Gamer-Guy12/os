#include "kernel/mem.h"
#include <stddef.h>
#include <stdint.h>

// These are stored as virtual addresses
struct fmem_node {
  struct fmem_node *next;
};

uintptr_t hhdm_offset = 0;

struct fmem_node *freelist = NULL;

void init_fmem(uintptr_t direct_offset) {
  hhdm_offset = direct_offset;
  // kprintf("\t[MEM] Initialized FMem\n");
}

void *fmem_palloc(void) {
  if (freelist == NULL) {
    return NULL;
  }

  uintptr_t ptr = (uintptr_t)freelist - hhdm_offset;
  freelist = freelist->next;

  return (void *)ptr;
}

void fmem_pfree(void *addr) {
  if (addr == NULL) {
    return;
  }

  uintptr_t ptr = (uintptr_t)addr + hhdm_offset;
  struct fmem_node *node = (struct fmem_node *)ptr;

  node->next = freelist;
  freelist = node;
}

void *fmem_valloc(void) {
  void *addr = fmem_palloc();

  if (addr == NULL) {
    return NULL;
  }

  uintptr_t ptr = (uintptr_t)addr;
  ptr += hhdm_offset;

  return (void *)ptr;
}

void fmem_vfree(void *addr) {
  if (addr == NULL) {
    return;
  }

  uintptr_t ptr = (uintptr_t)addr;
  ptr -= hhdm_offset;

  fmem_pfree((void *)ptr);
}

// End should be the byte after the last byte
void fmem_pfree_range(void *start, void *end) {
  uintptr_t start_ptr = (uintptr_t)start;
  uintptr_t end_ptr = (uintptr_t)end;
  start_ptr = ((start_ptr - 1) / PAGE_SIZE + 1) * PAGE_SIZE;
  end_ptr = (end_ptr & ~(PAGE_SIZE - 1));
  if (start_ptr >= end_ptr) {
    // Start is > End when they are both in the same page (or someone made an
    // error) Start is == End when they are in consecutive pages but don't fill
    // either of them up
    return;
  }

  size_t count = (end_ptr - start_ptr) / PAGE_SIZE;

  for (size_t i = 0; i < count; i++) {
    fmem_pfree((void *)(start_ptr + PAGE_SIZE * i));
  }
}
