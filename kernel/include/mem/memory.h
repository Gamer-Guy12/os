#ifndef MEMORY_H
#define MEMORY_H

#include <libk/bst.h>
#include <libk/spinlock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct vmm_region_struct vmm_region_t;

typedef struct vmm_mmap_region_struct {
  vmm_region_t *region;
} vmm_mmap_region_t;

/// This is for user mode
typedef struct vmm_region_struct {
  /// No need to store the stack top that is stored in rsp
  void *stack_bottom;

  void *start_brk;
  void *brk;

  void *start_code;
  void *end_code;

  void *start_data;
  void *end_data;

  void *start_read_only;
  void *end_read_only;

  void *start_autogen;
  void *end_autogen;

  void *start_mmap;
  void *end_mmap;

  spinlock_t stack_lock;
  spinlock_t brk_lock;
  spinlock_t code_lock;
  spinlock_t data_lock;
  spinlock_t read_only_lock;
  spinlock_t autogen_lock;
  spinlock_t mmap_lock;
  spinlock_t global_lock;

  // Stack doesn't rlly need a lock but ill leve it.
  // The break just moves linearlly so that doesn't needmore
  // Code is allocated statically so that doesn' need more
  // Data is same as code
  // Readonly is same as code
  // Autogen is the same as break
  // Mmap is different and needs information on where to alloc pages, these are
  // stored in a bst with the key being the addr
  bst_node_t *mmap_regions;
} vmm_region_t;

/// This is for the kernel
/// brk is at the bottom of address space
/// MMap grows up to the top
/// Autogen grows down to meet the break
typedef struct vmm_kernel_region_struct {
  void *start_brk;
  void *brk;

  void *start_mmap;
  void *end_mmap;

  // Start is the higher address and it grows down
  void *start_autogen;
  void *end_autogen;

  spinlock_t autogen_lock;
  spinlock_t mmap_lock;
  spinlock_t brk_lock;
  spinlock_t global_lock;

  bst_node_t *mmap_regions;
} vmm_kernel_region_t;

/// Allocate a physical page
void *phys_alloc(void);

/// Free a physical page
void phys_free(void *addr);

// Virtual Memory Management
// see vmm.txt

// Kmalloc (kernel malloc)
typedef enum {
  /// Tells the allocator to not go to the page mapping if this is big
  KMALLOC_BRK = 1,
} kmalloc_flags_t;

/// Use like malloc but for kernel
void *kmalloc(size_t size, uint8_t flags);

/// Use like free but for kernel
void kfree(void *addr);

/// Kernel side
void create_kernel_region(vmm_kernel_region_t *region);
void *increment_kernel_brk(vmm_kernel_region_t *region, uint64_t amount);
void *decrement_kernel_brk(vmm_kernel_region_t *region, uint64_t amount);
void *move_kernel_brk(vmm_kernel_region_t *region, int64_t amount);

void *increment_global_brk(size_t amount);
void *decrement_global_brk(size_t amount);
void init_global_brk(void);

#endif
