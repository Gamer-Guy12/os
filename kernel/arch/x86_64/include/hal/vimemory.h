#ifndef X86_64_VIMEMORY_H
#define X86_64_VIMEMORY_H

#include <libk/bst.h>
#include <libk/lock.h>
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
  void *end_brk;

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

/// Gets all the physical addresses necessary
/// Returns the address u are mapping
/// If its different then there was an error
void *map_page(void *addr, uint16_t flags, bool not_executable);
void *unmap_page(void *addr);

/// Kernel side
void create_kernel_region(vmm_kernel_region_t *region);
void *increment_kernel_brk(vmm_kernel_region_t *region, uint64_t amount);
void *decrement_kernel_brk(vmm_kernel_region_t *region, uint64_t amount);

#endif
