#ifndef MEMORY_H
#define MEMORY_H

#include <decls.h>
#include <libk/spinlock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Allocate a physical page
void WUNUSED *phys_alloc(void);

/// Free a physical page
void phys_free(void *addr);

typedef struct gheap_entry_struct {
  size_t size;
  struct gheap_entry_struct *next;
  struct gheap_entry_struct *prev;
  size_t padding;
} gheap_entry_t;

void WUNUSED *gmalloc(size_t size);
void gfree(void *ptr);

void *increment_global_brk(size_t amount);
void *decrement_global_brk(size_t amount);
void init_global_brk(void);

void *create_new_kernel_stack(size_t *stack_index);
/// Returns the first addr to delete
///
/// The second one is the return value + page size
void *delete_kernel_stack(size_t stack_index);

#endif
